# Makefile to download STM32F4 series standard peripheral libraries

#LIB_F40X=STM32F4-Discovery_FW_V1.1.0
#URL_F40X=http://www.st.com/st-web-ui/static/active/en/st_prod_software_internet/resource/technical/software/firmware/stsw-stm32068.zip
#FILE_F40X=stsw-stm32068.zip
LIB_F40X=STM32F4xx_DSP_StdPeriph_Lib_V1.6.1
FILE_F40X=stm32f4_dsp_stdperiph_lib.zip

all : f40x

################################## STM32F40x ###################################
stm32f40x : f40x

f40x : pkg_check f40x_errata

f40x_errata : $(LIB_F40X)

$(LIB_F40X) : $(FILE_F40X)
	@unzip -o $<

$(FILE_F40X) :
	@wget $(URL_F40X)

clean_f40x:
	@rm -rf $(LIB_F40X) $(FILE_F40X)
################################################################################

pkg_check :
ifeq ($(shell which wget),)
	@echo "'wget' is not installed."
	@false
endif
ifeq ($(shell which unzip),)
	@echo "'unzip' is not installed."
	@false
endif

clean : clean_f40x
	rm -rf tmp
