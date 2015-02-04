
CC=sdcc.exe
AS=asx8051.exe

AFLAGS=-losagj

#These flags seem to work
INCLUDES=-ID:\Embedded\Software\SDCC\include -ID:\Embedded\Software\SDCC\include\mcs51
CFLAGS=--debug --model-small
LDFLAGS=--code-loc 0x0000 --xram-loc 0x6000 --model-small --out-fmt-ihx

#Notice that xram-loc is required when using "xdata" variables.  Choose a value that won't overlap with code or any 8051 areas

#Give an output dir
OBJDIR=.
SRCDIR=.

#Set your target hex file
TARGET=SOS.ihx

#List all of your source files used (both ASM and C files), but with a .rel extension
OBJS=$(OBJDIR)\main.rel $(OBJDIR)\task.rel $(OBJDIR)\test.rel $(OBJDIR)\serial.rel $(OBJDIR)\sem.rel $(OBJDIR)\user_tasks.rel $(OBJDIR)\onew.rel

#Everything below this line should just work
.SUFFIXES : .hex .rel .c .asm
CLEANEXTNS=*.lst *.map *.lnk *.adb *.asm *.rel *.rst *.sym *.mem *~

all: $(OBJDIR)\$(TARGET)

$(OBJDIR)\$(TARGET): $(OBJS)
	$(CC) -o $@ $(INCLUDES) $(LDFLAGS) $**

{$(SRCDIR)\}.asm{$(OBJDIR)\}.rel:
	$(AS) $(AFLAGS) $<
	@move $(@F) $(OBJDIR)/
	@move $(@B).lst $(OBJDIR)/
	@move $(@B).sym $(OBJDIR)/

{$(SRCDIR)\}.c{$(OBJDIR)\}.rel:
	$(CC) -o $@ -c $(CFLAGS) $<

clean:
	cd $(OBJDIR)
	rm $(CLEANEXTNS)

rebuild: clean all
