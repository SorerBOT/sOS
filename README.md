# bootloader
This is a Legacy BIOS bootloader. The general goal is to slowly turn this into a self-hosting OS.


## Where are we ATM?

* 32-bit Protected Mode
* Functional textual VGA driver, including a custom printf function

In greater detail, the bootloader already reaches 32-bit Protected Mode, but it does not fully adhere to the MBR standard just yet. My aim is to rush-through stages, such as 16-bit Real Mode, and only return to them once I encounter issues in Protected / Long Modes, this is because its my first bootloader and I feel like this is the most pedagogical approach, at least for me.  

## OS Concepts I've learned, and some WHAT THE HECK line explanations
* A20 Line - older CPUs sometimes treated addresses above the 1MB limit as if they wrapped around to 0. Application developers would rely on this "feature", and that meant that as soon as the 1MB limit was increased, all the programs which relied on wrapping simply broke. To fight this, it was decided to make the "wrap around" optional and toggled on by default. By enabling the A20 gate, we simply tell the CPU: don't wrap around!
* Fast A20 - it used to be quite complicated to enable the A20 gate, but nowadays there's a simple method called A20, which is almost as simple as flipping a bit, and this is the method I used.
