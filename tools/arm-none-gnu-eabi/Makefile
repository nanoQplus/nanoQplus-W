OS=$(shell uname)
ifeq ($(findstring CYGWIN, $(OS)), CYGWIN)
	OS=Cygwin
endif

ifeq ($(OS), Cygwin)

GCC_URL=https://launchpad.net/gcc-arm-embedded/4.9/4.9-2014-q4-major/+download/gcc-arm-none-eabi-4_9-2014q4-20141203-win32.zip
GCC_PACKAGE=gcc-arm-none-eabi-4_9-2014q4-20141203-win32.zip

else ifeq ($(OS), Darwin)

GCC_URL=https://launchpad.net/gcc-arm-embedded/4.9/4.9-2014-q4-major/+download/gcc-arm-none-eabi-4_9-2014q4-20141203-mac.tar.bz2
GCC_PACKAGE=gcc-arm-none-eabi-4_9-2014q4-20141203-mac.tar.bz2

else ifeq ($(OS), Linux)

GCC_URL=https://launchpad.net/gcc-arm-embedded/4.9/4.9-2014-q4-major/+download/gcc-arm-none-eabi-4_9-2014q4-20141203-linux.tar.bz2
GCC_PACKAGE=gcc-arm-none-eabi-4_9-2014q4-20141203-linux.tar.bz2

endif

GCC_INSTALL=gcc-arm-none-eabi-4_9-2014q4

all: pkg_check $(GCC_INSTALL)
	@echo "### NOTE ###"
	@echo "- Download and install $(GCC_INSTALL) completed."
	@echo "- Add 'NOS_HOME/tools/arm-none-gnu-eabi/$(GCC_INSTALL)/bin' into your PATH variable."

pkg_check :
	@echo "pkg_check: tools/arm-none-gnu-eabi/makefile"
ifeq ($(shell which wget),)
	@echo "'wget' is not installed."
	@false
endif
ifeq ($(shell which unzip),)
	@echo "'unzip' is not installed."
	@false
endif

$(GCC_INSTALL) : $(GCC_PACKAGE)
	@echo $(suffix $(GCC_PACKAGE))
ifeq ($(suffix $(GCC_PACKAGE)), .zip)
	mkdir $(GCC_INSTALL)
	unzip -o $(GCC_PACKAGE) -d $(GCC_INSTALL)
else ifeq ($(suffix $(GCC_PACKAGE)), .bz2)
	tar xvjf $(GCC_PACKAGE)
endif

$(GCC_PACKAGE):
	wget --no-check-certificate $(GCC_URL) -O $(GCC_PACKAGE)

clean:
	rm -rf $(GCC_PACKAGE) $(GCC_INSTALL)
