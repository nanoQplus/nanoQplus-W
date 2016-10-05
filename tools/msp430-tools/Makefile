GCC_URL=http://downloads.sourceforge.net/project/mspgcc/Windows/mingw32/mspgcc-20120406-p20120911.zip
GCC_PACKAGE=mspgcc-20120406-p20120911.zip
GCC_INSTALL=mspgcc-20120406-p20120911

DEBUG_URL=http://gnutoolchains.com/mspdebug/download/mspdebug-0.19-windows.zip
DEBUG_PACKAGE=mspdebug-0.19-windows.zip
DEBUG_INSTALL=mspdebug-0.19

all: pkg_check gcc debug
	@echo "### NOTE ###"
	@echo "- Download and install both $(GCC_INSTALL) and $(DEBUG_INSTALL) completed."
	@echo "- Add '$(PWD)/$(GCC_INSTALL)/bin' into your PATH variable."

pkg_check :
ifeq ($(shell which wget),)
	@echo "'wget' is not installed."
	@false
endif
ifeq ($(shell which curl),)
	@echo "'curl' is not installed."
	@false
endif
ifeq ($(shell which unzip),)
	@echo "'unzip' is not installed."
	@false
endif

debug : $(DEBUG_PACKAGE)
	unzip $(DEBUG_PACKAGE)
	cp $(PWD)/MSP430.DLLv3/Cygwin/*.dll $(PWD)/$(DEBUG_INSTALL)
	chmod +x -R $(DEBUG_INSTALL)

$(DEBUG_PACKAGE) :
	wget $(DEBUG_URL)

gcc : $(GCC_PACKAGE)
	mkdir $(GCC_INSTALL)
	unzip $(GCC_PACKAGE) -d $(GCC_INSTALL)

$(GCC_PACKAGE):
	curl -L $(GCC_URL) > $(GCC_PACKAGE)

clean:
	rm -rf $(GCC_PACKAGE) $(GCC_INSTALL)
	rm -rf $(DEBUG_PACKAGE) $(DEBUG_INSTALL)
