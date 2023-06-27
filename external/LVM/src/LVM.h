#ifndef LVM_H
#define LVM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define LVM_STACK_MAX 1024
#define LVM_HEAP_MAX 1024
#define LVM_PROGRAM_MAX 1024
#define LVM_NATIVE_MAX 1024

typedef union Word {
	int64_t as_i64;
	uint64_t as_u64;
	double as_f64;
	void* as_ptr;
} Word;

typedef uint64_t LVM_OpAddr;

typedef enum LVM_Types {
	LVM_ANY,
	LVM_INT,
	LVM_FLOAT,
} LVM_Types;

typedef enum LVM_ExceptionType {
	LVM_ET_OK,
	LVM_ET_ILLEGAL_OP,
	LVM_ET_ILLEGAL_OP_ACCESS,
	LVM_ET_ILLEGAL_OPERAND,
	LVM_ET_STACK_OVERFLOW,
	LVM_ET_STACK_UNDERFLOW,
	LVM_ET_DIV_BY_ZERO,
} LVM_ExceptionType;

typedef enum LVM_OperationType {
	LVM_OP_NOP = 0,
	LVM_OP_PUSH,
	LVM_OP_POP,
	LVM_OP_DUP,
	LVM_OP_SWAP,
	LVM_OP_ADD,
	LVM_OP_ADDF,
	LVM_OP_SUB,
	LVM_OP_SUBF,
	LVM_OP_MULT,
	LVM_OP_MULTF,
	LVM_OP_DIV,
	LVM_OP_DIVF,
	LVM_OP_CALL,
	LVM_OP_NATIVE,
	LVM_OP_RETURN,
	LVM_OP_JMP,
	LVM_OP_JMP_IF,
	LVM_OP_EQ,
	LVM_OP_HLT,
	LVM_OP_PRINT_DEBUG,
	NUMBER_OF_OPERATIONS,
} LVM_OperationType;

typedef struct LVM_Operation {
	LVM_OperationType type;
	Word operand;
} LVM_Operation;

typedef struct LVM LVM;

typedef LVM_ExceptionType(*LVM_Native)(LVM*);

struct LVM {
	Word stack[LVM_STACK_MAX];
	uint64_t stack_size;

	//TODO: add heap
	//TODO: STOP!!!!! BEEING LAZY
	char* heap[LVM_HEAP_MAX];

	LVM_Operation program[LVM_PROGRAM_MAX];
	uint64_t program_size;
	LVM_OpAddr ip;

	LVM_Native natives[LVM_NATIVE_MAX];
	size_t natives_size;

	int halt;
};

#define LVM_CREATE_PUSH_OP(value, cast) { .type = LVM_OP_PUSH, .operand = { .as_##cast = value } }
#define LVM_CREATE_POP_OP() { .type = LVM_OP_POP }
#define LVM_CREATE_DUP_OP(addr) { .type = LVM_OP_DUP, .operand = { .as_u64 = addr } }
#define LVM_CREATE_SWAP_OP(addr) { .type = LVM_OP_SWAP, .operand = { .as_u64 = addr } }
#define LVM_CREATE_ADD_OP() { .type = LVM_OP_ADD }
#define LVM_CREATE_ADDF_OP() { .type = LVM_OP_ADDF }
#define LVM_CREATE_SUB_OP() { .type = LVM_OP_SUB }
#define LVM_CREATE_SUBF_OP() { .type = LVM_OP_SUBF }
#define LVM_CREATE_MULT_OP() { .type = LVM_OP_MULT }
#define LVM_CREATE_MULTF_OP() { .type = LVM_OP_MULTF }
#define LVM_CREATE_DIV_OP() { .type = LVM_OP_DIV }
#define LVM_CREATE_DIVF_OP() { .type = LVM_OP_DIVF }
#define LVM_CREATE_CALL_OP(addr) { .type = LVM_OP_CALL, .operand = { .as_u64 = addr } }
#define LVM_CREATE_NATIVE_OP(addr) { .type = LVM_OP_NATIVE, .operand = { .as_u64 = addr } }
#define LVM_CREATE_RETURN_OP() { .type = LVM_OP_RETURN }
#define LVM_CREATE_JMP_OP(addr) { .type = LVM_OP_JMP, .operand = { .as_u64 = addr } }
#define LVM_CREATE_JMP_IF_OP(addr) { .type = LVM_OP_JMP_IF, .operand = { .as_u64 = addr } }
#define LVM_CREATE_EQ_OP() { .type = LVM_OP_EQ }
#define LVM_CREATE_HLT_OP() { .type = LVM_OP_HLT }
#define LVM_CREATE_PRINT_DEBUG_OP() { .type = LVM_OP_PRINT_DEBUG }

