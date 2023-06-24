#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>

#ifdef _WIN32
#define WINPAUSE system("pause")
#else
#define WINPAUSE
#endif

#define NUMBER_STRING_MAX 1024
#define PROGRAM_SIZE 1024

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define STRING_SIZE(x) sizeof(x) - 1

typedef int64_t Word;

typedef enum OperationType {
	OP_NOP,
	OP_PUSH,
	OP_POP,
	OP_DUP,
	OP_ADD,
	OP_SUB,
	OP_MULT,
	OP_DIV,
	OP_JMP,
	OP_JMP_IF,
	OP_EQ,
	OP_HLT,
	OP_PRINT_DEBUG,
} OperationType;

#define CREATE_PUSH_OP(value) (Operation){ .type = OP_PUSH, .operand = value }
#define CREATE_POP_OP() (Operation){ .type = OP_POP }
#define CREATE_DUP_OP(addr) (Operation){ .type = OP_DUP, .operand = addr }
#define CREATE_ADD_OP() (Operation){ .type = OP_ADD }
#define CREATE_SUB_OP() (Operation){ .type = OP_SUB }
#define CREATE_MULT_OP() (Operation){ .type = OP_MULT }
#define CREATE_DIV_OP() (Operation){ .type = OP_DIV }
#define CREATE_JMP_OP(addr) (Operation){ .type = OP_JMP, .operand = addr }
#define CREATE_JMP_IF_OP(addr) (Operation){ .type = OP_JMP_IF, .operand = addr }
#define CREATE_EQ_OP() (Operation){ .type = OP_EQ }
#define CREATE_HLT_OP() (Operation){ .type = OP_HLT }
#define CREATE_PRINT_DEBUG_OP() (Operation){ .type = OP_PRINT_DEBUG }

typedef struct Operation {
	OperationType type;
	Word operand;
} Operation;

