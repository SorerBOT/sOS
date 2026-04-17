#include <ata_driver.h>
#include <console_output.h>
#include <cpu_io.h>

#define ATA_DRIVER_PRIMARY_IO_PORT_BASE 0x1F0
#define ATA_DRIVER_PRIMARY_CONTROL_PORT_BASE 0x3F6

#define ATA_DRIVER_SECONDARY_IO_PORT_BASE 0x170
#define ATA_DRIVER_SECONDARY_CONTROL_PORT_BASE 0x376

#define ATA_DRIVER_PRIMARY_IO_PORT_DATA                      ((ATA_DRIVER_PRIMARY_IO_PORT_BASE))
#define ATA_DRIVER_PRIMARY_IO_PORT_ERROR                     ((ATA_DRIVER_PRIMARY_IO_PORT_BASE) + 1)
#define ATA_DRIVER_PRIMARY_IO_PORT_FEATURES                  ((ATA_DRIVER_PRIMARY_IO_PORT_BASE) + 1)
#define ATA_DRIVER_PRIMARY_IO_PORT_SECTOR_COUNT              ((ATA_DRIVER_PRIMARY_IO_PORT_BASE) + 2)
#define ATA_DRIVER_PRIMARY_IO_PORT_LBA_LOW                   ((ATA_DRIVER_PRIMARY_IO_PORT_BASE) + 3)
#define ATA_DRIVER_PRIMARY_IO_PORT_LBA_MID                   ((ATA_DRIVER_PRIMARY_IO_PORT_BASE) + 4)
#define ATA_DRIVER_PRIMARY_IO_PORT_LBA_HIGH                  ((ATA_DRIVER_PRIMARY_IO_PORT_BASE) + 5)
#define ATA_DRIVER_PRIMARY_IO_PORT_DEVICE                    ((ATA_DRIVER_PRIMARY_IO_PORT_BASE) + 6)
#define ATA_DRIVER_PRIMARY_IO_PORT_STATUS                    ((ATA_DRIVER_PRIMARY_IO_PORT_BASE) + 7)
#define ATA_DRIVER_PRIMARY_IO_PORT_COMMAND                   ((ATA_DRIVER_PRIMARY_IO_PORT_BASE) + 7)

#define ATA_DRIVER_PRIMARY_CONTROL_PORT_ALTERNATE_STATUS     ((ATA_DRIVER_PRIMARY_CONTROL_PORT_BASE) + 0)
#define ATA_DRIVER_PRIMARY_CONTROL_PORT_DEVICE_CONTROL       ((ATA_DRIVER_PRIMARY_CONTROL_PORT_BASE) + 0)
#define ATA_DRIVER_PRIMARY_CONTROL_PORT_DRIVE_ADDRESS        ((ATA_DRIVER_PRIMARY_CONTROL_PORT_BASE) + 1)

#define ATA_DRIVER_SECONDARY_IO_PORT_DATA                    ((ATA_DRIVER_SECONDARY_IO_PORT_BASE))
#define ATA_DRIVER_SECONDARY_IO_PORT_ERROR                   ((ATA_DRIVER_SECONDARY_IO_PORT_BASE) + 1)
#define ATA_DRIVER_SECONDARY_IO_PORT_FEATURES                ((ATA_DRIVER_SECONDARY_IO_PORT_BASE) + 1)
#define ATA_DRIVER_SECONDARY_IO_PORT_SECTOR_COUNT            ((ATA_DRIVER_SECONDARY_IO_PORT_BASE) + 2)
#define ATA_DRIVER_SECONDARY_IO_PORT_LBA_LOW                 ((ATA_DRIVER_SECONDARY_IO_PORT_BASE) + 3)
#define ATA_DRIVER_SECONDARY_IO_PORT_LBA_MID                 ((ATA_DRIVER_SECONDARY_IO_PORT_BASE) + 4)
#define ATA_DRIVER_SECONDARY_IO_PORT_LBA_HIGH                ((ATA_DRIVER_SECONDARY_IO_PORT_BASE) + 5)
#define ATA_DRIVER_SECONDARY_IO_PORT_DEVICE                  ((ATA_DRIVER_SECONDARY_IO_PORT_BASE) + 6)
#define ATA_DRIVER_SECONDARY_IO_PORT_STATUS                  ((ATA_DRIVER_SECONDARY_IO_PORT_BASE) + 7)
#define ATA_DRIVER_SECONDARY_IO_PORT_COMMAND                 ((ATA_DRIVER_SECONDARY_IO_PORT_BASE) + 7)

