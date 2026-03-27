# Physical Memory Allocator

This spec is going to contain the details behind sOS's Physical Memory Allocator. The allocator is going to be optimised for sOS's usecase and hardware. The usecase is optimising for speed, and the hardware is assumed to be abundant in RAM (at least 16GiB).  
Under these assumptions I decided to employ 2MiB frames so as to prevent excessive TLB misses and prioritise raw speed (at the cost of introducing internal fragmentation). This means that the system contains exactly 8 * 1024 frames (not all allocatable).  
To the PMA's end, it seems plausible to allocate an entire frame. Loosely calculating that leaves us with 256 bytes per frame, which we could use to store the frame's initial address (8 bytes) and its status (1 bytes boolean) SEVERAL times. This means, that a combined approach of using a linked list to quickly find the allocatable frame, and a bitmap to find the status of a frame is completely relevant and won't consume excessive space.
