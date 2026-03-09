#include <string.h>
#include <tty.h>
#include <shell.h>
#include <types.h>
#include <infinite_loop.h>

#define SHELL_ARGV_MAX_SIZE 16
#define SHELL_BUFFER_SIZE 256
#define SHELL_WELCOME_MSG (                                                   \
"  _      __    __                     __             ____  ____\n"           \
" | | /| / /__ / /______  __ _  ___   / /____    ___ / __ \\/ __/\n"          \
" | |/ |/ / -_) / __/ _ \\/  ' \\/ -_) / __/ _ \\  (_-</ /_/ /\\ \\  \n"      \
" |__/|__/\\__/_/\\__/\\___/_/_/_/\\__/  \\__/\\___/ /___/\\____/___/  \n" )

char shell_buffer[SHELL_BUFFER_SIZE];

static inline void print_welcome(void);
static inline void command_execute_internal(int argc, char** argv);
static inline void command_execute(char* command);
static inline errors_t command_parse(char* command, int* argc, char** argv, size_t argv_size);

static inline void print_welcome(void)
{
    tty_print(SHELL_WELCOME_MSG);
    tty_print("\n");
}

static inline errors_t command_parse(char* command, int* argc, char** argv, size_t argv_size)
{
    *argc = 0;

    bool is_parameter_start = true;
    bool is_in_quotes = false;
    for (; *command != '\0'; ++command )
    {
        if ( *command == '\"' )
        {
            if ( is_in_quotes == true )
            {
                *command = '\0';
            }
            else
            {
                is_parameter_start = true;
            }

            is_in_quotes = !is_in_quotes;
        }

        else if ( *command == ' ' && !is_in_quotes )
        {
            is_parameter_start = true;
            *command = '\0';
        }

        else if ( is_parameter_start )
        {
            is_parameter_start = false;
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

static inline void command_execute_internal(int argc, char** argv)
{
    if ( strcmp( argv[0], "infinite_loop" ) == 0 )
    {
        infinite_loop_launch(argc, argv);
    }
}

static inline void command_execute(char* command)
{
    int argc = 0;
    char* argv[SHELL_ARGV_MAX_SIZE];

    errors_t error = command_parse(command, &argc, argv, SHELL_ARGV_MAX_SIZE);

    if ( error == ERRORS_NONE )
    {
        if ( argc == 0 )
        {
            return;
        }

        command_execute_internal(argc, argv);
    }
    else
    {
        tty_print("SHELL error: not enough space in argv to execute command\n");
    }
}

void shell_launch(void)
{
    print_welcome();

    for (;;)
    {
        tty_print("sOS> ");
        tty_read_line(shell_buffer, SHELL_BUFFER_SIZE);
        command_execute(shell_buffer);
    }
}
