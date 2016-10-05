#include "nos_store_context.h"

void Select_Reset_Mode(uint8_t mode){
	uint32_t* base_addr = (uint32_t *) 0x40024000;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);   // set RCC->APB1ENR.pwren
	PWR_BackupAccessCmd(ENABLE);                          // set PWR->CR.dbp = 1;
	PWR_BackupRegulatorCmd(ENABLE);     // set PWR->CSR.bre = 1;
	
	if(mode==0){
		*(base_addr) = (uint8_t)0x00000000;
	}
	else{
		*(base_addr) = 0x00000001;
	}
	//PWR_BackupAccessCmd(DISABLE);                     // reset PWR->CR.dbp = 0;	
}

uint32_t Find_EPDS(void)
{
	int m_counter=0;
	int i;
	uint32_t startAddress = 0x08080000;
	uint32_t sourceAddress = 0x20000000;
	uint32_t ds_ep=0;
	uint32_t result=0;
	//  *(__IO uint32_t*)Address = *Data;
	for(i=0;i<28672;i++)
	{
		if((*(__IO uint32_t*)(startAddress + (i*4)))==(*(__IO uint32_t*)(sourceAddress + (i*4))))
		{
			m_counter++;
		}
		else{
			m_counter=0;
		}
	}
	if(m_counter>2){
		ds_ep=(startAddress + ((i-m_counter)*4));
	}
	result=(sourceAddress + (ds_ep-startAddress));
	
	return result;
	
}

void Store_DS_IN_BKSRAM(uint32_t ep_sram)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	PWR_BackupRegulatorCmd(ENABLE);
	
	uint32_t startAddress =0x20000000;
	uint32_t DestAddress = 0x40024100;
	 *(__IO uint32_t*)DestAddress = (DestAddress + (ep_sram - startAddress));
	 DestAddress=0x40024104;
	uint32_t SRAMpointer = 0x20000000;	
	uint32_t i;
	for(i=0;i<(ep_sram - startAddress);i+=4)
	{
		 *(__IO uint32_t*)DestAddress = *(__IO uint32_t*)SRAMpointer;
		 DestAddress += 4;
		 SRAMpointer += 4;
	}
}

int8_t write_to_backup_sram(uint32_t *data, uint16_t bytes, uint16_t offset)
{
  const uint16_t backup_size = 0x1000;
  uint32_t* base_addr = (uint32_t* ) BKPSRAM_BASE;
  uint16_t i;
    base_addr+=1;
  if( bytes + offset >= backup_size ) {
    /* ERROR : the last byte is outside the backup SRAM region */
    return -1;
  }
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
  /* disable backup domain write protection */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);   // set RCC->APB1ENR.pwren
  PWR_BackupAccessCmd(ENABLE);                          // set PWR->CR.dbp = 1;
  /** enable the backup regulator (used to maintain the backup SRAM content in
    * standby and Vbat modes).  NOTE : this bit is not reset when the device
    * wakes up from standby, system reset or power reset. You can check that
    * the backup regulator is ready on PWR->CSR.brr, see rm p144 */
  PWR_BackupRegulatorCmd(ENABLE);     // set PWR->CSR.bre = 1;
  for( i = 0; i < bytes; i++ ) {
    *(base_addr + offset + i) = *(data + i);
  }
  PWR_BackupAccessCmd(DISABLE);                     // reset PWR->CR.dbp = 0;
  return 10;
	
}

int8_t read_from_backup_sram(uint8_t *data, uint16_t bytes, uint16_t offset)
{
  const uint16_t backup_size = 0x1000;
  uint8_t* base_addr = (uint8_t *) BKPSRAM_BASE;
  uint16_t i;
  base_addr+=1;
  if( bytes + offset >= backup_size ) {
    /* ERROR : the last byte is outside the backup SRAM region */
    return -1;
  }
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
  for( i = 0; i < bytes; i++ ) {
    *(data + i) = *(base_addr + offset + i);
  }
  return 11;
}

