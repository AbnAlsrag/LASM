#define LVM_IMPLEMENTATION
#include <LVM.h>

#ifdef _WIN32
#define WINPAUSE system("pause")
#else
#define WINPAUSE
#endif

#define LABEL_MAX 1024

typedef struct String_View {
	char* buffer;
	int64_t size;
} String_View;

typedef struct Label {
	String_View str;
	uint64_t op_count;
} Label;

typedef struct DeferedLabel {
	String_View label;
	LVM_OperationType op_type;
	uint64_t op_addr;
} DeferedLabel;


typedef struct LabelManger {
	Label labels[LABEL_MAX];
	uint64_t label_size;
	DeferedLabel calls[LABEL_MAX];
	uint64_t call_size;
} LabelManger;

LabelManger labels = { 0 };

char* read_file(char* path) {
	FILE* file = fopen(path, "rb");

	if (file == NULL) {
		fprintf(stderr, "Error: Couldn't open file %s: %s\n", path, strerror(errno));
		exit(1);
	}

	if (fseek(file, 0, SEEK_END) < 0) {
		fprintf(stderr, "Error: Couldn't read file %s: %s\n", path, strerror(errno));
		exit(1);
	}

	size_t file_size = ftell(file);

	if (file_size < 0) {
		fprintf(stderr, "Error: Couldn't read file %s: %s\n", path, strerror(errno));
		exit(1);
	}

	if (fseek(file, 0, SEEK_SET) < 0) {
		fprintf(stderr, "Error: Couldn't read file %s: %s\n", path, strerror(errno));
		exit(1);
	}

	char* buffer = (char*)malloc((sizeof(char) * file_size) + 1);

	if (buffer == NULL) {
		fprintf(stderr, "Error: Couldn't allocate buffer %s: %s\n", path, strerror(errno));
		exit(1);
	}

	fread(buffer, sizeof(char), file_size / sizeof(char), file);
	buffer[file_size] = '\0';

	fclose(file);

	return buffer;
}

inline uint8_t is_alpha(char c) {
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'));
}

inline uint8_t is_num(char c) {
	return (c >= '0' && c <= '9');
}

