/*
 * I currently use this shell not as a shell, but rather as a means to
 * achieve some degree of interactivity for my current teting
 * environment. I do not adhere to the tty/console_output separation
 * because the kernel is far from ready to host a real shell yet.
 */

#include <console_output.h>
#include <string.h>
#include <tty.h>
#include <shell.h>
#include <types.h>
#include <infinite_loop.h>
#include <kernel_allocator.h>
#include <ata_driver.h>

#include <pmm.h>
#include <vmm.h>

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
        infinite_loop_launch();
        return;
    }

    if ( strcmp( argv[0], "alloc_frame" ) == 0 )
    {
        dword* frame = VMM_TRANSLATE_PHYSICAL_TO_KERNEL_MAP(pmm_frame_alloc());
        if ( frame == NULL )
        {
            tty_print("Unexpected error: failed to allocate memory.\n");
            return;
        }

        console_output_printf("frame address: %p\n", frame);

        for ( size_t i = 0; i < (2 * 1024 * 1024 / sizeof(frame[0])); ++i )
        {
            frame[i] = 0xcafebabe;
        }

        pmm_frame_free(VMM_TRANSLATE_KERNEL_MAP_TO_PHYSICAL(frame));

        frame = VMM_TRANSLATE_PHYSICAL_TO_KERNEL_MAP(pmm_frame_alloc());
        if ( frame == NULL )
        {
            tty_print("Unexpected error: failed to allocate memory.\n");
            return;
        }

        for ( size_t i = 0; i < (2 * 1024 * 1024 / sizeof(frame[0])); ++i )
        {
            if ( frame[i] != 0xcafebabe )
            {
                tty_print("Unexpected error: restored frame does not contain 0xcafebabe.\n");
                return;
            }
        }
        pmm_frame_free(VMM_TRANSLATE_KERNEL_MAP_TO_PHYSICAL(frame));
        tty_print("Successfully passed frame allocator test.\n");
    }

    if ( strcmp( argv[0], "dump_sector" ) == 0 )
    {
        size_t lba_address = 0;


        byte* sector = kernel_allocator_allocate(ATA_DRIVER_SECTOR_SIZE_IN_BYTES);
        if ( sector == NULL )
        {
            console_output_print_blue_screen("Failed to allocate memory\n");
            while (1)
            {
                __asm__ volatile ("hlt");
            }
        }
        ata_driver_read_sector(0, sector, ATA_DRIVER_SECTOR_SIZE_IN_BYTES);
        word* sector_words = (word*) sector;
        for ( size_t i = 0; i <  ATA_DRIVER_SECTOR_SIZE_IN_WORDS; ++i )
        {
            console_output_printf("%hx ", sector_words[i]);
        }
        console_output_printf("\n");
    }

    if ( strcmp( argv[0], "overwrite_sector" ) == 0 )
    {
        size_t lba_address = 0;


        byte* sector = kernel_allocator_allocate(ATA_DRIVER_SECTOR_SIZE_IN_BYTES);
        if ( sector == NULL )
        {
            console_output_print_blue_screen("Failed to allocate memory\n");
            while (1)
            {
                __asm__ volatile ("hlt");
            }
        }

        word* sector_words = (word*) sector;
        for ( size_t i = 0; i <  ATA_DRIVER_SECTOR_SIZE_IN_WORDS; ++i )
        {
            sector_words[i] = 0x6942;
        }
        
        ata_driver_write_sector(0, sector, ATA_DRIVER_SECTOR_SIZE_IN_BYTES);
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
