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
* In your project **BLANCK_PROJECT**, open **config.h** and at the top of the file, include the **PLIB.h** following the directory where it is stored. 
* Then include the file **PLIB.h** in your **Header folder** project. Right click on the **Header folder** and **Add Existing Item...**. Search and select the file to include it in your project.

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

Name | Validated | Commented | Autonomous | Example | Dependencies
 :--- | :---: | :---: | :---: | :---: | :---: 
**Low Level** | ************ | ************ | ************ | ************ | ************
*s08_interrupt_mapping* | yes | yes | yes | |
*s12_ports* | yes |  |  |  |
*s14_timers* | yes | yes | yes | |
*s16_output_compare* | | | | | T2 & T3
*s17_adc* | | | | |
*s23_spi* | | | | | T1 & GPIO & \*DMAx
*s24_i2c* | | | | | T1
*s34_can* | | | | | T1
*s35_ethernet* | | | | | T1
**High Level** | ************ | ************ | ************ | ************ | ************
*utilities* | | | | | 
*string_advance* | | | | | 
*one_wire_communication* | | | | | 
*lin* | | | | | UART*2* & UART*5*
*ble* | | | | | UART*4* & DMA*2*
**External Components** | ************ | ************ | ************ | ************ | ************
*25lc512* | | | | | SPI*x* & DMA*x*
*mcp23s17* | | | | | SPI*x* & DMA*x*
*ws2812b* | | | | | SPI*x* & DMA*x*
*qt2100* | | | | | SPI*x* & DMA*x*
*amis30621* | | | | | LIN*2* & LIN*5*
*tmc429* | | | | | SPI*x* & DMA*x*
**Experimental** | ************ | ************ | ************ | ************ | ************
*EXP_log* | | | | | UART*x* & DMA*x*
*EXP_s21_uart* | | | | | 
