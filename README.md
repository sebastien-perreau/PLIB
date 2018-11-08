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
ble | | | | | DMA2 & UART4

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
log | | | | | DMA6 & UARTx
s21_uart | | | | | 
