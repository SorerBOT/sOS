__asm__(".code32\n");

#define VGA_BUFFER 0xB8000
#define WHITE_ON_BLACK 0x05
#define PREVIOUS_STAGES_LINES_USED 12

void print_char(char c, int col, int row)
{
    volatile char* video = (volatile char*)VGA_BUFFER;
    int index = (row * 80 + col) * 2; // 2 bytes per char (char + color)
    video[index] = c;
    video[index + 1] = WHITE_ON_BLACK;
}

void print_string(const char* str, int row)
{
    int col = 0;
    while (*str) {
        print_char(*str, col, row);
        col++;
        str++;
    }
}

void bootloader_stage_3()
{
    print_string("[sOS]: stage 2 completed...", PREVIOUS_STAGES_LINES_USED);
    print_string("[sOS]: successfully entered protected 32-bit mode.", PREVIOUS_STAGES_LINES_USED + 1);
    print_string("[sOS]: running stage 2...", PREVIOUS_STAGES_LINES_USED + 2);


    // Hang forever
    while(1);
}
