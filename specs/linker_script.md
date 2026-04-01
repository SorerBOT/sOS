# Linker Script
This isn't exactly a spec as much as it is an attempt to document how I derived the numbers involved in the linker script, and in the setup of the kernel's Virtual Memory.

The highest possible 64-bit unsigned integer: 0xFFFFFFFFFFFFFFFF
In x86_64 addresses are truncated to 48-bits, and then extended to with bits that repliace the 48th bit.
This means, that if the 48th bit is off, the highest address is: 0x00007FFFFFFFF and if the 48th bit is on, the minimum address is 0xFFFF800000000000 and as it would appear there's a massive gap between them. The main takeaway from this discussion is that the highest address is 0x0000FFFFFFFFFFFF and not the one where all the bits are toggled on.  
Anyhow, we want the kernel to live in the last 2 GiBs of memory, the principle idea of the C memory model is that if all addresses are smaller than 2 GiBs, then we can store them in 32-bit "variables" and registers which in turn allows us to generate a significantly smaller binaries which is desirable.  
Now, our goal is to find the address that is exactly 2 GiBs from the end of the higher address space (where the 48th bit is on). The maximum address there, we have concluded is: 0x0000FFFFFFFFFFFF. The Address exactly 2 GiBs from the end, is 0xFFFFFFFFFFFFFFFF - 2 * GiB + 1, where I add 1, because I'm trying to get the address from which we count 2 GiBs and reach the last address, including it, not the address just before it; it can be thought of like this; if I want to get the last 2 bits in 10 bits, I need to start at bit 9, not at bit 10 - 2 = 8.  
With that said, the address that is 2 GiBs from the end is: 0XFFFFFFFF80000000.

Our last goal, is to find the the exact page in the page table which translates this virtual memory address; to do so, we divide first by a PML4T entry size (in terms of how much memory it maps) which is 512GiB we get 511.REMAINDER meaning we're inside the 512th entry in the PML4T, which is index 511.
Then we use the remainder to find the PDPT entry. We apply the same logic to find the DPT entry as well; I went and created the following stub to perform and validate the calculations for me:
```c
#include <stdio.h>
#include <stdint.h>

int main(void)
{
    uint64_t max_address = 0x0000FFFFFFFFFFFF;
    uint64_t GiB = 1024 * 1024 * 1024;
    uint64_t address = max_address - 2 * GiB + 1;
    uint64_t address_temp = address;


    uint64_t pdt_entry_memory_size = 2 * 1024 * 1024;
    uint64_t pdpt_entry_memory_size = 512 * pdt_entry_memory_size;
    uint64_t pml4t_entry_memory_size = 512 * pdpt_entry_memory_size;


    uint64_t pml4t_entry_idx = address_temp / pml4t_entry_memory_size;
    address_temp -= pml4t_entry_idx * pml4t_entry_memory_size;
    uint64_t pdpt_entry_idx = address_temp / pdpt_entry_memory_size;
    address_temp -= pdpt_entry_idx * pdpt_entry_memory_size;
    uint64_t pdt_entry_idx = address_temp / pdt_entry_memory_size;
    address_temp -= pdt_entry_idx * pdt_entry_memory_size;
    
    printf("Address 2 GiB from the end: %llx\nPML4T entry idx: %llu\nPDPT entry idx: %llu\nPDT entry idx: %llu\n", address, pml4t_entry_idx, pdpt_entry_idx, pdt_entry_idx);

    return 0;
}
```

The results were:
```
Address 2 GiB from the end: ffff80000000
PML4T entry idx: 511
PDPT entry idx: 510
PDT entry idx: 0
```
