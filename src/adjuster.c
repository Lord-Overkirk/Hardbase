#define PRINTBUF 0x40e895e9
#define PRINTCRLF 0x40e88a49


const char TASK_NAME[] = "DOOM\0";


void our_hook(){
	static int beenhere = 0;

	if (beenhere != 2){
	void (*printlen)(char *, int) = PRINTBUF;
	void (*printcrlf)(void) = PRINTCRLF;
	printlen("YOHAY",6);
	printcrlf();
	beenhere++;
	}
	return 0;
}

int task_main(void){
	char * msg = "DOOM CODE GOES HERE";
	void (*printlen)(char *, int) = PRINTBUF;
	void (*printcrlf)(void) = PRINTCRLF;
	//char * SHELLCODE = "\x70\x47";
	//char * SHELLCODE = "\x51\x23\xc4\xf2\xc0\x73\x18\x47";
	//char * SHELLCODE = "\x4f\xf2\x85\x73\xc4\xf2\xf0\x03\x18\x47"; // jmp to bx lr
	//char * SHELLCODE = "\x00\x23\xc4\xf2\xc0\x73\x4b\xf6\xef\x62\xcd\xf6\xad\x62\xc3\xf8\x00\x28\x70\x47"; //write smthg
	//char * SHELLCODE = "\x00\xbf\x00\xbf\x00\xbf\x00\xbf\x70\x47"; //nop ret
	//char * SHELLCODE = "\xdf\xf8\x00\x30\x18\x47\x51\x00\xc0\x47"; //0x47 C0 00 00+0x50
	char * SHELLCODE = "\xdf\xf8\x00\x30\x18\x47\xde\xad\xbe\xef"; //0x47 C0 00 00+0x50
	//char * SHELLCODE = "\x51\x23\xc4\xf2\xc0\x73\x18\x47";


	printcrlf();
	printlen("START", 5);
	printcrlf();

	// try write to address
	volatile char * target = (char * )0x4060b1ee;
	//*target = SHELLCODE[1];
	//*(target+1) = SHELLCODE[0];
	//*target = 0x71;
	//*(target+1) = 0x47;
	//for(unsigned int i = 0; i< sizeof(SHELLCODE); i++) *(target+i) = SHELLCODE[i];

	//memcpy(target, SHELLCODE, sizeof(SHELLCODE));
	//memcpy(target, &our_hook, 100);

	//asm volatile ("DSB SY");
	//asm volatile ("ISB");
	//volatile char *mpu_addr = (char *) 0x41617a25;

	// ldr rx, [pc] // 2 insns ahaed
	// b rx
	// 0x where our code lives





/*
	if (*mpu_addr == 0x06) printlen("SUCCESS", 8);
	else if (*mpu_addr == 0x03) printlen("Fail", 8);
	else  printlen("STUPID", 8);
*/


	printlen(msg, 19);
	printcrlf();
	our_hook();
	//while(1);
	return 0;

}