void clear_backup_sram()
{
  const uint16_t backup_size = 0x1000;
  uint8_t* base_addr = (uint8_t *) BKPSRAM_BASE;
  uint16_t i;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE); /* disable backup domain write protection */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);   // set RCC->APB1ENR.pwren
  PWR_BackupAccessCmd(ENABLE);                          // set PWR->CR.dbp = 1;

  PWR_BackupRegulatorCmd(ENABLE);     // set PWR->CSR.bre = 1;
  for( i = 0; i < backup_size; i++ ) {
    *(base_addr + i) = 0;
  }
  PWR_BackupAccessCmd(DISABLE);
}

//  (*(volatile unsigned long*) 0x40023830) |= 0x00000040UL;

void Store_LocalStack_Value(void)
{
	uint32_t StackPointer =(*(volatile uint32_t*) 0x40024058); //Main Program StackPointer Address
	uint32_t DstAddr = 0x40024070; 
	uint32_t EndStackPointer = 0x20020000;

	for(;StackPointer<EndStackPointer;StackPointer+=4, DstAddr+=4)
	{
		*(__IO uint32_t*)DstAddr = *(__IO uint32_t*)StackPointer;
	}
}

void Store_GlobalStack_Value(void)
{
	uint32_t BSSPointer = 0x20000000; //BSS Value
	uint32_t DstAddr = 0x40024200; 	//Backup in BKSRAM
	/*
	uint32_t temp = (*(volatile uint32_t*) 0x40024060); //End of BSS section
	uint32_t EndBSSPointer = (*(volatile uint32_t*) temp);*/
	uint32_t EndBSSPointer	= (*(volatile uint32_t*) 0x40024060); //End of BSS section
	for(;BSSPointer<EndBSSPointer;BSSPointer+=4, DstAddr+=4)
	{
		*(__IO uint32_t*)DstAddr = *(__IO uint32_t*)BSSPointer;
	}
	
}

/* Store SCB configuration struct information*/
/*
#define SCS_BASE            (0xE000E000UL)
#define SCB                 ((SCB_Type *) SCB_BASE)
#define SCB_BASE            (SCS_BASE +  0x0D00UL)  
:0xE000ED00UL ~ + 0x088
Store the context of SCB in BKSRAM (0x40024300~0x40024400)
*/
void Store_SCB_Value(void)
{
	uint32_t SCB_Pointer = 0xE000ED00UL;
	uint32_t Dst_Addr = 0x40024300; 	
	uint32_t End_SCB_Pointer = 0xE000ED88; 
	
	for(;SCB_Pointer<=End_SCB_Pointer;SCB_Pointer+=4, Dst_Addr+=4)
	{
		*(__IO uint32_t*)Dst_Addr = *(__IO uint32_t*)SCB_Pointer;
	}
}

/* 
\brief  Structure type to access the System Timer (SysTick).
 
typedef struct
{
  __IO uint32_t CTRL;  //!< Offset: 0x000 (R/W)  SysTick Control and Status Register
  __IO uint32_t LOAD;            //!< Offset: 0x004 (R/W)  SysTick Reload Value Register
  __IO uint32_t VAL;     //!< Offset: 0x008 (R/W)  SysTick Current Value Register      
  __I  uint32_t CALIB;            //!< Offset: 0x00C (R/ )  SysTick Calibration Register        
} SysTick_Type;

Store Systick Register information
#define SCS_BASE            (0xE000E000UL)
#define SysTick_BASE        (SCS_BASE +  0x0010UL)  
#define SysTick             ((SysTick_Type   *)     SysTick_BASE  )      
:0xE000E010UL~ +0x0c
Store the context of SCB in BKSRAM (0x40024400~0x40024420)
*/
void Store_Systick_Value(void)
{
	uint32_t SysTick_Pointer = 0xE000E010UL;
	uint32_t Dst_Addr = 0x40024400;	
	uint32_t End_SysTick_Pointer = 0xE000E01C; 
	
	for(;SysTick_Pointer<=End_SysTick_Pointer;SysTick_Pointer+=4, Dst_Addr+=4)
	{
		*(__IO uint32_t*)Dst_Addr = *(__IO uint32_t*)SysTick_Pointer;
	}
}

