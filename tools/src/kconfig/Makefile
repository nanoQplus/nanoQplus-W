# ===========================================================================
# Kernel configuration targets
# These targets are used from top-level makefile

bin=$(NOS_HOME)/tools

.PHONY: config menuconfig

all: config menuconfig

config: $(bin)/conf.exe

menuconfig: $(bin)/mconf.exe
	@cd ../lxdialog; $(MAKE)

clean:
	-rm -f *.o zconf.tab.c zconf.tab.h lex.zconf.c
	-rm -f zconf.output lex.backup
	-rm -f $(bin)/conf.exe
	-rm -f $(bin)/mconf.exe
	@cd ../lxdialog; $(MAKE) clean

$(bin)/conf.exe: conf.o zconf.tab.o
	$(CC) -o $@ conf.o zconf.tab.o

$(bin)/mconf.exe: mconf.o zconf.tab.o
	$(CC) -o $@ mconf.o zconf.tab.o

zconf.tab.o: zconf.tab.c zconf.tab.h

zconf.tab.c: zconf.y confdata.c symbol.c menu.c expr.c lex.zconf.c

%.tab.c: %.y
	bison -t -d -v -b $* -p $(notdir $*) $<

lex.%.c: %.l
	flex -P$(notdir $*) -o$@ $<
