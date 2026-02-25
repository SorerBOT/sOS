/* Im sorta using two namespaces, because KEYBOARD will soon be separated from ps2_keyboard.
 * keyboard will handle a general keypress, without the specifics of how to read the data when using ps2.
 * don't freak out.
 */

#include <ps2_keyboard.h>
#include <cpu_io.h>
#include <console_io.h>
#include <ring_buffer.h>

#define KEYBOARD_SCANCODE_2_EXTENDED_OFFSET 0xE0
#define KEYBOARD_BUFFER_SIZE 256


typedef enum
{
    KEYBOARD_INVALID,
    KEYBOARD_PRESSED,
    KEYBOARD_RELEASED
} keyboard_event_type_t;

typedef struct
{
    keyboard_event_type_t event_type;
    char key_ascii;
} keyboard_event_t;

static const keyboard_event_t* parse_scancode(byte scancode);



static ps2_keyboard_settings_t settings;
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static ring_buffer_t keyboard_ring_buffer;

static keyboard_event_t map_scancode_to_event[KEYBOARD_SCANCODE_2_EXTENDED_OFFSET];

static const keyboard_event_t* parse_scancode(byte scancode)
{
    return &map_scancode_to_event[scancode];
}

void ps2_keyboard_init(const ps2_keyboard_settings_t* _settings)
{
    settings = *_settings;

    keyboard_ring_buffer = (ring_buffer_t)
    {
        .buffer = (byte*) &keyboard_buffer[0],
        .head = 0,
        .tail = 0,
        .size = KEYBOARD_BUFFER_SIZE
    };


    // i don't want to get a huge binary because of this....
    map_scancode_to_event[0]    = (keyboard_event_t) { .event_type = KEYBOARD_INVALID };
    map_scancode_to_event[1]    = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 0x1b };
    map_scancode_to_event[2]    = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '1' };
    map_scancode_to_event[3]    = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '2' };
    map_scancode_to_event[4]    = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '3' };
    map_scancode_to_event[5]    = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '4' };
    map_scancode_to_event[6]    = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '5' };
    map_scancode_to_event[7]    = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '6' };
    map_scancode_to_event[8]    = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '7' };
    map_scancode_to_event[9]    = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '8' };
    map_scancode_to_event[10]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '9' };
    map_scancode_to_event[11]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '0' };
    map_scancode_to_event[12]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '-' };
    map_scancode_to_event[13]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '=' };
    map_scancode_to_event[14]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 0x08 };
    map_scancode_to_event[15]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '\t' };
    map_scancode_to_event[15]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'Q' };
    map_scancode_to_event[16]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'W' };
    map_scancode_to_event[17]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'E' };
    map_scancode_to_event[18]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'R' };
    map_scancode_to_event[19]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'T' };
    map_scancode_to_event[20]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'Y' };
    map_scancode_to_event[21]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'U' };
    map_scancode_to_event[22]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'I' };
    map_scancode_to_event[23]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'O' };
    map_scancode_to_event[24]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'P' };
    map_scancode_to_event[25]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '[' };
    map_scancode_to_event[26]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = ']' };
    map_scancode_to_event[27]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '\n' };
    map_scancode_to_event[28]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID };
    map_scancode_to_event[29]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'A' };
    map_scancode_to_event[30]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'S' };
    map_scancode_to_event[31]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'D' };
    map_scancode_to_event[32]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'F' };
    map_scancode_to_event[33]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'G' };
    map_scancode_to_event[34]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'H' };
    map_scancode_to_event[35]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'J' };
    map_scancode_to_event[36]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'K' };
    map_scancode_to_event[37]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'L' };
    map_scancode_to_event[38]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = ';' };
    map_scancode_to_event[39]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '\'' };
    map_scancode_to_event[40]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '`' };
    map_scancode_to_event[41]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID };
    map_scancode_to_event[42]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '\\' };
    map_scancode_to_event[43]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'Z' };
    map_scancode_to_event[44]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'X' };
    map_scancode_to_event[45]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'C' };
    map_scancode_to_event[46]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'V' };
    map_scancode_to_event[47]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'B' };
    map_scancode_to_event[48]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'N' };
    map_scancode_to_event[49]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = 'M' };
    map_scancode_to_event[50]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = ',' };
    map_scancode_to_event[51]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '.' };
    map_scancode_to_event[52]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '/' };
    map_scancode_to_event[53]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID };
    map_scancode_to_event[54]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '*' }; // keypad...
    map_scancode_to_event[55]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // left alt
    map_scancode_to_event[56]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = ' ' };
    map_scancode_to_event[57]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // capslock
    map_scancode_to_event[58]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F1
    map_scancode_to_event[59]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F2
    map_scancode_to_event[60]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F3
    map_scancode_to_event[61]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F4
    map_scancode_to_event[62]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F5
    map_scancode_to_event[63]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F6
    map_scancode_to_event[64]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F7
    map_scancode_to_event[65]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F8
    map_scancode_to_event[66]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F9
    map_scancode_to_event[67]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F10
    map_scancode_to_event[68]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // numberlock
    map_scancode_to_event[69]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // scrollock
    map_scancode_to_event[70]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '7' };
    map_scancode_to_event[71]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '8' };
    map_scancode_to_event[72]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '9' };
    map_scancode_to_event[73]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '-' };
    map_scancode_to_event[74]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '4' };
    map_scancode_to_event[75]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '5' };
    map_scancode_to_event[76]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '6' };
    map_scancode_to_event[77]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '+' };
    map_scancode_to_event[78]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '1' };
    map_scancode_to_event[79]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '2' };
    map_scancode_to_event[80]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '3' };
    map_scancode_to_event[81]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '0' };
    map_scancode_to_event[82]   = (keyboard_event_t) { .event_type = KEYBOARD_PRESSED, .key_ascii = '.' };
    map_scancode_to_event[83]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap
    map_scancode_to_event[84]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap
    map_scancode_to_event[85]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap
    map_scancode_to_event[85]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F11
    map_scancode_to_event[86]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F12
    map_scancode_to_event[87]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap
    map_scancode_to_event[88]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap
    map_scancode_to_event[89]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap
    map_scancode_to_event[90]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap
    map_scancode_to_event[91]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap
    map_scancode_to_event[92]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap
    map_scancode_to_event[93]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap
    map_scancode_to_event[94]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap
    map_scancode_to_event[95]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap
    map_scancode_to_event[96]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap
    map_scancode_to_event[97]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap
    map_scancode_to_event[98]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap
    map_scancode_to_event[99]   = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap
    map_scancode_to_event[100]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[101]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[102]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[103]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[104]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[105]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[106]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[107]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[108]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[109]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[110]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[111]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[112]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[113]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[114]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[115]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[116]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[117]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[118]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[119]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[120]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[121]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[122]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[123]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[124]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[125]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[126]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[127]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap;
    map_scancode_to_event[128]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID };
    map_scancode_to_event[129]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 0x1b };
    map_scancode_to_event[130]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '1' };
    map_scancode_to_event[131]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '2' };
    map_scancode_to_event[132]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '3' };
    map_scancode_to_event[133]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '4' };
    map_scancode_to_event[134]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '5' };
    map_scancode_to_event[135]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '6' };
    map_scancode_to_event[136]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '7' };
    map_scancode_to_event[137]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '8' };
    map_scancode_to_event[138]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '9' };
    map_scancode_to_event[139]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '0' };
    map_scancode_to_event[140]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '-' };
    map_scancode_to_event[141]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '=' };
    map_scancode_to_event[142]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 0x08 };
    map_scancode_to_event[143]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '\t' };
    map_scancode_to_event[144]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'Q' };
    map_scancode_to_event[145]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'W' };
    map_scancode_to_event[146]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'E' };
    map_scancode_to_event[147]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'R' };
    map_scancode_to_event[148]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'T' };
    map_scancode_to_event[149]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'Y' };
    map_scancode_to_event[150]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'U' };
    map_scancode_to_event[151]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'I' };
    map_scancode_to_event[152]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'O' };
    map_scancode_to_event[153]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'P' };
    map_scancode_to_event[154]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '[' };
    map_scancode_to_event[155]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = ']' };
    map_scancode_to_event[156]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '\n' };
    map_scancode_to_event[157]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID };
    map_scancode_to_event[158]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'A' };
    map_scancode_to_event[159]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'S' };
    map_scancode_to_event[160]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'D' };
    map_scancode_to_event[161]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'F' };
    map_scancode_to_event[162]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'G' };
    map_scancode_to_event[163]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'H' };
    map_scancode_to_event[164]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'J' };
    map_scancode_to_event[165]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'K' };
    map_scancode_to_event[166]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'L' };
    map_scancode_to_event[167]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = ';' };
    map_scancode_to_event[168]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '\'' };
    map_scancode_to_event[169]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '`' };
    map_scancode_to_event[170]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID };
    map_scancode_to_event[171]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '\\' };
    map_scancode_to_event[172]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'Z' };
    map_scancode_to_event[173]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'X' };
    map_scancode_to_event[174]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'C' };
    map_scancode_to_event[175]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'V' };
    map_scancode_to_event[176]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'B' };
    map_scancode_to_event[177]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'N' };
    map_scancode_to_event[178]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = 'M' };
    map_scancode_to_event[179]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = ',' };
    map_scancode_to_event[180]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '.' };
    map_scancode_to_event[181]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '/' };
    map_scancode_to_event[182]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID };
    map_scancode_to_event[183]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '*' }; // keypad...
    map_scancode_to_event[184]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // left alt
    map_scancode_to_event[185]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = ' ' };
    map_scancode_to_event[186]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // capslock
    map_scancode_to_event[187]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F1
    map_scancode_to_event[188]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F2
    map_scancode_to_event[189]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F3
    map_scancode_to_event[190]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F4
    map_scancode_to_event[191]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F5
    map_scancode_to_event[192]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F6
    map_scancode_to_event[193]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F7
    map_scancode_to_event[194]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F8
    map_scancode_to_event[195]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F9
    map_scancode_to_event[196]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F10
    map_scancode_to_event[197]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // numberlock
    map_scancode_to_event[198]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // scrollock
    map_scancode_to_event[199]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '7' };
    map_scancode_to_event[200]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '8' };
    map_scancode_to_event[201]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '9' };
    map_scancode_to_event[202]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '-' };
    map_scancode_to_event[203]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '4' };
    map_scancode_to_event[204]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '5' };
    map_scancode_to_event[205]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '6' };
    map_scancode_to_event[206]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '+' };
    map_scancode_to_event[207]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '1' };
    map_scancode_to_event[208]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '2' };
    map_scancode_to_event[209]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '3' };
    map_scancode_to_event[210]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '0' };
    map_scancode_to_event[211]  = (keyboard_event_t) { .event_type = KEYBOARD_RELEASED, .key_ascii = '.' };
    map_scancode_to_event[212]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap
    map_scancode_to_event[213]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap
    map_scancode_to_event[214]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // just a gap
    map_scancode_to_event[215]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F11
    map_scancode_to_event[216]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID }; // F12
    map_scancode_to_event[217]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID };
    map_scancode_to_event[218]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID };
    map_scancode_to_event[219]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID };
    map_scancode_to_event[220]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID };
    map_scancode_to_event[221]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID };
    map_scancode_to_event[222]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID };
    map_scancode_to_event[223]  = (keyboard_event_t) { .event_type = KEYBOARD_INVALID };
}

void ps2_keyboard_read_and_handle_scancode(void)
{
    byte scancode = cpu_io_read_byte(settings.data_port);
    const keyboard_event_t* event = parse_scancode(scancode);

    if ( event->event_type == KEYBOARD_RELEASED )
    {
        ring_buffer_force_write(&keyboard_ring_buffer, (byte*)&event->key_ascii, sizeof(event->key_ascii));
    }
}

void ps2_keyboard_read_char_from_ring_buffer(char* c)
{
    while ( ring_buffer_read(&keyboard_ring_buffer, (byte*)c, sizeof(*c)) == 0 )
    {
        __asm__ volatile("hlt");
    }
}
