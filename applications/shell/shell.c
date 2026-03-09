#include <tty.h>
#include <shell.h>

#define SHELL_BUFFER_SIZE 256
#define SHELL_WELCOME_MSG (                                                 \
"  _      __    __                     __             ____  ____\n"           \
" | | /| / /__ / /______  __ _  ___   / /____    ___ / __ \\/ __/\n"          \
" | |/ |/ / -_) / __/ _ \\/  ' \\/ -_) / __/ _ \\  (_-</ /_/ /\\ \\  \n"      \
" |__/|__/\\__/_/\\__/\\___/_/_/_/\\__/  \\__/\\___/ /___/\\____/___/  \n" )

char shell_buffer[SHELL_BUFFER_SIZE];

static inline void shell_print_welcome(void);
static inline void shell_execute_command(const char* command);

static inline void shell_print_welcome(void)
{
    tty_print_line(SHELL_WELCOME_MSG);
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
