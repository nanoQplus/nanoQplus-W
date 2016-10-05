/*
 * Copyright (c) 2006-2014
 * Electronics and Telecommunications Research Institute (ETRI)
 * All Rights Reserved.
 *
 * Following acts are STRICTLY PROHIBITED except when a specific prior written
 * permission is obtained from ETRI or a separate written agreement with ETRI
 * stipulates such permission specifically:
 *
 * a) Selling, distributing, sublicensing, renting, leasing, transmitting,
 * redistributing or otherwise transferring this software to a third party;
 *
 * b) Copying, transforming, modifying, creating any derivatives of, reverse
 * engineering, decompiling, disassembling, translating, making any attempt to
 * discover the source code of, the whole or part of this software in source or
 * binary form;
 *
 * c) Making any copy of the whole or part of this software other than one copy
 * for backup purposes only; and
 *
 * d) Using the name, trademark or logo of ETRI or the names of contributors in
 * order to endorse or promote products derived from this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS," WITHOUT A WARRANTY OF ANY KIND. ALL
 * EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING ANY
 * IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NON-INFRINGEMENT, ARE HEREBY EXCLUDED. IN NO EVENT WILL ETRI (OR ITS
 * LICENSORS, IF ANY) BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR
 * DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR PUNITIVE DAMAGES,
 * HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF LIABILITY, ARISING FROM, OUT
 * OF OR IN CONNECTION WITH THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN
 * IF ETRI HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 *
 * Any permitted redistribution of this software must retain the copyright
 * notice, conditions, and disclaimer as specified above.
 */
     
 /**
  * @file spi.c
  * @breif SPI module for STM32F4
  * @author hj.park@etri.re.kr (ETRI)
  * @date 2016. 06. 13
  * @description Use 2nd channel only in STM32F4.  1st channel conflicts with JTAG.
  */

#include "spi.h"
#include "critical_section.h"

// TBD
int nos_spi_dma_init(uint8_t channel) {
#if 0
    DMA_InitTypeDef DMA_InitStructure;

    if (channel == 1)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

        /* SPI TX DMA config */
        DMA_DeInit(SPI1_TX_DMA_CH);
        DMA_InitStructure.DMA_Priority = DMA_Priority_Low; // must be lower than SPI RX priority
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
        DMA_InitStructure.DMA_BufferSize = 0;   //temporal
        DMA_InitStructure.DMA_MemoryBaseAddr = 0;   //temporal
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(SPI1->DR));
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        DMA_Init(SPI1_TX_DMA_CH, &DMA_InitStructure);
        DMA_ITConfig(SPI1_TX_DMA_CH, DMA_IT_TC, ENABLE);

        /* SPI RX DMA config */
        DMA_DeInit(SPI1_RX_DMA_CH);
        DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; // // must be higher than SPI RX priority
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
        DMA_Init(SPI1_RX_DMA_CH, &DMA_InitStructure);
        DMA_ITConfig(SPI1_RX_DMA_CH, DMA_IT_TC, ENABLE);
    }
    else if (channel == 2)
    {        
    }
#endif
    return EXIT_SUCCESS;
}

void init_SPI2() {

	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;

	// enable clock for used IO pins
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);


	/* configure pins used by SPI2
	 */
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// connect SPI2 pins to SPI alternate function
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);


	/* Configure the chip select pin
	   in this case we will use PB12 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIOB->BSRRL |= GPIO_Pin_12; // set PE7 high

	// enable peripheral clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	/* configure SPI2 in Mode 0 
	 * CPOL = 0 --> clock is low when idle
	 * CPHA = 0 --> data is sampled at the first edge
	 */
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // set to full duplex mode, seperate MOSI and MISO lines
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;     // transmit in master mode, NSS pin has to be always high
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b; // one packet of data is 8 bits wide
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;        // clock is low when idle
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;      // data sampled at first edge
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft; // set the NSS management to internal and pull internal NSS high
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; // SPI frequency is APB2 frequency / 4
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;// data is transmitted MSB first
	SPI_Init(SPI2, &SPI_InitStruct); 

	SPI_Cmd(SPI2, ENABLE); // enable SPI2
} // end func




// Master mode only, no interrupt
int nos_spi_init(uint8_t channel, SPI_InitTypeDef *spi_init_p) {

	switch (channel) {
		case 1:
			// currently, not using
			return EXIT_FAILURE;
			break;
		case 2:
			// experimental code
			// spi_init_p is ignored currently
			init_SPI2();
			break;
		default:
			return EXIT_FAILURE;
	} // end switch
	return EXIT_SUCCESS;
} // end func

// Master mode only
uint8_t nos_spi_txrx(uint8_t channel, uint8_t tx_byte) {
	
	switch(channel) {
		case 1:
			return 0; // dummy value
			break;
		case 2:
			GPIOB->BSRRH |= GPIO_Pin_12;
	
			SPI2->DR = tx_byte; // write data to be transmitted to the SPI data register
			while( !(SPI2->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
			while( !(SPI2->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
			while( SPI2->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
			return SPI2->DR;
			break;
		default:
			return 0; // dummy value
	} // end switch
	return 0; // dummy value
} // end func

