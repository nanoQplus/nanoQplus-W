deps_config := \
	support/storage/Kconfig \
	nos/lib/Kconfig \
	nos/kernel/Kconfig \
	Kconfig \
	nos/arch/stm32f40x/Kconfig \
	/cygdrive/c/NOS3-Tickless-s1/nos/platform/stm32f4_discovery/Kconfig

.config include/linux/autoconf.h: $(deps_config)

$(deps_config):