/* Store RTC Register information
#define PERIPH_BASE           ((uint32_t)0x40000000) //< Peripheral base address in the alias region 
#define APB1PERIPH_BASE       PERIPH_BASE
#define RTC_BASE              (APB1PERIPH_BASE + 0x2800)
#define RTC                 ((RTC_TypeDef *) RTC_BASE)
:0x40002800 + 0x9C

Store the context of RTC in BKSRAM (0x40024450~0x40024550)
*/

void Store_RTC_Value(void)
{
	/* Disable the write protection for RTC registers */
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;
	
	uint32_t RTC_Pointer = 0x40002800;
	uint32_t Dst_Addr = 0x40024450; 	
	uint32_t End_RTC_Pointer = 0x4000289C; 
	
	for(;RTC_Pointer<=End_RTC_Pointer;RTC_Pointer+=4, Dst_Addr+=4)
	{
		*(__IO uint32_t*)Dst_Addr = *(__IO uint32_t*)RTC_Pointer;
	}
	  
	/* Enable the write protection for RTC registers */
	RTC->WPR = 0xFF; 
}


/* Store RTC Register information
#define PERIPH_BASE           ((uint32_t)0x40000000) //< Peripheral base address in the alias region 
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000)
#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800)
#define RCC                 ((RCC_TypeDef *) RCC_BASE)
:0x40023800 + 0x84

Store the context of RCC in BKSRAM (0x40024560~0x40024650)
*/

void Store_RCC_Value(void)
{	
	uint32_t RCC_Pointer = 0x40023800;
	uint32_t Dst_Addr = 0x40024560; 	
	uint32_t End_RCC_Pointer = 0x40023884; 
	
	for(;RCC_Pointer<=End_RCC_Pointer;RCC_Pointer+=4, Dst_Addr+=4)
	{
		*(__IO uint32_t*)Dst_Addr = *(__IO uint32_t*)RCC_Pointer;
	}
	 
}

/* Store EXTI Register information
#define PERIPH_BASE           ((uint32_t)0x40000000) //< Peripheral base address in the alias region 
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000)
#define EXTI_BASE             (APB2PERIPH_BASE + 0x3C00)
#define EXTI                ((EXTI_TypeDef *) EXTI_BASE)
:0x40013C00 + 0x14

Store the context of EXTI in BKSRAM (0x40024660~0x40024680)
*/

void Store_EXTI_Value(void)
{	
	uint32_t EXTI_Pointer = 0x40013C00;
	uint32_t Dst_Addr = 0x40024660; 	
	uint32_t End_EXTI_Pointer	= 0x40013C14; 
	
	for(;EXTI_Pointer<=End_EXTI_Pointer;EXTI_Pointer+=4, Dst_Addr+=4)
	{
		*(__IO uint32_t*)Dst_Addr = *(__IO uint32_t*)EXTI_Pointer;
	}
}
/*
// Store NVIC Register information
typedef struct
{
  __IO uint32_t ISER[8];                 //!< Offset: 0x000 (R/W)  Interrupt Set Enable Register           
       uint32_t RESERVED0[24];
  __IO uint32_t ICER[8];                 //!< Offset: 0x080 (R/W)  Interrupt Clear Enable Register         
       uint32_t RSERVED1[24];
  __IO uint32_t ISPR[8];                 //!< Offset: 0x100 (R/W)  Interrupt Set Pending Register          
       uint32_t RESERVED2[24];
  __IO uint32_t ICPR[8];                 //!< Offset: 0x180 (R/W)  Interrupt Clear Pending Register        
       uint32_t RESERVED3[24];
  __IO uint32_t IABR[8];                 //!< Offset: 0x200 (R/W)  Interrupt Active bit Register           
       uint32_t RESERVED4[56];
  __IO uint8_t  IP[240];                 //!< Offset: 0x300 (R/W)  Interrupt Priority Register (8Bit wide)
       uint32_t RESERVED5[644];
  __O  uint32_t STIR;                    //!< Offset: 0xE00 ( /W)  Software Trigger Interrupt Register     
}  NVIC_Type;

#define SCS_BASE            (0xE000E000UL)   
#define NVIC_BASE           (SCS_BASE + 0x0100UL)  
#define NVIC                ((NVIC_Type *) NVIC_BASE)   // NVIC configuration struct   
:0xE000E100UL + 0xE00

Store the context of RTC in BKSRAM (0x40024700~0x40024801)
*/

