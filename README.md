WIP
# sOS
This is a small single-user kernel which boots using a Legacy BIOS bootloader I wrote myself. The general goal is to slowly turn this into a self-hosting OS which is optimised and tailored to my usecase: a single user machine, with only a few applications running at a time and much more RAM than required by those applications.

## Features
* Bootloader which boots into 64-bit Long Mode.
* VGA driver, including a custom printf function and scroll functionality.
* Full printf spec (except floats, but including all the flags). Tested using Marco Paland testing suite which I adapted to use my own testing framework (cunit.h)
* Interrupts - 
    * IDT - written
    * ISRs for exceptions - written (at least for the most common exceptions)
    * PIC - re-programmed the PIC
    * IRQs handled: as of now only IRQ 1 is handled (keyboard IRQ)
* Keyboard Driver - robust keyboard driver with two layers; one to handle hardware specific details like PS/2 scancodes, and another to handle a stream of keyboard events offering the client to receive the complete keyboard state, raw keyboard events and baked ones, with support for inserting custom keyboard layouts.
* A syscall interface; implemented a process_launch syscall.
* Basic Context Switching - able to switch between two processes, including swapping out the page tables.
* Memory management:
   * The bootloader fetches the computer's memory map (using Legacy BIOS functions).
   * PMM - fully functional and very efficient Physical Memory Manager, including of course a Physical Memory Allocator (a stack + bitmap allocator, using 2MiB huge pages).
   * VMM - able to create and modify page tables, including a copy of the higher half of the kernel's page table and the kernel's binary.
   * Kernel Allocator - employing several slab-allocators to allocate different powers of two. This setup is pretty amazing in terms of speed, and reduces the memory waste introduced when keeping allocation headers before small allocations, but introduces quite some fragmentation when the datastructures are big.
