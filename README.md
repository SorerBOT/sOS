# sOS


https://github.com/user-attachments/assets/af7378a8-ca2b-4ffc-9797-3c89804e7af6


sOS is a 64-bit kernel and bootloader I wrote myself from scratch.
## Features
* Bootloader - a custom Legacy BIOS bootloader.
* VGA driver - including scrolling functionality.
* `vsnprintf` - implemented the entire `vsnprintf` spec (except floats, but including all the flags). Tested using Marco Paland's testing suite which I refactored to use my own testing framework ([cunit.h](https://github.com/SorerBOT/cunit.h))
* Interrupts - 
    * IDT - completed.
    * ISRs - implemented ISRs for the most common exceptions, the keyboard IRQ, and my syscall mechanism.
    * PIC - re-programmed the PIC to remap IRQs.
    * IRQs - implemented IRQ 1 (i.e. the keyboard IRQ).
* Keyboard Driver - implemented a robust two layer keyboard driver; the first handles hardware specific details like PS/2 scancodes, and the second handles a stream of keyboard events offering the client the complete keyboard state, a raw stream of keyboard events and baked events I call `unit`s. The driver also supports inserting custom keyboard layouts.
* Syscalls - implemented a mechanism for sending and handling syscalls, and implemented a `process_launch` syscall.
* Context Switching - able to switch between two processes and swapping out their appropriate page tables.
* Memory management -
   * Memory Map - the bootloader fetches the computer's memory map (using the Legacy BIOS 0xE820 function).
   * PMM - fully functional and very efficient Physical Memory Manager, and Physical Memory Allocator (a stack + bitmap allocator, using 2MiB huge pages).
   * VMM - able to create and modify page tables, including a copy of the higher half of the kernel's page table and the kernel's source binary.
   * Kernel Allocator - an allocator which employs several slab-allocators to allocate different powers of two. This setup is pretty amazing in terms of speed, and reduces the memory waste introduced when keeping allocation headers before small allocations, but introduces quite some fragmentation when the allocations are big.
* Disk Driver - implemented a basic PIO based ata disk driver. ATM only the bare minimum (initialisation + reading a sector) is implemented, however I am currently working towards extending the driver.
## Building & Running
Make sure you have the dependencies installed, then clone and run:
```bash
./runOS.sh
```

### Dependencies
* `qemu`
* `x86_64-elf` cross compiler
* `nasm`