#define ATA_DRIVER_SECONDARY_CONTROL_PORT_ALTERNATE_STATUS   ((ATA_DRIVER_SECONDARY_CONTROL_PORT_BASE) + 0)
#define ATA_DRIVER_SECONDARY_CONTROL_PORT_DEVICE_CONTROL     ((ATA_DRIVER_SECONDARY_CONTROL_PORT_BASE) + 0)
#define ATA_DRIVER_SECONDARY_CONTROL_PORT_DRIVE_ADDRESS      ((ATA_DRIVER_SECONDARY_CONTROL_PORT_BASE) + 1)

#define ATA_DRIVER_REGISTER_DEVICE_BASE 0b10100000
#define ATA_DRIVER_REGISTER_DEVICE_MASTER (ATA_DRIVER_REGISTER_DEVICE_BASE)
#define ATA_DRIVER_REGISTER_DEVICE_SLAVE (ATA_DRIVER_REGISTER_DEVICE_BASE | (1 << 4))

#define ATA_DRIVER_REGISTER_DEVICE_CONTROL_NIEN (1 << 1)

#define ATA_DRIVER_COMMAND_IDENTIFY 0xEC

#define ATA_DRIVER_SECTOR_SIZE_IN_WORDS 256

typedef enum
{
    ATA_DRIVER_ERROR_AMNF   = 1 << 0,
    ATA_DRIVER_ERROR_TKZNF  = 1 << 1,
    ATA_DRIVER_ERROR_ABRT   = 1 << 2,
    ATA_DRIVER_ERROR_MCR    = 1 << 3,
    ATA_DRIVER_ERROR_IDNF   = 1 << 4,
    ATA_DRIVER_ERROR_MC     = 1 << 5,
    ATA_DRIVER_ERROR_UNC    = 1 << 6,
    ATA_DRIVER_ERROR_BBK    = 1 << 7
} ata_driver_error_t;

typedef enum
{
    ATA_DRIVER_STATUS_ERR   = 1 << 0,
    ATA_DRIVER_STATUS_IDX   = 1 << 1,
    ATA_DRIVER_STATUS_CORR  = 1 << 2,
    ATA_DRIVER_STATUS_DRQ   = 1 << 3,
    ATA_DRIVER_STATUS_SRV   = 1 << 4,
    ATA_DRIVER_STATUS_DF    = 1 << 5,
    ATA_DRIVER_STATUS_RDY   = 1 << 6,
    ATA_DRIVER_STATUS_BSY   = 1 << 7
} ata_driver_status_t;

typedef enum
{
    ATA_DRIVER_DEVICE_CONTROL_ACTION_STOP_INTERRUPTS    = 1 << 1,
    ATA_DRIVER_DEVICE_CONTROL_ACTION_SOFTWARE_RESET     = 1 << 2,
    ATA_DRIVER_DEVICE_CONTROL_ACTION_HIGH_ORDER_BYTE    = 1 << 7,
} ata_driver_device_control_action_t;

#define ATA_DRIVER_DRIVE_ADDRESS_SELECTED_DRIVE_0_MASK  (1 << 0)
#define ATA_DRIVER_DRIVE_ADDRESS_SELECTED_DRIVE_1_MASK  (1 << 1)
#define ATA_DRIVER_DRIVE_ADDRESS_SELECTED_HEAD_MASK     (0xF << 2)
#define ATA_DRIVER_DRIVE_ADDRESS_WRITE_GATE_MASK        (1 << 6)

