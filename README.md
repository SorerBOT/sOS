WIP
# bootloader
This is a Legacy BIOS bootloader. The general goal is to slowly turn this into a self-hosting OS.


## Where are we ATM?

* 64-bit Long Mode.
* Functional textual VGA driver, including a custom printf function and scroll functionality.
* Full printf spec (except floats, but including all the flags). Tested using Marco Paland testing suite, adapted to use my own testing framework (cunit.h)

In greater detail, the bootloader already reaches 32-bit Long Mode, but it does not fully adhere to the MBR standard just yet. My aim is to rush-through stages, specifically 16-bit Real Mode (in which you can perform BIOS calls), and 32-bit Protected Mode, and only return to them once they would help me implement a new feature in Long Modes, this is because its my first bootloader/kernel and I feel like this is the most pedagogical approach, at least for me.  

## OS Concepts I've learned, and some WHAT THE HECK line explanations
* A20 Line - older CPUs sometimes treated addresses above the 1MB limit as if they wrapped around to 0. Application developers would rely on this "feature", and that meant that as soon as the 1MB limit was increased, all the programs which relied on wrapping simply broke. To fight this, it was decided to make the "wrap around" optional and toggled on by default. By enabling the A20 gate, we simply tell the CPU: don't wrap around!
* Fast A20 - it used to be quite complicated to enable the A20 gate, but nowadays there's a simple method called A20, which is almost as simple as flipping a bit, and this is the method I used.
* GDT - the GDT, or Global Descriptor Table is a table that stores information about different segments, such as where in memory does the segment start, where does it end, who is allowed to interact with it and what are they allowed to do.
* GDT Header - a small data structure which describes where in memory does the Global Descriptor Table reside, and how large it is.
* GDTR - a register designated to store the address of the GDT Header.
* Null Descriptor - the first entry of the GDT has to be the Null Descriptor which is practically just a quadword of 0s.
### Interrupts
any form of communication consists of either of the following schemas:
* polling based: the first party constantly asks the second party, have you got a message for me? Kinda like opening the door every 5 minutes to check if the guests have arrived.
* interrupt based: the first party minds it business until the second party, sends it a message. Kinda like a doorbell.
Many modern computer architectures favour the interrupt based approach, mostly because polling means performing a lot of wasteful actions (like checking the door 100 times, before the guests finally arrive).  
When the CPU gets interrupted, it first checks if its an interrupt it should handle, if so, it checks if a handler is found for that interrupt in a table called "IDT". If it is present, then the CPU would want to execute it, in order to have the interrupt handled, but beforehand, it needs a way to return back to what it was doing formerly and for that reason, it pushes vital state data onto the stack, such as the instruction pointer, the stack pointer, and more!  
Once the interrupt handler (aka. ISR) is being executed, its first job is to make sure it doesn't ruin the CPU's ability to go back to what it was formerly doing. For this reason, ISRs typically store most, if not all, general registers onto the stack (at least if they're handled in a high level language, where the programmer has no control over the registers in use). When all general register values are safely stashed, it is finally time to handle the interrupt. When the interrupt handler is finished, it has to restore the registers to the prior values (using the stashed registers), and call the IRET instruction which would have the computer go back to its previous state, using the data that it psuhed onto the stack, and that's it.

#### PIC
We have just described what interrupts ARE, but we have not spoken about who dispatches them. The "interrupt lifecycle" is the following: firstly, a device such as a keyboard decides it wants to capture the CPU's attention. It raises an electrical signal that is recognised by the keyboard device controller. The keyboard device controller, sends a certain voltage over to a component called "PIC", which translates this voltage into an actual interrupt number, and forwards it to the CPU, which handles it using the mechanisms discussed in the above.  
So basically, this PIC's role is to mediate between the CPU and the device controllers, transforming their electrical voltages into concrete interrupt numbers. The PIC has another important role, which is to decide in what order should the interrupts be handled.  
In its original form (8259 PIC), the PIC was actually comprised of two PICS, the master and slave PICs. The master PIC, was directly connected to the CPU, and the slave PIC was connected to the master PIC. Each had 8 channels connected to it called IRQs. Each IRQ is basically a wire which connects the PIC to a device controller and to the CPU. Because the slave PIC was not directly connected to the CPU, it would send its interrupts through one of the masters IRQs (IRQ no. 2). For this reason, we only have a total of 15 available IRQs (one is wasted).  
Modern PICs, such as the APIC start up in a mode which emulates the legacy 8259 PIC.

#### Device Controller
As we have said, an interrupt only works to grab the CPUs attention, so as to have it tend to some device. It does not carry any data related to what does the device want (except for the interrupt number). For example, if a keyboard key is pressed, the interrupt number does not specify which key was pressed, but simply tells the CPU that the keyboard wants its attention. For this reason, we would want to have some component, which deals with the internals of the keyboad: it would check exactly what key was pressed, and it would place it somewhere (output buffer), where the CPU could read it. That component, is the device controller.
