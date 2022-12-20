#include "shannon.h"

// Address includes THUMB bit
#define PRINTBUF 0x40e895e9
#define PRINTCRLF 0x40e88a49
#define STRLEN 0x40f146a5
#define SPRINTF 0x40f0c891


const char TASK_NAME[] = "DEBUG\0";

void (*printlen)(char *, int) = (void*) PRINTBUF;
size_t (*strlen)(char*) = (void*) STRLEN;
void (*printcrlf)(void) = (void*) PRINTCRLF;
int (*sprintf)(char*, char*, ...) = (void*) SPRINTF;

/* Print len bytes as hexadecimals from specified addr. */
void print_hex(char* in, int len) {
    for (int i = 0; i < len; i++) {
        char b = in[i];
        char h = (b >> 4) & 0xf;
        char l = b & 0xf;

		char r1 = (h >= 10 ? 'a' + (h-10) : '0' + h);
        char r2 = (l >= 10 ? 'a' + (l-10) : '0' + l);
		char s1[1] = {r1};
		char s2[1] = {r2};

        printlen(s1, 1);
        printlen(s2, 1);
        printlen(" ", 1);
    }
}

/* Dump the bytes as hex in the specified range. */
void dump_byte_range(unsigned int start, unsigned int end) {
	if (start > end) {
		char* err_str = "Error, wrong range\n";
		printlen(err_str, strlen(err_str));
		return;
	}
	print_hex((char*) start, end-start);
}

void debugger_hook() {
	static int beenhere = 0;

	if (beenhere != 2){
		char* a = "YOHAY1";
		size_t y_len = strlen(a);
		
		char buffer[50];
		int d = 10, b = 20, c;
		c = d + b;
		sprintf(buffer, "Sum of %d and %d is %d", d, b, c);
		printlen(buffer, strlen(buffer));

		printcrlf();
		print_hex(a, 7);
		printcrlf();

		dump_byte_range(0x40669586, 0x4066959f);
		printcrlf();
		dump_byte_range(0x4066959f, 0x40669586);
		printcrlf();

		beenhere++;
	}
	return;
}

int task_main() {
	char * msg = "DEBUGGER CODE GOES HERE";
	int msg_len = 23;
	void (*printlen)(char *, int) = (void*) PRINTBUF;
	void (*printcrlf)(void) = (void*) PRINTCRLF;

	printcrlf();
	printlen("START", 5);
	printcrlf();

	printlen(msg, msg_len);
	printcrlf();
	debugger_hook();
	return 0;

}