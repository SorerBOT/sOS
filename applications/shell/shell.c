#include <tty.h>
#include <shell.h>
#include <types.h>
#include <console_output.h>

#define SHELL_ARGV_MAX_SIZE 16
#define SHELL_BUFFER_SIZE 256
#define SHELL_WELCOME_MSG (                                                   \
"  _      __    __                     __             ____  ____\n"           \
" | | /| / /__ / /______  __ _  ___   / /____    ___ / __ \\/ __/\n"          \
" | |/ |/ / -_) / __/ _ \\/  ' \\/ -_) / __/ _ \\  (_-</ /_/ /\\ \\  \n"      \
" |__/|__/\\__/_/\\__/\\___/_/_/_/\\__/  \\__/\\___/ /___/\\____/___/  \n" )

char shell_buffer[SHELL_BUFFER_SIZE];

static inline void print_welcome(void);
static inline void command_execute(char* command);
static inline errors_t command_parse(char* command, int* argc, const char** argv, size_t argv_size);

static inline void print_welcome(void)
{
    tty_print_line(SHELL_WELCOME_MSG);
}

static inline errors_t command_parse(char* command, int* argc, const char** argv, size_t argv_size)
{
    *argc = 0;

    bool is_right_after_space = true;
    for (; *command != '\0'; ++command )
    {
        if ( *command == ' ' )
        {
            is_right_after_space = true;
            *command = '\0';
        }
        else if ( is_right_after_space )
        {
            is_right_after_space = false;
            if ( *argc < argv_size )
            {
                argv[(*argc)++] = command;
            }
            else
            {
                return ERRORS_WRITE_NOT_ENOUGH_SPACE;
            }
        }
    }

    return ERRORS_NONE;
}

static inline void command_execute(char* command)
{
    int argc = 0;
    const char* argv[SHELL_ARGV_MAX_SIZE];

    errors_t error = command_parse(command, &argc, argv, SHELL_ARGV_MAX_SIZE);

    if ( error == ERRORS_NONE )
    {
        console_output_printf("Parsed command. Found: %d parameters, which are:\n", argc);
        for ( size_t i = 0; i < argc; ++i )
        {
            console_output_printf("\t%lu) %s:\n", i, argv[i]);
        }
    }
    else
    {
        tty_print_line("SHELL error: not enough space in argv to execute command");
    }
}

void shell_launch(void)
{
    print_welcome();

    for (;;)
    {
        tty_read_line(shell_buffer, SHELL_BUFFER_SIZE);
        command_execute(shell_buffer);
    }
}