void Store_NVIC_Value(void)
{	
	uint32_t NVIC_Pointer = 0xE000E100UL;
	uint32_t Dst_Addr = 0x40024700; 	
	//uint32_t End_NVIC_Pointer	= 0xE000EF00; 
	uint32_t End_Reg_Addr = NVIC_Pointer + 0x20;
	
	//__IO uint32_t ISER[8]; Interrupt Set Enable Register
	// Store Interrupt Set Enable Register in BKSram.
	for(;NVIC_Pointer<End_Reg_Addr;NVIC_Pointer+=4, Dst_Addr+=4)
	{
		*(__IO uint32_t*)Dst_Addr = *(__IO uint32_t*)NVIC_Pointer;
	}
	
	// Offset: 0x080 (R/W)  Interrupt Clear Enable Register. 
	NVIC_Pointer=0xE000E180;
	Dst_Addr = 0x40024730;
	End_Reg_Addr = NVIC_Pointer + 0x20;
	
	for(;NVIC_Pointer<End_Reg_Addr;NVIC_Pointer+=4, Dst_Addr+=4)
	{
		*(__IO uint32_t*)Dst_Addr = *(__IO uint32_t*)NVIC_Pointer;
	}
	
	//Offset: 0x100 (R/W)  Interrupt Set Pending Register.
	NVIC_Pointer=0xE000E200UL;
	Dst_Addr = 0x40024760;
	End_Reg_Addr = NVIC_Pointer + 0x20;
	
	for(;NVIC_Pointer<End_Reg_Addr;NVIC_Pointer+=4, Dst_Addr+=4)
	{
		*(__IO uint32_t*)Dst_Addr = *(__IO uint32_t*)NVIC_Pointer;
	}
	
	//Offset: 0x180 (R/W)  Interrupt Clear Pending Register
	NVIC_Pointer=0xE000E280UL;
	Dst_Addr = 0x40024780;
	End_Reg_Addr = NVIC_Pointer + 0x20;
	
	for(;NVIC_Pointer<End_Reg_Addr;NVIC_Pointer+=4, Dst_Addr+=4)
	{
		*(__IO uint32_t*)Dst_Addr = *(__IO uint32_t*)NVIC_Pointer;
	}
	
	// Offset: 0x200 (R/W)  Interrupt Active bit Register
	NVIC_Pointer=0xE000E300UL;
	Dst_Addr = 0x40024800;
	End_Reg_Addr = NVIC_Pointer + 0x20;
	
	for(;NVIC_Pointer<End_Reg_Addr;NVIC_Pointer+=4, Dst_Addr+=4)
	{
		*(__IO uint32_t*)Dst_Addr = *(__IO uint32_t*)NVIC_Pointer;
	}
	
	// Offset: 0x300 (R/W)  Interrupt Priority Register (8Bit wide)
	NVIC_Pointer = 0xE000E400;
	Dst_Addr = 0x40024830;
	End_Reg_Addr += NVIC_Pointer + 0x150;
		
	for(;NVIC_Pointer<End_Reg_Addr;NVIC_Pointer+=4, Dst_Addr+=4)
	{
		*(__IO uint32_t*)Dst_Addr = *(__IO uint32_t*)NVIC_Pointer;
	}
	
	//Offset: 0xE00 ( /W)  Software Trigger Interrupt Register
	NVIC_Pointer = 0xE000EF00;
	Dst_Addr = 0x40024990;
	*(__IO uint32_t*)Dst_Addr = *(__IO uint32_t*)NVIC_Pointer;
}

