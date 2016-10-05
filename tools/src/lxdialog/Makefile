bin=$(NOS_HOME)/tools

HOST_EXTRACFLAGS := -DLOCALE 
ifeq ($(shell uname),SunOS)
HOST_LOADLIBES   := -lcurses -lpanel -lmenu
else
HOST_LOADLIBES   := -lncurses -lpanel -lmenu
endif

ifeq (/usr/include/ncurses/ncurses.h, $(wildcard /usr/include/ncurses/ncurses.h))
        HOST_EXTRACFLAGS += -I/usr/include/ncurses -DCURSES_LOC="<ncurses.h>"
else
ifeq (/usr/include/ncurses/curses.h, $(wildcard /usr/include/ncurses/curses.h))
        HOST_EXTRACFLAGS += -I/usr/include/ncurses -DCURSES_LOC="<ncurses/curses.h>"
else
ifeq (/usr/include/ncurses.h, $(wildcard /usr/include/ncurses.h))
        HOST_EXTRACFLAGS += -DCURSES_LOC="<ncurses.h>"
else
	HOST_EXTRACFLAGS += -DCURSES_LOC="<curses.h>"
endif
endif
endif

hostprogs-y	:= lxdialog
always		:= ncurses $(hostprogs-y)

lxdialog-objs := checklist.o menubox.o textbox.o yesno.o inputbox.o \
		 util.o lxdialog.o msgbox.o

.PHONY: lxdialog $(bin)/ncurses

lxdialog: $(bin)/lxdialog.exe

clean:
	-rm -f $(lxdialog-objs)
#	-rm -f $(bin)/lxdialog.exe

$(bin)/lxdialog.exe: $(lxdialog-objs)
	$(CC) -o $@ $(lxdialog-objs) $(HOST_LOADLIBES)

.c.o:
	$(CC) -c $(HOST_EXTRACFLAGS) $<

$(bin)/ncurses:
	@echo "main() {}" > lxtemp.c
	@if $(CC) lxtemp.c  $(HOST_LOADLIBES); then \
		rm -f lxtemp.c a.exe; \
	else \
		rm -f lxtemp.c; \
		echo -e "\007" ;\
		echo ">> Unable to find the Ncurses libraries." ;\
		echo ">>" ;\
		echo ">> You must install ncurses-devel in order" ;\
		echo ">> to use 'make menuconfig'" ;\
		echo ;\
		exit 1 ;\
	fi
