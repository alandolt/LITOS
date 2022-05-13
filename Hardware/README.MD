# LITOS Hardware
In most cases it is recomended to order [LITOS directly](../../wiki/1.-How-to-get-one). 

In case you really want to order LITOS directly from a PCB manufacturing service, this folder contains the needed information (Gerbers file with PCB position file, schematics and bill of material). 
Additionally, you can find the raw KiCad PCB layout project folder in the folder PCB design data\Kicad project folder.  

***Attention:*** In revision 3 there was an error in the provided PCB files. It was not possible to flash your LITOS over USB, you had to use an external USB-FTDI programmer (connected to LITOS' RX, TX pins on J7) while disconnecting the FTDI USB to serial converter. 

This error had been fixed in **revision 4** while improving trace routing too. Further revision 4 now uses more available components derived from LCSC. 

We are currently in the progress of ordering and testing revision 4 ourselves. Once this is done, we will update this post. However, in the meantime, you are free to try out revision 4 for yourself. The software of revision 3 is compatible with the PCB revision 4, albeit the button order is reversed (the physical red button switches the green button and vice versa). This bug will be fixed as soon as the testing for rev. 4 is completed.  
