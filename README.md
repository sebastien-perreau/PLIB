# PLIB_21810

Prototype Library ver. October 18

## HARDWARE BASED ON:

Target: ***SPCB*** - (**S**)tandard (**P**)rototype (**C**)alculator (**B**)oard

- BLE PICKIT rev.C (*Prototype dev.*) or ETH PICKIT
- Ethernet Starter KIT II (*PIC32MX795F512L*)
- PICAdapter rev.D (*Prototype dev.*)


## HOW TO USE IT:

1. Include the file "PLIB.h" into your project:/n
		- In your project, open the file "config.h" and include PLIB.h
		#include "../PLIB_21809/PLIB.h"
		- Then add the file "PLIB.h" into your project. Right click on Header Files folder and Add Existing Item...
		- Select "PLIB.h" to include it.
2. Include the library into your project (2 ways)
	2.1. Include the library project (code is open source)
		- Right click on your project, Properties/Conf: [default]/Libraries
		- Add Library Project... then select the last PLIB.X (last library project)
		- Remove previous library version if present.
	2.2. Include the compiled library (code is protected)
		- Right click on your project, Properties/Conf: [default]/Libraries
		- Add Library/Object File... then select the last PLIB.a (last compiled library)
		- Remove previous library version if present.


Release: PLIB_21809
----------------------------------------------------------------------
- Initiale release

						 ________________
						|		 |		Project Library (.x)
						|   PLIB_xxxxx   |	as 		or
						|________________|		Compiled Library (.a)
								
							|
							|	(PLIB.h)
							v
						 ________________
						|		 |	main.c / interrupt.c
						|   User Prog.   |	config.h / config.c
						|________________|	...	

- The aim is to propose always a last version of a standard and centralized library.

- Creation of a Library Project on MPLAB X containing all drivers dispose as follow:
	+ _Experimental (folder)
		Contains all experimental driver developements which need to be validated before being tidy in one of the three next folders. Also contains all _EXAMPLES when available.
	+ _External_Components (folder)
		Contains all external components driver developments such as eeprom, smart led, I/O expander... Each of these external components needs either a _High_Level_Driver or a _Low_Level_Driver to work.
	+ _High_Level_Driver (folder)
		Contains drivers which can use a _Low_Level_Driver such as LIN (UART), BLE (UART) or some other usefull functions integrated in "utilities" or "one_wire_communication".
	+ _Low_Level_Driver (folder)
		Contains drivers (registers level) for the PIC32MX795F512L peripherals. Each of these drivers are tidy by name (starting by "sxx_" where "xx" is the peripheral number - See datasheet on microchip website). 
	defines.h (file)
		Contains some usefull defines, macro and structures. This file is include in "PLIB.h" and should be accesible for the Prototype Library and for the User Program.
	PLIB.h (file)
		Contains all includes for each drivers store in the above folders.

- Most of the drivers need to be (re)validated (because based on an old version).

- What is new:
	+ _High_Level_Driver/utilities
		> SWITCH has been fully re-written. New definition, instance, structure and back_task operations.
		> BUS_MANAGEMENT has been fully re-written. New definition, instance, structure and back_task operations.