const char* const LVM_operations_name[NUMBER_OF_OPERATIONS] = {
	[LVM_OP_NOP]            = "nop",
	[LVM_OP_PUSH]           = "push",
	[LVM_OP_POP]            = "pop",
	[LVM_OP_DUP]            = "dup",
	[LVM_OP_SWAP]           = "swap",
	[LVM_OP_ADD]            = "add",
	[LVM_OP_ADDF]           = "addf",
	[LVM_OP_SUB]            = "sub",
	[LVM_OP_SUBF]           = "subf",
	[LVM_OP_MULT]           = "mult",
	[LVM_OP_MULTF]          = "multf",
	[LVM_OP_DIV]            = "div",
	[LVM_OP_DIVF]           = "divf",
	[LVM_OP_CALL]           = "call",
	[LVM_OP_NATIVE]         = "native",
	[LVM_OP_RETURN]         = "return",
	[LVM_OP_JMP]            = "jmp",
	[LVM_OP_JMP_IF]         = "jmp_if",
	[LVM_OP_EQ]             = "eq",
	[LVM_OP_HLT]            = "hlt",
	[LVM_OP_PRINT_DEBUG]    = "print_debug",
};

const uint8_t LVM_operations_has_operand[NUMBER_OF_OPERATIONS] = {
	[LVM_OP_PUSH]   = 1,
	[LVM_OP_DUP]    = 1,
	[LVM_OP_SWAP]   = 1,
	[LVM_OP_CALL]   = 1,
	[LVM_OP_NATIVE] = 1,
	[LVM_OP_JMP]    = 1,
	[LVM_OP_JMP_IF] = 1,
};

#define LVM_GET_OPERATION_TYPE_NAME(operation) "OPERATION "#operation

const char* LVM_exception_value(LVM_ExceptionType type);
inline const char* LVM_operation_name(LVM_OperationType type);
inline uint8_t LVM_operation_has_operand(LVM_OperationType type);
LVM_ExceptionType LVM_execute_operation(LVM* lvm);
LVM_ExceptionType LVM_execute_program(LVM* lvm, int limit);
void LVM_push_native(LVM* lvm, LVM_Native native);
void LVM_dump_stack(FILE* stream, const LVM* lvm);
void LVM_save_program_to_file(const LVM* lvm, const char* file_path);
void LVM_load_program_from_memory(LVM* lvm, LVM_Operation* program, size_t program_size);
void LVM_load_program_from_file(LVM* lvm, const char* file_path);

#ifdef LVM_IMPLEMENTATION

const char* LVM_exception_value(LVM_ExceptionType type) {
	switch (type)
	{
	case LVM_ET_OK:
		return "OK";
	case LVM_ET_ILLEGAL_OP:
		return "EXCEPTION ILLEGAL LVM_Operation";
	case LVM_ET_ILLEGAL_OP_ACCESS:
		return "EXCEPTION ILLEGAL LVM_Operation ACCESS";
	case LVM_ET_ILLEGAL_OPERAND:
		return "EXCEPTION ILLEGAL OPERAND";
	case LVM_ET_STACK_OVERFLOW:
		return "EXCEPTION STACK OVERFLOW";
	case LVM_ET_STACK_UNDERFLOW:
		return "EXCEPTION STACK UNDERFLOW";
	case LVM_ET_DIV_BY_ZERO:
		return "EXCEPTION DIVISION BY ZERO";
	default:
		assert(0 && "UNVALID EXCEPTION TYPE");
		return NULL;
	}
}

inline const char* LVM_operation_name(LVM_OperationType type) {
	assert((type < NUMBER_OF_OPERATIONS && type >= 0) && "UNVALID LVM_Operation TYPE");
	return LVM_operations_name[type];
}

inline uint8_t LVM_operation_has_operand(LVM_OperationType type) {
	assert((type < NUMBER_OF_OPERATIONS && type >= 0) && "UNVALID LVM_Operation TYPE");
	return LVM_operations_has_operand[type];
}