static inline void HI1_check_status_phase(void);
static inline void HPIOI1_check_status_phase(void);
static inline void select_device_phase(word io_port_device, byte device);
static inline void write_command_phase(word io_port_command, byte command);
static inline ata_driver_status_t wait_400_ns_and_read_alternate_status(void);
static inline void write_command_phase(word io_port_command, byte command);
static inline void disable_interrupts(word control_port_device);
static inline void identify_drive(void);

static word data_buffer[ATA_DRIVER_SECTOR_SIZE_IN_WORDS];

/*
 * ATA/ATAPI-6 SPEC REFERENCE; section 9.3: Bus Idle Protocol, HI1: Check_Status State
 */
static inline void HI1_check_status_phase(void)
{
    for ( ;; )
    {
        ata_driver_status_t status = cpu_io_read_byte(ATA_DRIVER_PRIMARY_IO_PORT_STATUS);

        if ( status & ATA_DRIVER_STATUS_BSY || status & ATA_DRIVER_STATUS_DRQ )
        {
            continue;
        }

        return;
    }
}

/*
 * ATA/ATAPI-6 SPEC REFERENCE; section 9.5: PIO data-in command protocol, HPIOI1: Check_Status State
 */
static inline void HPIOI1_check_status_phase(void)
{
    ata_driver_status_t status = wait_400_ns_and_read_alternate_status();

    if ( (status & ATA_DRIVER_STATUS_BSY) == 0 && status & ATA_DRIVER_STATUS_DRQ )
    {
        return;
    }

    while ( status & ATA_DRIVER_STATUS_BSY )
    {
        status = cpu_io_read_byte(ATA_DRIVER_PRIMARY_IO_PORT_STATUS);
    }

    /*
     * ATA/ATAPI-6 SPEC REFERENCE; section 9.5 PIO data-in command protocol, HPIOI1:HI0
     * an error has occurred, I'll panic for now.
     */
    if ( status & ATA_DRIVER_STATUS_DRQ )
    {
        console_output_print_blue_screen("Disk Error: expected data after data-request and received none.\n");
    }
}

/*
 * ATA/ATAPI-6 SPEC REFERENCE; section 9.3: Bus Idle Protocol, HI1: Device_Select State
 */
static inline void select_device_phase(word io_port_device, byte device)
{
    cpu_io_write_byte(io_port_device, device);
}

/*
 * ATA/ATAPI-6 SPEC REFERENCE; section 9.3: Bus Idle Protocol, HI4: Write_Command State
 */
static inline void write_command_phase(word io_port_command, byte command)
{
    cpu_io_write_byte(io_port_command, command);
}

/*
 * ATA/ATAPI-6 SPEC REFERENCE; section 7.8: Device Control Register, nIEN bit
 */
static inline void disable_interrupts(word control_port_device)
{
    cpu_io_write_byte(control_port_device, ATA_DRIVER_REGISTER_DEVICE_CONTROL_NIEN);
}

/*
 * OSDev trick for the 400ns delays mandated by the ATA/ATAPI-6 spec.
 * Based on ATA/ATAPI-6 SPEC; section 7.3.4: Alternate Status Register, Effect
 * it is better to use the alternate_status register because it does not clear
 * interrupts (This driver won't use interrupts but a future one might
 * and I rather prepare for that).
 */
static inline ata_driver_status_t wait_400_ns_and_read_alternate_status(void)
{
    for ( size_t i = 0; i < 14; ++i )
    {
        cpu_io_read_byte(ATA_DRIVER_PRIMARY_CONTROL_PORT_ALTERNATE_STATUS);
    }

    return cpu_io_read_byte(ATA_DRIVER_PRIMARY_CONTROL_PORT_ALTERNATE_STATUS);
}

