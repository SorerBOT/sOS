# Linker Script
This isn't exactly a spec as much as it is an attempt to document how I derived the numbers involved in the linker script, and in the setup of the kernel's Virtual Memory.

The highest possible 64-bit unsigned integer: 0xFFFFFFFFFFFFFFFF
In x86_64 addresses are truncated to 48-bits, and then extended to with bits that repliace the 48th bit.
This means, that if the 48th bit is off, the highest address is: 0x00007FFFFFFFF and if the 48th bit is on, the minimum address is 0xFFFF800000000000 and as it would appear there's a massive gap between them.  
Now, we want the kernel to live in the last 2 GiBs of memory, the principle idea of the C memory model is that if all addresses are smaller than 2 GiBs, then we can store them in 32-bit "variable" and registers and that allows us to generate a significantly smaller binary which we want. For this reason we're looking to get the final 2 GiBs so subtracting 2 GiBs we get: 0x0000FFFF7FFFFFFF
Anyhow, we're looking for the address that is exactly 2 GiBs before the end of the higher address space (where the 48th bit is on). The maximum address there, is obviously: 0xFFFFFFFFFFFFFFFF. The Address exactly 2 GiBs from the end, is 0xFFFFFFFFFFFFFFFF - 2 * GiB + 1, where I add 1, because I'm trying to get the address from which we count 2 GiBs and reach the last address, including it, not the address just before it; it can be thought of like this; if I want to get the last 2 bits in 10 bits, I need to start at bit 9, not at bit 10 - 2 = 8.  
With that said, the address that is 2 GiBs from the end is: 0XFFFFFFFF80000000.



Then dividing by a PML4T entry size (in terms of how much memory it maps) which is 512GiB we get 511.996... meaning we're inside the 512th entry in the PML4T, which is index 511.
Then we use the remainder to find the PDPT entry. The remainder is: 0x7F7FFFFFFF
Now dividing by the size of a PDPT entry which is the size of an entire DPT table which is 512 * 2 MiB we get: 509.9999999990687
Meaning we're at the 510th entry, at index 509, and then to find the PDT entry we're looking for we get the remainder which is: 0x3fffffff
and we divide it by 2MiB to get: 511.99999952316284
which means we're in the 512th entry, at index 511.
I don't understand how come this last step has a remainder.
