# -*- mode:Makefile; -*-
###############################################################################
# Default Makefile for NanoQplus3 module management                           #
# Author: Haeyong Kim (ETRI)                                                  #
#                                                                             #
#NOSDIR: headers + sources + sources for archive                              #
#NOSLIB: sources for archive only (NOSDIR is used for looking for headers)    #
###############################################################################


### Variables
RM	:= rm -f
MD := mkdir -p
RD := rm -rf
CP := cp
MV := mv
TOUCH := touch
AR := ar



### Read Kconfig configuration if exist
-include $(PWD)/.config

#pre-definitions
PREDEFINES = $(MCU)


### Set NanoQplus Source Directories
NOSDIR = \
	$(NOS_HOME)/nos\
	$(NOS_HOME)/nos/include\
	$(NOS_HOME)/nos/include/net\
	$(NOS_HOME)/nos/arch\
	$(NOS_HOME)/nos/arch/$(MCU)\
	$(NOS_HOME)/nos/platform\
	$(NOS_HOME)/nos/platform/$(PLATFORM)\
	$(NOS_HOME)/nos/platform/$(PLATFORM)/pm\
	$(NOS_HOME)/nos/platform/$(PLATFORM)/uart\
	$(NOS_HOME)/nos/drivers\
	$(NOS_HOME)/nos/lib


ifeq ($(CONFIG_KERNEL_M),y)
	NOSDIR += \
		$(NOS_HOME)/nos/kernel \
		$(NOS_HOME)/nos/kernel/thread \
		$(NOS_HOME)/nos/kernel/base 
endif

ifeq ($(CONFIG_LMC1623_M),y)
	NOSDIR += $(NOS_HOME)/nos/drivers/lmc1623
endif

ifeq ($(CONFIG_CC1120_M),y)
	NOSDIR += $(NOS_HOME)/nos/drivers/wpan_dev/cc1120
endif

ifeq ($(CONFIG_CC2520_M),y)
	NOSDIR += $(NOS_HOME)/nos/drivers/wpan_dev/cc2520
endif

ifeq ($(CONFIG_WPAN_DEV_M),y)
	NOSDIR += $(NOS_HOME)/nos/drivers/wpan_dev
endif	

ifeq ($(CONFIG_SLIP_M),y)
	NOSDIR += $(NOS_HOME)/nos/net/slip
	NOSLIB += $(NOS_HOME)/nos/net/slip
endif

ifeq ($(CONFIG_LR_WPANS_M),y)
	NOSDIR += $(NOS_HOME)/nos/net/802.15.4
	NOSLIB += $(NOS_HOME)/nos/net/802.15.4
endif

ifeq ($(CONFIG_SIXLO_M),y)
	NOSDIR += $(NOS_HOME)/nos/net/6lowpan
	NOSLIB += $(NOS_HOME)/nos/net/6lowpan
endif

ifeq ($(CONFIG_NIP6_M),y)
	NOSDIR += $(NOS_HOME)/nos/net/inet_interface
	NOSDIR += $(NOS_HOME)/nos/net/ipv6
	NOSLIB += $(NOS_HOME)/nos/net/inet_interface
	NOSLIB += $(NOS_HOME)/nos/net/ipv6
endif

ifeq ($(CONFIG_RPL_M),y)
	NOSDIR += $(NOS_HOME)/nos/net/rpl
	NOSLIB += $(NOS_HOME)/nos/net/rpl
endif

ifeq ($(CONFIG_ZIP_M),y)
	NOSDIR += $(NOS_HOME)/nos/net/zigbee_ip
	NOSLIB += $(NOS_HOME)/nos/net/zigbee_ip
endif

ifeq ($(CONFIG_NTCP_M),y)
	NOSDIR += $(NOS_HOME)/nos/net/tcp
	NOSLIB += $(NOS_HOME)/nos/net/tcp
endif

ifeq ($(CONFIG_UDP_M),y)
	NOSDIR += $(NOS_HOME)/nos/net/udp
	NOSLIB += $(NOS_HOME)/nos/net/udp
endif

ifeq ($(CONFIG_COAP_M),y)
	NOSDIR += $(NOS_HOME)/nos/net/coap
	NOSLIB += $(NOS_HOME)/nos/net/coap
endif

ifeq ($(CONFIG_NBR_MGMT_M),y)
	NOSDIR += $(NOS_HOME)/nos/net/neighbor_mgmt
	NOSLIB += $(NOS_HOME)/nos/net/neighbor_mgmt
endif










ifeq ($(CONFIG_INET_M),y)
	NOSDIR += $(NOS_HOME)/nos/net/ip6
endif

ifeq ($(CONFIG_MPL_M),y)
	NOSDIR += $(NOS_HOME)/nos/net/mpl
endif

ifeq ($(CONFIG_ETH_DEV_M),y)
	NOSDIR += $(NOS_HOME)/nos/drivers/eth_dev
	NOSDIR += $(NOS_HOME)/nos/drivers/eth_dev/enc28j60
endif

ifeq ($(CONFIG_ETHERNET6_M),y)
	NOSDIR += $(NOS_HOME)/nos/net/ethernet6
endif

ifeq ($(CONFIG_MLE_M),y)
	NOSDIR += $(NOS_HOME)/nos/net/mle
endif

ifeq ($(CONFIG_CFD_M),y)
	NOSDIR += $(NOS_HOME)/support/storage/cfd
	NOSDIR += $(NOS_HOME)/support/storage/cfd/ecc
	NOSDIR += $(NOS_HOME)/support/storage/cfd/lld
endif

ifeq ($(CONFIG_SSL_M),y)
	NOSDIR += $(NOS_HOME)/support/storage/ssl
endif

### Include Makefiles
include $(NOS_HOME)/nos/arch/$(MCU)/Makefile.build
include $(NOS_HOME)/nos/platform/$(PLATFORM)/Makefile.build

#must be included last
ifeq ($(CONFIG_GCC_TOOLCHAIN),y)
	include $(NOS_HOME)/support/make/Makefile.gnuc
else ifeq ($(CONFIG_IAR_TOOLCHAIN),y)
	include $(NOS_HOME)/support/make/Makefile.ewarm
else ifeq ($(CONFIG_KEIL_TOOLCHAIN),y)
	include $(NOS_HOME)/support/make/Makefile.uv4
endif