static inline void identify_drive(void)
{
    HI1_check_status_phase();
    select_device_phase(ATA_DRIVER_PRIMARY_IO_PORT_DEVICE, ATA_DRIVER_REGISTER_DEVICE_MASTER);
    HI1_check_status_phase();

    /*
     * ATA/ATAPI-6 SPEC REFERENCE; section 9.3: Bus Idle Protocol, HI3: Write_Parameters State
     */
    cpu_io_write_byte(ATA_DRIVER_PRIMARY_IO_PORT_FEATURES, 0);
    cpu_io_write_byte(ATA_DRIVER_PRIMARY_IO_PORT_SECTOR_COUNT, 0);
    cpu_io_write_byte(ATA_DRIVER_PRIMARY_IO_PORT_LBA_LOW, 0);
    cpu_io_write_byte(ATA_DRIVER_PRIMARY_IO_PORT_LBA_MID, 0);
    cpu_io_write_byte(ATA_DRIVER_PRIMARY_IO_PORT_LBA_HIGH, 0);

    write_command_phase(ATA_DRIVER_PRIMARY_IO_PORT_COMMAND, ATA_DRIVER_COMMAND_IDENTIFY);

    /*
     * ATA/ATAPI-6 SPEC REFERENCE; section 9.5: PIO data-in command protocol, HPIOI1: Check_Status state
     */
    HPIOI1_check_status_phase();
    

    /*
     * ATA/ATAPI-6 SPEC REFERENCE; section 9.5: PIO data-in command protocol, HPIOI2: Transfer_Data state
     * reading the data
     */
    for ( size_t i = 0; i < 256; ++i )
    {
        data_buffer[i] = cpu_io_read_word(ATA_DRIVER_PRIMARY_IO_PORT_DATA);
    }
    
    /*
     * ATA/ATAPI-6 SPEC REFERENCE; section 9.5: PIO data-in command protocol, HPIOI2: Transfer_Data state
     * now moving into the HI0: Host_Idle state
     */
}

void ata_driver_setup(void)
{
    disable_interrupts(ATA_DRIVER_PRIMARY_CONTROL_PORT_DEVICE_CONTROL);
    identify_drive();


    /*
     * ATA/ATAPI-6 SPEC REFERENCE; section 8.16: IDENTIFY DEVICE, Table 26
     * I could not really make sensible #defines out of those. I'd just have
     * to refer to the aforementioned table 26.
     */
    word max_read_write_multiple = data_buffer[47] & 0xFF;
    word capabilities = data_buffer[49];
    word command_set_supported = data_buffer[83];
    word command_set_enabled = data_buffer[86];
    qword max_48_bit_user_lba_address = ( (qword)data_buffer[103] << 48 ) | ( (qword)data_buffer[102] << 32  ) | ( (qword)data_buffer[101] << 16 ) | ((qword)data_buffer[100]);
    word current_read_write_multiple = data_buffer[59];




    bool is_iordy_supported = capabilities & (1 << 11);
    bool is_lba_supported   = capabilities & (1 << 9);
    bool is_dma_supported   = capabilities & (1 << 8);

    if ( is_iordy_supported == false )
    {
        console_output_print_blue_screen("Disk Error: IORDY is not supported.\n");
    }

    if ( is_lba_supported == false )
    {
        console_output_print_blue_screen("Disk Error: LBA is not supported\n");
    }

    console_output_report("Disk: DMA is not supported.\n", is_dma_supported ? CONSOLE_OUTPUT_SUCCESS : CONSOLE_OUTPUT_FAILURE);



    bool is_read_write_multiple_enabled = current_read_write_multiple & (1 << 8);
    if ( is_read_write_multiple_enabled )
    {
        console_output_print_blue_screen("Disk Error: ata_driver does not support reading and writing in multiples yet.\n");
    }

}
