# LITOS Hardware
In case you want to order LITOS directly from a PCB manufacturing service, the folder [*PCB design data*](PCB%20design%20data) contains the needed information (Gerbers file with PCB position file, schematics, and bill of material). Additionally, you can find the raw KiCad PCB layout project under [*LITOS Kicad Rev4.zip*](PCB%20design%20data/LITOS%20Kicad%20Rev4.zip). 

For the proper alignment of the microtiter plate on the LED matrix the blueprints (for building them with a 3D printer or a CNC machine) for two different masks (one / two microplates) are provided under [*Alignment masks*](Alignment%20masks). The mask for one microplate allows its placement in the middle of the LED matrix, whereas the mask for two plates stocks them horizontally. This allows to stimulate two multiwell plates with one LITOS to further increase through-put. Please note that when using the mask for two plates, the first and last row cannot be illuminated. 

An optional 3D printed case that protects LITOS can be found in the [*3D printed case*](3D%20printed%20case) folder. The case comes in two versions, with or without ventilation holes. Albeit, given that LITOS does not heat up excessively, you can use whichever version you prefer. To minimize overhangs, we recommend you print the case in the orientation it is stored in the .stl file, the open side of the case facing the printer plate. To adjust for under- or overextruding printers, changing the scale of the whole design worked well for us (98%-102%). No support structures were needed with our printer (Creality CR 20 Pro).

<img src="https://github.com/alandolt/LITOS/blob/master/images/overview_3d_prints.jpg" alt="a) closed design, b) desgin with ventilation holes, c) photo of printed case with ventilation holes" width="500" class="center"/>

<br>

> **Warning** By default, the source code adapted for revision 4 is used during the programming process. When you are still using an older revision 3 board, you can choose the adapted source code for this version by selecting LITOS_Rev2_Rev3 as build environment in PlatformIO. 


<img src="https://github.com/alandolt/LITOS/blob/master/images/flash_litos_rev2.png" alt="Chose the build environment LITOS_Rev2_Rev3 when you still use an old Rev2 or Rev3 LITOS, else you can directly choose LITOS as build environment" width="500" class="center"/>

*Chose the build environment LITOS_Rev2_Rev3 when you still use an old Rev2 or Rev3 LITOS, else you can directly choose LITOS as build environment.*

