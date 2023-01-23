void parse_command(char*);
char* get_command(void);

enum command_type {
    memory = 0,
    registers = 1
};