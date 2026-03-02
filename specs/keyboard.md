# Keyboard Spec
This spec details the requirements of any keyboard driver implemented in the project, be it a ps2 driver, a USB driver, or whatever.  

### One Instance Driver
The simplest possible keyboard driver listens to keyboard inputs, interprets their meaning (i.e. transforms a key-id into a concrete character), and keeps track of those events, so that it can serve them to the OS on demand.

### Abstracting the Layout Away
To my initial comprehension, the keyboard driver's role was to, on demand, return the character corresponding to the key currently pressed. Only later did it strike me that a keyboard driver has no way whatsoever of establishing that relation, as it is completely oblivious to the type of keyboard its operator is using; be it an English keyboard equipped with the QWERTY layout, or a Hebrew keyboard equipped with the SI-1452-1 layout. These distinctions, between different languages and different layouts seem out of the driver's scope, and so we must resolve to a lower mutual criteria. The one I thought about is physical key positions. To my understanding, most keyboards have the same set of physical keys whose interpretation differs based on the user's preference (handled by the OS). With this mindset, the driver's role would be to simply highlight which keys are currently pressed, which keys have just been released and so on. The role of transforming this into an actual state with resolved control-keys and characters would be delegated to some part of the kernel, or a libc extension.
