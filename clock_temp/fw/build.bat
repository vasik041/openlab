del *.bak
del *.lst
del *.r90
del *.hex
c:\iar\ew22demo\a90\bin\icca90 -v0 -mt -e -gA -RCODE -l main.lst -q -t8 -IC:\IAR\EW22DEMO\A90\inc\ -z9 -K main.c
if errorlevel 1 goto end
c:\iar\ew22demo\a90\bin\icca90 -v0 -mt -e -gA -RCODE -l display.lst -q -t8 -IC:\IAR\EW22DEMO\A90\inc\ -z9 -K display.c
if errorlevel 1 goto end
c:\iar\ew22demo\a90\bin\icca90 -v0 -mt -e -gA -RCODE -l i2c.lst -q -t8 -IC:\IAR\EW22DEMO\A90\inc\ -z9 -K i2c.c
if errorlevel 1 goto end
c:\iar\ew22demo\a90\bin\icca90 -v0 -mt -e -gA -RCODE -l time.lst -q -t8 -IC:\IAR\EW22DEMO\A90\inc\ -z9 -K time.c
if errorlevel 1 goto end
c:\iar\ew22demo\a90\bin\icca90 -v0 -mt -e -gA -RCODE -l ds1820.lst -q -t8 -IC:\IAR\EW22DEMO\A90\inc\ -z9 -K ds1820.c
if errorlevel 1 goto end
c:\iar\ew22demo\a90\bin\xlink main.r90 display.r90 i2c.r90 time.r90 ds1820.r90 -o main.hex -Fintel-standard -l main.map -xms -IC:\IAR\EW22DEMO\A90\LIB\ -f lnk.xcl
if errorlevel 1 goto end

hexbin main.hex main.bin
rem isp -c2 -p tiny26 -s -k
if errorlevel 1 goto end
rem isp -c2 -p tiny26 -w main.bin
:end

                      