Operation* generate_program_from_file(const char* file_path, size_t* program_size) {
	FILE* file = fopen(file_path, "rb");

	if (file == NULL) {
		fprintf(stderr, "Error: Couldn't open file %s: %s\n", file_path, strerror(errno));
		exit(1);
	}

	if (fseek(file, 0, SEEK_END) < 0) {
		fprintf(stderr, "Error: Couldn't read file %s: %s\n", file_path, strerror(errno));
		exit(1);
	}

	size_t file_size = ftell(file);

	if (file_size < 0) {
		fprintf(stderr, "Error: Couldn't read file %s: %s\n", file_path, strerror(errno));
		exit(1);
	}

	if (fseek(file, 0, SEEK_SET) < 0) {
		fprintf(stderr, "Error: Couldn't read file %s: %s\n", file_path, strerror(errno));
		exit(1);
	}

	char* buffer = (char*)malloc(file_size + 1);

	if (buffer == NULL) {
		fprintf(stderr, "Error: Couldn't allocate buffer %s: %s\n", file_path, strerror(errno));
		exit(1);
	}

	fread(buffer, sizeof(char), file_size, file);
	buffer[file_size] = '\0';

	fclose(file);

	size_t size = 0;
	Operation program[PROGRAM_SIZE] = { 0 };
	char* current_line = buffer;
	size_t current = 0;

	while (current < file_size) {
		if (memcmp(current_line, "push", STRING_SIZE("push")) == 0) {
			current += STRING_SIZE("push ");

			size_t i = 0;
			char number_str[NUMBER_STRING_MAX] = { 0 };

			while (buffer[current] != '\n' && buffer[current] != '\r' && buffer[current] != '\0') {
				number_str[i++] = buffer[current++];
			}

			current += 2;
			current_line = buffer + current;
			number_str[i] = '\0';
			Word number = _atoi64(number_str);

			program[size++] = CREATE_PUSH_OP(number);
		}
		else if (memcmp(current_line, "pop", STRING_SIZE("pop")) == 0) {
			program[size++] = CREATE_POP_OP();
			current += STRING_SIZE("pop") + 2;
			current_line = buffer + current;
		}
		else if (memcmp(current_line, "dup", STRING_SIZE("dup")) == 0) {
			current += STRING_SIZE("dup ");

			size_t i = 0;
			char number_str[NUMBER_STRING_MAX] = { 0 };

			while (buffer[current] != '\n' && buffer[current] != '\r' && buffer[current] != '\0') {
				number_str[i++] = buffer[current++];
			}

			current += 2;
			current_line = buffer + current;
			number_str[i] = '\0';
			Word number = _atoi64(number_str);

			program[size++] = CREATE_DUP_OP(number);
		}
		else if (memcmp(current_line, "add", STRING_SIZE("add")) == 0) {
			program[size++] = CREATE_ADD_OP();
			current += STRING_SIZE("add") + 2;
			current_line = buffer + current;
		}
		else if (memcmp(current_line, "sub", STRING_SIZE("sub")) == 0) {
			program[size++] = CREATE_SUB_OP();
			current += STRING_SIZE("sub") + 2;
			current_line = buffer + current;
		}
		else if (memcmp(current_line, "mult", STRING_SIZE("mult")) == 0) {
			program[size++] = CREATE_MULT_OP();
			current += STRING_SIZE("mult") + 2;
			current_line = buffer + current;
		}
		else if (memcmp(current_line, "div", STRING_SIZE("div")) == 0) {
			program[size++] = CREATE_DIV_OP();
			current += STRING_SIZE("div") + 2;
			current_line = buffer + current;
		}
		else if (memcmp(current_line, "jmp_if", STRING_SIZE("jmp_if")) == 0) {
			current += STRING_SIZE("jmp_if ");

			size_t i = 0;
			char number_str[NUMBER_STRING_MAX] = { 0 };

			while (buffer[current] != '\n' && buffer[current] != '\r' && buffer[current] != '\0') {
				number_str[i++] = buffer[current++];
			}

			current += 2;
			current_line = buffer + current;
			number_str[i] = '\0';
			Word number = _atoi64(number_str);

			program[size++] = CREATE_JMP_IF_OP(number);
		}
		else if (memcmp(current_line, "jmp", STRING_SIZE("jmp")) == 0) {
			current += STRING_SIZE("jmp ");

			size_t i = 0;
			char number_str[NUMBER_STRING_MAX] = { 0 };

			while (buffer[current] != '\n' && buffer[current] != '\r' && buffer[current] != '\0') {
				number_str[i++] = buffer[current++];
			}

			current += 2;
			current_line = buffer + current;
			number_str[i] = '\0';
			Word number = _atoi64(number_str);

			program[size++] = CREATE_JMP_OP(number);
		}
		else if (memcmp(current_line, "eq", STRING_SIZE("eq")) == 0) {
			program[size++] = CREATE_EQ_OP();
			current += STRING_SIZE("eq") + 2;
			current_line = buffer + current;
		}
		else if (memcmp(current_line, "hlt", STRING_SIZE("hlt")) == 0) {
			program[size++] = CREATE_HLT_OP();
			current += STRING_SIZE("hlt") + 2;
			current_line = buffer + current;
		}
		else if (memcmp(current_line, "print_debug", STRING_SIZE("print_debug")) == 0) {
			program[size++] = CREATE_PRINT_DEBUG_OP();
			current += STRING_SIZE("print_debug") + 2;
			current_line = buffer + current;
		}
		else {
			current++;
			current_line = buffer + current;
		}
	}

	*program_size = size;
	return program;
}

void compile_program(const char* file_path, Operation* program, size_t program_size) {
	FILE* file = fopen(file_path, "wb");

	if (file == NULL) {
		fprintf(stderr, "Error: Couldn't open file %s: %s\n", file_path, strerror(errno));
		exit(1);
	}

	fwrite(program, sizeof(program[0]), program_size, file);

	if (ferror(file)) {
		fprintf(stderr, "Error: Couldn't write to file %s: %s\n", file_path, strerror(errno));
		exit(1);
	}

	fclose(file);
}

int main() {
	const char* input_path = "examples/example-2.lasm";
	const char* output_path = "examples/bin/example-2.melf";
	size_t program_size = 0;

	Operation* program = generate_program_from_file(input_path, &program_size);
	compile_program(output_path, program, program_size);

	WINPAUSE;
	return 0;
}