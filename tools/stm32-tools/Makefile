all : stm32flash

stm32flash : stm32flash-read-only
	make -C stm32flash-read-only
	mv stm32flash-read-only/stm32flash .

stm32flash-read-only :
	svn export http://stm32flash.googlecode.com/svn/trunk/ stm32flash-read-only
#	git clone https://code.google.com/p/stm32flash/ 

clean :
	rm -rf stm32flash-read-only stm32flash