/*
#define PERIPH_BASE           ((uint32_t)0x40000000) // Peripheral base address in the alias region
#define APB1PERIPH_BASE       PERIPH_BASE
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000)
#define GPIOD_BASE            (AHB1PERIPH_BASE + 0x0C00)
#define GPIOD               ((GPIO_TypeDef *) GPIOD_BASE) //0x40020C00
:0x40020C00 + 0x24

Store the context of EXTI in BKSRAM (0x40024810~0x40024850)
*/

void Store_GPIOD_Value(void)
{	
	uint32_t GPIOD_Pointer = 0x40020C00;
	uint32_t Dst_Addr = 0x40024A00; 	
	uint32_t End_GPIOD_Pointer	=0x40020C24;
	
	for(;GPIOD_Pointer<=End_GPIOD_Pointer;GPIOD_Pointer+=4, Dst_Addr+=4)
	{
		*(__IO uint32_t*)Dst_Addr = *(__IO uint32_t*)GPIOD_Pointer;
	}
	 
}

/*
#define PERIPH_BASE           ((uint32_t)0x40000000) // Peripheral base address in the alias region
#define APB1PERIPH_BASE       PERIPH_BASE
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000)
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000)
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE) //0X40020000
:0x40020C00 + 0x24

Store the context of EXTI in BKSRAM (0x40024850~0x40024900)
*/

void Store_GPIOA_Value(void)
{	
	uint32_t GPIOA_Pointer = 0x40020000;
	uint32_t Dst_Addr = 0x40024A50; 	
	uint32_t End_GPIOA_Pointer	= 0x40020024; 
	
	for(;GPIOA_Pointer<=End_GPIOA_Pointer;GPIOA_Pointer+=4, Dst_Addr+=4)
	{
		*(__IO uint32_t*)Dst_Addr = *(__IO uint32_t*)GPIOA_Pointer;
	}
	 
}

/*
#define PERIPH_BASE           ((uint32_t)0x40000000) //!< Peripheral base address in the alias region                              
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000)
#define SYSCFG_BASE           (APB2PERIPH_BASE + 0x3800)
#define SYSCFG              ((SYSCFG_TypeDef *) SYSCFG_BASE)

0x40013800
*/
void Store_SYSCFG_Value(void)
{	
	uint32_t SYSCFG_Pointer = 0x40013800;
	uint32_t Dst_Addr = 0x40024B00; 	
	uint32_t End_SYSCFG_Pointer	= 0x40013820; 
	
	for(;SYSCFG_Pointer<=End_SYSCFG_Pointer;SYSCFG_Pointer+=4, Dst_Addr+=4)
	{
		*(__IO uint32_t*)Dst_Addr = *(__IO uint32_t*)SYSCFG_Pointer;
	}
	 
}
/*************************************************************************
  * @brief  Enables or disables access to the backup domain (RTC registers, RTC 
  *         backup data registers and backup SRAM).
  * @note   If the HSE divided by 2, 3, ..31 is used as the RTC clock, the 
  *         Backup Domain Access should be kept enabled.
  * @param  NewState: new state of the access to the backup domain.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
 // Alias word address of DBP bit 
 PWR_BASE : 0x40007000
   PERIPH_BASE:0x40000000
PWR_OFFSET 0x7000
 #define PWR_OFFSET               (PWR_BASE - PERIPH_BASE)
#define CR_OFFSET                (PWR_OFFSET + 0x00)
#define DBP_BitNumber            0x08
#define CR_DBP_BB                (PERIPH_BB_BASE + (CR_OFFSET * 32) + (DBP_BitNumber * 4))
  42000000 + E0000 + 20 = 0x420e0020
void PWR_BackupAccessCmd(FunctionalState NewState)
{
  //Check the parameters 
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  *(__IO uint32_t *) CR_DBP_BB = (uint32_t)NewState;
}
*/