inline uint8_t is_white_space(char c) {
	return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

inline uint8_t is_float(String_View str) {
	for (uint64_t i = 0; i < str.size && (is_num(str.buffer[i]) || str.buffer[i] == '.'); i++) {
		if (str.buffer[i] == '.') {
			return 1;
		}
	}

	return 0;
}

inline String_View sv_create(char* buffer, uint64_t size, uint64_t offset) {
	return (String_View) { .buffer = buffer + offset, .size = size };
}

inline void sv_lchop(String_View* str) {
	str->buffer++;
	str->size--;
}

inline void sv_rchop(String_View* str) {
	str->size--;
}

inline void sv_lstrip(String_View* str) {
	while (is_white_space(str->buffer[0])) {
		sv_lchop(str);
	}
}

inline void sv_rstrip(String_View* str) {
	while (is_white_space(str->buffer[str->size - 1])) {
		sv_rchop(str);
	}
}

int8_t sv_compare(String_View str1, String_View str2) {
	if (str1.size != str2.size) {
		return 0;
	}

	for (uint64_t i = 0; i < str1.size; i++) {
		if (str1.buffer[i] != str2.buffer[i]) {
			return 0;
		}
	}

	return 1;
}

String_View sv_get_line(String_View* str) {
	uint64_t i = 0;
	for (; i < str->size && str->buffer[i] != '\n'; i++) {

	}

	if (str->size < 1) {
		return (String_View) { .buffer = NULL, .size = 0 };
	}

	str->buffer += i + 1;
	str->size -= i + 1;

	return (String_View) { .buffer = str->buffer - (i + 1), .size = i };
}

inline uint8_t sv_is_text(String_View str) {
	uint64_t i = 0;
	for (; i < str.size && is_alpha(str.buffer[i]); i++) {

	}

	if (i > 0) {
		return 1;
	}

	return 0;
}

String_View sv_parse_text(String_View* str) {
	uint64_t i = 0;
	for (; i < str->size && is_alpha(str->buffer[i]); i++) {

	}

	if (str->size < 1) {
		return (String_View) { .buffer = NULL, .size = 0 };
	}

	str->buffer += i;
	str->size -= i;

	return (String_View) { .buffer = str->buffer - (i), .size = i };
}

inline uint8_t sv_is_label(String_View str) {
	if (sv_parse_text(&str).buffer) {
		sv_lstrip(&str);
		if (str.size > 0 && str.buffer[0] == ':') {
			return 1;
		}
	}

	return 0;
}

String_View sv_parse_label(String_View* str) {
	String_View label = sv_parse_text(str);
	sv_lchop(str);
	sv_lstrip(str);
	return label;
}

int64_t sv_parse_int(String_View* str) {
	uint8_t negate = 0;
	if (str->buffer[0] == '-') {
		negate = 1;
		sv_lchop(str);
	}

	uint64_t i = 0;
	int64_t num = 0;
	for (; i < str->size && is_num(str->buffer[i]); i++) {
		num = num * 10 + (int64_t)(str->buffer[i] - '0');
	}

	str->buffer += i;
	str->size -= i;

	if (negate) {
		num *= -1;
	}

	return num;
}

double sv_parse_float(String_View* str) {
	uint8_t negate = 0;
	if (str->buffer[0] == '-') {
		negate = 1;
		sv_lchop(str);
	}

	uint64_t i = 0, current= 0;
	uint8_t after_point = 0;
	double num = 0;
	for (; i < str->size && (is_num(str->buffer[i]) || str->buffer[i] == '.'); i++) {
		if (str->buffer[i] == '.') {
			after_point = 1;
			current = i;
		}
		else if(!after_point)
		{
			num = num * 10 + (int64_t)(str->buffer[i] - '0');
		}
		else if (after_point) {
			double n = (str->buffer[i] - '0');
			double c = n / pow(10, (i - current));
			num += n / pow(10, (i - current));
		}
	}

	str->buffer += i;
	str->size -= i;

	if (negate) {
		num *= -1;
	}

	return num;
}

inline int32_t is_operation(String_View str) {
	for (uint64_t i = 0; i < NUMBER_OF_OPERATIONS; i++) {
		if (sv_compare(sv_create(LVM_operations_name[i], strlen(LVM_operations_name[i]), 0), str)) {
			return i;
		}
	}
	
	return -1;
}

inline uint64_t label_exist(String_View str) {
	for (uint64_t i = 0; i < labels.label_size; i++) {
		if (sv_compare(labels.labels[i].str, str)) {
			return i;
		}
	}

	return -1;
}

void compute_labels(LVM* lvm) {
	for (uint64_t i = 0; i < labels.call_size; i++) {
		DeferedLabel call = labels.calls[i];
		int64_t exist = label_exist(call.label);
		if (exist > -1) {
			Label label = labels.labels[exist];
			lvm->program[call.op_addr] = (LVM_Operation){ .type = call.op_type, .operand = {.as_u64 = label.op_count} };
		}
	}
}

void parse_program_from_file(LVM* lvm, const char* path) {
	char* file_data = read_file(path);
	String_View source = sv_create(file_data, strlen(file_data), 0);
	uint64_t op_count = 0;

	for (uint64_t i = 0; i < LVM_PROGRAM_MAX && source.size > 0; i++) {
		sv_lstrip(&source);
		if (source.buffer[0] == '#') {
			sv_get_line(&source);
		}

		if (sv_is_label(source)) {
			String_View label_txt = sv_parse_label(&source);
			labels.labels[labels.label_size++] = (Label){ .str = label_txt, .op_count = op_count };
		}
		else {
			String_View txt = sv_parse_text(&source);
			int32_t is_op = is_operation(txt);
			if (is_op > -1) {
				op_count++;
				sv_lstrip(&source);
				if (LVM_operation_has_operand(is_op)) {
					if (sv_compare(txt, sv_create(LVM_operations_name[LVM_OP_PUSH], strlen(LVM_operations_name[LVM_OP_PUSH]), 0))) {
						if (is_float(source)) {
							lvm->program[lvm->program_size++] = (LVM_Operation)LVM_CREATE_PUSH_OP(sv_parse_float(&source), f64);
						}
						else {
							lvm->program[lvm->program_size++] = (LVM_Operation)LVM_CREATE_PUSH_OP(sv_parse_int(&source), i64);
						}
					}
					else {
						if (sv_is_text(source)) {
							labels.calls[labels.call_size++] = (DeferedLabel){ .label = sv_parse_text(&source), .op_type = is_op, .op_addr = lvm->program_size++ };
						}
						else {
							lvm->program[lvm->program_size++] = (LVM_Operation){ .type = is_op, .operand = { .as_i64 = sv_parse_int(&source) } };
						}
					}
				}
				else {
					lvm->program[lvm->program_size++] = (LVM_Operation){ .type = is_op };
				}
			}
		}
	}

	compute_labels(lvm);
}

void command(char** args, int count) {

}

LVM lvm = { 0 };

int main(int argc, char** argv) {
	command(argv, argc);

	const char* input_path = "examples/example-3.lasm";
	const char* output_path = "examples/bin/example-3.melf";

	parse_program_from_file(&lvm, input_path);
	LVM_save_program_to_file(&lvm, output_path);

	WINPAUSE;
	return 0;
}