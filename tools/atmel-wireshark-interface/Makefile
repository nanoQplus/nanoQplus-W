# -*- mode:Makefile; -*-

BIN=Wireshark_Sniffer_Interface.exe
URL=https://spaces.atmel.com/svn/wireshark_cap/trunk/Windows/Wireshark_Sniffer_Interface/Wireshark_Sniffer_Exe/$(BIN)

all : $(BIN)

$(BIN) :
	@echo "Exporting $(BIN) from Wireshark Capture Interface Project <https://spaces.atmel.com/gf/project/wireshark_cap> of Atmel Spaces..."
	@svn export --username anonymous --password "" $(URL)
	@chmod +x $(BIN)
	@echo
	@echo "### Note ###"
	@echo "- Execute $(BIN). Enjoy!"

clean :
	@rm -rf $(BIN)

run : all
	@$(shell pwd)/$(BIN)