LVM_ExceptionType LVM_execute_operation(LVM* lvm) {
	if (lvm->ip < 0 || lvm->ip >= lvm->program_size) {
		return LVM_ET_ILLEGAL_OP_ACCESS;
	}

	LVM_Operation operation = lvm->program[lvm->ip];

	switch (operation.type)
	{
	case LVM_OP_NOP:
		break;
	case LVM_OP_PUSH:
		if (lvm->stack_size >= LVM_STACK_MAX) {
			return LVM_ET_STACK_OVERFLOW;
		}

		lvm->stack[lvm->stack_size++] = operation.operand;
		break;
	case LVM_OP_POP:
		if (lvm->stack_size < 1) {
			return LVM_ET_STACK_UNDERFLOW;
		}

		lvm->stack_size--;
		break;
	case LVM_OP_DUP:
		if (operation.operand.as_i64 < 0 || operation.operand.as_u64 >= lvm->stack_size) {
			return LVM_ET_ILLEGAL_OPERAND;
		}

		if (lvm->stack_size >= LVM_STACK_MAX) {
			return LVM_ET_STACK_OVERFLOW;
		}

		if (lvm->stack_size - operation.operand.as_u64 <= 0) {
			return LVM_ET_STACK_UNDERFLOW;
		}

		lvm->stack[lvm->stack_size] = lvm->stack[(lvm->stack_size - 1) - operation.operand.as_u64];
		lvm->stack_size++;
		break;
	case LVM_OP_SWAP:
		if (lvm->stack_size < 2 || (lvm->stack_size - 2 - operation.operand.as_u64) < 0) {
			return LVM_ET_STACK_UNDERFLOW;
		}
		
		Word tmp = lvm->stack[lvm->stack_size - 1 - operation.operand.as_u64];
		lvm->stack[lvm->stack_size - 1 - operation.operand.as_u64] = lvm->stack[lvm->stack_size - 2 - operation.operand.as_u64];
		lvm->stack[lvm->stack_size - 2 - operation.operand.as_u64] = tmp;

		break;
	case LVM_OP_ADD:
		if (lvm->stack_size < 2) {
			return LVM_ET_STACK_UNDERFLOW;
		}

		lvm->stack[lvm->stack_size - 2].as_i64 += lvm->stack[lvm->stack_size - 1].as_i64;
		lvm->stack_size--;
		break;
	case LVM_OP_ADDF:
		if (lvm->stack_size < 2) {
			return LVM_ET_STACK_UNDERFLOW;
		}

		lvm->stack[lvm->stack_size - 2].as_f64 += lvm->stack[lvm->stack_size - 1].as_f64;
		lvm->stack_size--;
		break;
	case LVM_OP_SUB:
		if (lvm->stack_size < 2) {
			return LVM_ET_STACK_UNDERFLOW;
		}

		lvm->stack[lvm->stack_size - 2].as_i64 -= lvm->stack[lvm->stack_size - 1].as_i64;
		lvm->stack_size--;
		break;
	case LVM_OP_SUBF:
		if (lvm->stack_size < 2) {
			return LVM_ET_STACK_UNDERFLOW;
		}

		lvm->stack[lvm->stack_size - 2].as_f64 -= lvm->stack[lvm->stack_size - 1].as_f64;
		lvm->stack_size--;
		break;
	case LVM_OP_MULT:
		if (lvm->stack_size < 2) {
			return LVM_ET_STACK_UNDERFLOW;
		}

		lvm->stack[lvm->stack_size - 2].as_i64 *= lvm->stack[lvm->stack_size - 1].as_i64;
		lvm->stack_size--;
		break;
	case LVM_OP_MULTF:
		if (lvm->stack_size < 2) {
			return LVM_ET_STACK_UNDERFLOW;
		}

		lvm->stack[lvm->stack_size - 2].as_f64 *= lvm->stack[lvm->stack_size - 1].as_f64;
		lvm->stack_size--;
		break;
	case LVM_OP_DIV:
		if (lvm->stack_size < 2) {
			return LVM_ET_STACK_UNDERFLOW;
		}

		if (lvm->stack[lvm->stack_size - 1].as_u64 == 0) {
			return LVM_ET_DIV_BY_ZERO;
		}

		lvm->stack[lvm->stack_size - 2].as_i64 /= lvm->stack[lvm->stack_size - 1].as_i64;
		lvm->stack_size--;
		break;
	case LVM_OP_DIVF:
		if (lvm->stack_size < 2) {
			return LVM_ET_STACK_UNDERFLOW;
		}

		if (lvm->stack[lvm->stack_size - 1].as_f64 == 0) {
			return LVM_ET_DIV_BY_ZERO;
		}

		lvm->stack[lvm->stack_size - 2].as_f64 /= lvm->stack[lvm->stack_size - 1].as_f64;
		lvm->stack_size--;
		break;
	case LVM_OP_CALL:
		if (lvm->stack_size >= LVM_STACK_MAX) {
			return LVM_ET_STACK_OVERFLOW;
		}

		lvm->stack[lvm->stack_size++].as_u64 = lvm->ip;
		lvm->ip = operation.operand.as_u64 - 1;
		break;
	case LVM_OP_NATIVE:
		if (lvm->stack_size < 1) {
			return LVM_ET_STACK_UNDERFLOW;
		}

		LVM_ExceptionType exception = lvm->natives[operation.operand.as_u64](lvm);
		lvm->ip++;
		return exception;
	case LVM_OP_RETURN:
		if (lvm->stack_size < 1) {
			return LVM_ET_STACK_UNDERFLOW;
		}

		lvm->ip = lvm->stack[--lvm->stack_size].as_u64;
		break;
	case LVM_OP_JMP:
		lvm->ip = operation.operand.as_u64 - 1;
		break;
	case LVM_OP_JMP_IF:
		if (lvm->stack_size < 1) {
			return LVM_ET_STACK_UNDERFLOW;
		}

		if (lvm->stack[lvm->stack_size - 1].as_u64) {
			lvm->stack_size--;
			lvm->ip = operation.operand.as_u64 - 1;
		}
		break;
	case LVM_OP_EQ:
		if (lvm->stack_size < 2) {
			return LVM_ET_STACK_UNDERFLOW;
		}

		lvm->stack[lvm->stack_size - 2].as_i64 = lvm->stack[lvm->stack_size - 2].as_i64 == lvm->stack[lvm->stack_size - 1].as_i64;
		lvm->stack_size--;
		break;
	case LVM_OP_HLT:
		lvm->halt = 1;
		break;
	case LVM_OP_PRINT_DEBUG:
		if (lvm->stack_size < 1) {
			return LVM_ET_STACK_UNDERFLOW;
		}

		printf("%lld\n", lvm->stack[lvm->stack_size - 1].as_i64);
		lvm->stack_size--;
		break;
	case NUMBER_OF_OPERATIONS:
	default:
		return LVM_ET_ILLEGAL_OP;
		break;
	}

	lvm->ip++;
	return LVM_ET_OK;
}

