PROGRAM_NAME = cloakof64
TARGET = -tc64
VCFLAGS = -u __EXEHDR__ -O -g 
CFLAGS = -u __EXEHDR__ -O --static-locals
BINARY = bin/$(PROGRAM_NAME).prg
MGLIB = mgLib/vid_asm_minimal.s mgLib/upk.c mgLib/upk_asm.s mgLib/mem_asm_minimal.s -I mgLib

all: build_game 

build_game: 
	cl65 -I .. $(TARGET) main.c $(MGLIB) -o $(BINARY) $(VCFLAGS)
	c1541.exe -format $(PROGRAM_NAME),666 d64 bin/$(PROGRAM_NAME).d64 -attach bin/$(PROGRAM_NAME).d64 -write bin/$(PROGRAM_NAME).prg $(PROGRAM_NAME)
	script\img\addimages.bat bin/$(PROGRAM_NAME).d64

run:
	x64sc.exe bin/$(PROGRAM_NAME).d64
debug:
	x64sc.exe -moncommands bin/$(PROGRAM_NAME).dbg bin/$(PROGRAM_NAME).prg
d64:
	c1541.exe -format diskname,id d64 bin/$(PROGRAM_NAME).d64 -attach bin/$(PROGRAM_NAME).d64 -write bin/$(PROGRAM_NAME).prg $(PROGRAM_NAME)
clean:
	del *.o
	del *.prg
	del bin\*.o
	del bin\*.prg
