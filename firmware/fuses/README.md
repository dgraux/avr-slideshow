ATmega168's fuses and locks
===========================

> A brief documentation about these special bytes !  
> You should find more info in the official datasheet avaliable on ATMEL's web site.  

Introduction
------------

It is important to remember there are special bytes in the ATmega168 used to configure it.  
In our case, there are 3 bytes relative to fuses and 1 lock byte.  

Warning, with these bytes, the bits are programmed with '0' as value !!  

Fuses
-----

Only the bits used in this project are descibed here.  
The following arrays give the bit name, the number, a short decription and the default value.  

The 'extended' fuse:  
-	        7	-			1  
-	       	6	-			1  
-	       	5	-			1  
-	       	4	-			1  
-	       	3	-			1  
BOOTSZ1	       	2	Select boot size	0  
BOOTSZ2	       	1	Select boot size	0  
BOOTRST	       	0	Select reset vector	1  

The 'high' fuse:  
RSTDISBL	7	-			1  
DWEN		6	-			1  
SPIEN		5	SPI programming		0  
WDTON		4	-			1  
EESAVE		3	-			1  
BODLEVEL2	2	-			1  
BODLEVEL1	1	-			1  
BODLEVEL0	0	-			1  

The 'low' fuse:  
CKDIR8	  	7	Divide clock by 8	0  
CKOUT		6	Clock output		1  
SUT1		5	Select start-up time	1  
SUT0		4	Select start-up time	0  
CKSEL3		3	Select clock source	0  
CKSEL2		2	Select clock source	0  
CKSEL1		1	Select clock source	1  
CKSEL0		0	Select clock source	0  

Only the low one is changed:  
Default value: 01100010 -> 62  
Changed value: 11100111 -> E7  

In fact, the 'CKDIV8' is disabled and SUT and CKSEL are turned to Crystal Oscillator with fast rising power.  

Locks
-----



Other
-----

There are also bytes about signature, calibration and size but we do not use them in this project.  

Scripts in the directory
------------------------

'modify'............... change the clock used to an external one with no clock divider  
'default'.............. switch back to the default configuration  
'lockall'.............. block the system from new flash  