void Store_CR_DBP_BB(void)
{	
	uint32_t CR_DBP_BB_Pointer = 0x420e0020;
	uint32_t Dst_Addr = 0x40024B30; 	
	if((*(__IO uint32_t*)CR_DBP_BB_Pointer) == 1){
		*(__IO uint32_t*)Dst_Addr=1;
	}	
	//*(__IO uint32_t*)Dst_Addr = *(__IO uint32_t*)CR_DBP_BB_Pointer;
}
/********************************** 
  * @brief  Enables or disables the Internal Low Speed oscillator (LSI).
  * @note   After enabling the LSI, the application software should wait on 
  *         LSIRDY flag to be set indicating that LSI clock is stable and can
  *         be used to clock the IWDG and/or the RTC.
  * @note   LSI can not be disabled if the IWDG is running.  
  * @param  NewState: new state of the LSI.
  *          This parameter can be: ENABLE or DISABLE.
  * @note   When the LSI is stopped, LSIRDY flag goes low after 6 LSI oscillator
  *         clock cycles. 
  * @retval None
  * void RCC_LSICmd(FunctionalState NewState)

void RCC_LSICmd(FunctionalState NewState)
{
  // Check the parameters 
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  *(__IO uint32_t *) CSR_LSION_BB = (uint32_t)NewState;
  rcc_base:0x40023800
  PERIPH_BASE:0x40000000
  PERIPH_BB_BASE: 0x42000000
}
#define RCC_OFFSET                (RCC_BASE - PERIPH_BASE)
 Alias word address of LSION bit 
#define CSR_OFFSET                (RCC_OFFSET + 0x74)
#define LSION_BitNumber           0x00
#define CSR_LSION_BB              (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (LSION_BitNumber * 4))
0x23800+0x74
0x20023874(0-bit)
  *************************************************************************/
  
void Store_CSR_LSION_BB(void)
{	
	uint32_t CSR_LSION_BB_Pointer = 0x42470e80;
	uint32_t Dst_Addr = 0x40024B40; 	
	if(*(__IO uint32_t*)CSR_LSION_BB_Pointer == 1){
		*(__IO uint32_t*)Dst_Addr=1;
	}
	//*(__IO uint32_t*)Dst_Addr = *(__IO uint32_t*)CSR_LSION_BB_Pointer;
}

/**
  * @brief  Enables or disables the RTC clock.
  * @note   This function must be used only after the RTC clock source was selected
  *         using the RCC_RTCCLKConfig function.
  * @param  NewState: new state of the RTC clock. This parameter can be: ENABLE or DISABLE.
  * @retval None

void RCC_RTCCLKCmd(FunctionalState NewState)
{
  // Check the parameters
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  *(__IO uint32_t *) BDCR_RTCEN_BB = (uint32_t)NewState;
}

 --- BDCR Register ---
Alias word address of RTCEN bit 
#define BDCR_OFFSET               (RCC_OFFSET + 0x70)
#define RTCEN_BitNumber           0x0F
#define BDCR_RTCEN_BB             (PERIPH_BB_BASE + (BDCR_OFFSET * 32) + (RTCEN_BitNumber * 4))
0x20023870 (F)

470E00
*/

void Store_BDCR_RTCEN_BB(void)
{	
	uint32_t BDCR_RTCEN_BB_Pointer = 0x42470E3C;
	uint32_t Dst_Addr = 0x40024B50; 	
	if(*(__IO uint32_t*)BDCR_RTCEN_BB_Pointer == 1){
		*(__IO uint32_t*)Dst_Addr=1;
	}	
	//(__IO uint32_t*)Dst_Addr = *(__IO uint32_t*)BDCR_RTCEN_BB_Pointer;
	//RTC_WaitForSynchro();
}