LVM_ExceptionType LVM_execute_program(LVM* lvm, int limit) {
	for (; limit != 0 && !lvm->halt; ) {
		printf(":Operation: %s:\n", LVM_operation_name(lvm->program[lvm->ip].type));
		LVM_ExceptionType exception = LVM_execute_operation(lvm);
		LVM_dump_stack(stdout, lvm);

		if (exception != LVM_ET_OK) {
			return exception;
		}

		if (limit > 0) {
			limit--;
		}
	}

	return LVM_ET_OK;
}

void LVM_push_native(LVM* lvm, LVM_Native native) {
	assert(lvm->natives_size < LVM_NATIVE_MAX);
	lvm->natives[lvm->natives_size++] = native;
}

void LVM_dump_stack(FILE* stream, const LVM* lvm) {
	fprintf(stream, "Stack:\n");

	if (lvm->stack_size > 0) {
		for (uint64_t i = 0; i < lvm->stack_size; i++) {
			fprintf(stream, "  i64:%lld, u64:%llu, f64:%lf, ptr:%p\n",
				lvm->stack[i].as_i64, lvm->stack[i].as_u64, lvm->stack[i].as_f64, lvm->stack[i].as_ptr);
		}
	}
	else {
		fprintf(stream, "  [THE STACK IS EMPTY]\n");
	}
}

void LVM_save_program_to_file(const LVM* lvm, const char* file_path) {
	FILE* file = fopen(file_path, "wb");

	if (file == NULL) {
		fprintf(stderr, "Error: Couldn't open file %s: %s\n", file_path, strerror(errno));
		exit(1);
	}

	fwrite(lvm->program, sizeof(lvm->program[0]), lvm->program_size, file);

	if (ferror(file)) {
		fprintf(stderr, "Error: Couldn't write to file %s: %s\n", file_path, strerror(errno));
		exit(1);
	}

	fclose(file);
}

void LVM_load_program_from_memory(LVM* lvm, LVM_Operation* program, size_t program_size) {
	assert(program_size < LVM_PROGRAM_MAX);
	memcpy(lvm->program, program, sizeof(program[0]) * program_size);
	lvm->program_size = program_size;
}

void LVM_load_program_from_file(LVM* lvm, const char* file_path) {
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

	assert(file_size % sizeof(lvm->program[0]) == 0);
	assert(file_size <= LVM_PROGRAM_MAX * sizeof(lvm->program[0]));

	fread(lvm->program, sizeof(lvm->program[0]), file_size / sizeof(lvm->program[0]), file);
	lvm->program_size = file_size / sizeof(lvm->program[0]);

	fclose(file);
}

#endif

#endif