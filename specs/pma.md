# Physical Memory Allocator

This spec is going to contain the details behind sOS's Physical Memory Allocator. The allocator is going to be optimised for sOS's usecase and hardware. The usecase is optimising for speed, and the hardware is assumed to be abundant in RAM (at least 16GiB).  
Under these assumptions I decided to use 2MiB pages so as to prevent excessive TLB misses and prioritise raw speed (at the cost of introducing internal fragmentation). This means that the system contains exactly 8 * 1024 pages (not all allocatable).
