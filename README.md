# PLIB_21811

**Prototype Library ver. *November 18***

## HARDWARE BASED ON:

Target: ***SPCB*** - (**S**)tandard (**P**)rototype (**C**)alculator (**B**)oard

- BLE PICKIT rev.C (*Prototype dev.*) or ETH PICKIT
- Ethernet Starter KIT II (*PIC32MX795F512L*)
- PICAdapter rev.D (*Prototype dev.*)


## HOW TO USE THE LIBRARY:

There are two ways to use the library in your project: 
* Either by include it as a ***Library Project***.
* Or by include the ***Pre-compiled*** library.

In both cases, you have to include ***PLIB.h***, which includes all library's header file, in your project:
* In your project, open **config.h** and at the top of the file, include the **PLIB.h** following the directory where it is stored. 
* Now it is necessary to include the file **PLIB.h** in your **Header folder** project. Right click on the **Header folder** and click **Add Existing Item...**. Search and select the file to include it in your project.

Now you have to include the library (either the **Library project** or the **Pre-compiled** library):

#### 1. Library project
* Right click on your **project --> Properties --> Conf: [default] --> Libraries**.
* Click on **Add Library Project...** and select **PLIB.X** (*the MPLABX IDE library project*).
* Remove previous library if present.

#### 2. Pre-compiled library
* Right click on your **project --> Properties --> Conf: [default] --> Libraries**.
* Click on **Add Library/Object File...** and select **PLIB.a** (*the compiled library*).
* Remove previous library if present.

## LIBRARY STATUS

#### Low Level
Name | Tested & Validated | Commented | Autonomous | Example
-----|--------------------|-----------|------------|--------
s08_interrupt_mapping | | | |
s12_ports | yes | yes | yes | yes
s14_timers | yes | yes | yes | yes
s16_output_compare | | | | 
s17_adc | | | | 
s21_uart | | | | 
s23_spi | | | | 
s24_i2c | | | | 
s34_can | | | | 
s35_ethernet | | | | 

#### High Level
Name | Tested & Validated | Commented | Autonomous | Example | Dependencies
-----|--------------------|-----------|------------|---------|-------------
utilities | | | | | 
string_advance | | | | | 
one_wire_communication | | | | | 
lin | | | | | 
ble | | | | | 

#### External Components
Name | Tested & Validated | Commented | Autonomous | Example | Dependencies
-----|--------------------|-----------|------------|---------|-------------
eeprom | | | | | 
mcp23s17 | | | | | 
ws2812b | | | | | 
qt2100 | | | | | 
amis30621 | | | | | 
tmc429 | | | | | 

#### Experimental
Name | Stable | Commented | Autonomous | Example | Dependencies
-----|--------------------|-----------|------------|---------|-------------
log | | | | | 
s21_uart | | | | | 

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
