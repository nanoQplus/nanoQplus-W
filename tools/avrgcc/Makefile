WINAVR_URL=http://sourceforge.net/projects/winavr/files/WinAVR/20100110/WinAVR-20100110-install.exe
WINAVR_EXE=WinAVR-20100110-install.exe

all : pkg_check $(WINAVR_EXE)
	@chmod +x $(WINAVR_EXE)
	@echo "### NOTE ###"
	@echo "- Downloading $(WINAVR_EXE) is done. Continue installation via Windows Installer."
	@echo

pkg_check :
ifeq ($(shell which curl),)
	@echo "'curl' is not installed."
	@false
endif

$(WINAVR_EXE) :
	curl -L $(WINAVR_URL) > $(WINAVR_EXE)

clean :
	rm $(WINAVR_EXE)
