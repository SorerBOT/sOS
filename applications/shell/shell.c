#include <tty.h>
#include <shell.h>

#define SHELL_BUFFER_SIZE 256

char shell_buffer[SHELL_BUFFER_SIZE];

static inline void shell_print_welcome(void);
static inline void shell_execute_command(const char* command);

static inline void shell_print_welcome(void)
{
    tty_print_line("Welcome to sOS shell!");
}

static inline void shell_command_execute(const char* command)
{
    tty_print_line("Finished executing command!");
}

void shell_launch(void)
{
    shell_print_welcome();

    for (;;)
    {
        tty_read_line(shell_buffer, SHELL_BUFFER_SIZE);
        shell_command_execute(shell_buffer);
    }
}
