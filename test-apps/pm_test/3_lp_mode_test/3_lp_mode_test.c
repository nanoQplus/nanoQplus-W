//========================================================================
// File		: 3_LP_MODE_TEST.c
// Author	: hj.park@etri.re.kr
// Date		: 2016.10.02
// Description : Low Power Mode Test
//========================================================================
// Copyright 2016-2025, ETRI
//========================================================================

#include "nos.h"
#include "kconf.h"
#include "spi.h"
#include "alarm.h"
#include "bkpsram.h"
#include "lowpower.h"

#ifdef CFD_M
#include "fd_config.h"
#include "fd_if.h"
#include "snapshot.h"

static snapshot_t mysnapshot ;
#endif

#include "3_lp_mode_test.h"

typedef union _myspiinfo {
	unsigned long   data;
	uint8_t d[4];
} spi32comm;

extern __IO uint8_t UserButtonStatus;


// declarations
void my_task1(void *);
UINT32 nos_spi_txrx_32(uint8_t, UINT32);
void my_busy_wait(UINT32);
UINT32 mysystem_save(UINT32);
UINT32 mysystem_save_standby(UINT32);




void button_cb(void *args) {
	uint32_t id;
	UINT32 sig;
	id = *((uint32_t *)args);
	uart_printf("\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\nButton (ID: %ld) is pressed! \r\n\r\n", id);
	
	sig = 0;
	bkpsram_write((UINT32)&sig, BKPSRAM_START_ADDR, sizeof(UINT32));
	
#ifdef CFD_M
	if ((id = fd_erase_all(0, FALSE))) {
		uart_printf("button_cb:: Error(%d) occurred!! Halt!!\r\n", (int)id);
		for (;;) {;}
	} // end if
#endif // CFD_M
	
	system_abort(0);
	
} // end func

#ifdef CFD_M
void init_mysnapshot() {
// FIXME
	mysnapshot.num_cpu_regs = 16;
	mysnapshot.num_io_regs = 32;
	//mysnapshot.num_mem_descs = 1;

#if 0
	mysnapshot.mem_desc[0].start_addr = &my_struct;
	mysnapshot.mem_desc[0].num_bytes = sizeof(my_struct);
#else
	mysnapshot.mem_desc.start_addr = (void *)0x20000000;
	mysnapshot.mem_desc.num_bytes = 0x20000;
#endif

} // end func
#endif // CFD_M






// 32bit transfer along spi
UINT32 nos_spi_txrx_32(uint8_t channel, UINT32 d) {
        int i;
        spi32comm myspi;

        myspi.data = d;

        for (i = 0; i < 4; i++) {
                nos_spi_txrx(channel, myspi.d[i]);
        } // end for
        return 0; // currently, nos_spi_txrx_32 returns dummy value
} // end func

// for scenario2, using RTC, sleep shortly and busywait
void short_span_works(UINT32 n, UINT32 working_time, UINT32 sleep_span) {
	int i;
	
	for (i = 0; i < n; i++) {
		RTC_Configuration(sleep_span); 
		RTC_WakeUpCmd(ENABLE);
		RTC_ClearFlag(RTC_FLAG_WUTF);	/* Clear WakeUp (WUTF) pending flag */
		my_busy_wait(working_time); // ms

		//set_power_mode(PWR_SLEEP_MODE, FREQ_STAYING, PERI_STAYING); //__WFI();		
		__WFI();
	} // end for		
} // end func


// for scenario4, using RTC, imitate working sensors to sleep, and busywait
void sleep_busy_wait(UINT32 working_time, UINT32 sleep_span, unsigned long power_off_time){

		//limit_lp_mode = SLEEP_MODE;
		RTC_Configuration(sleep_span);
		RTC_WakeUpCmd(ENABLE);
		RTC_ClearFlag(RTC_FLAG_WUTF);	/* Clear WakeUp (WUTF) pending flag */		
		__WFI();
		my_busy_wait(working_time);

		//mysystem_save(power_off_time);
		//limit_lp_mode = STOP_MODE;
		/*
		if(dnode_delta < 150){
			limit_lp_mode = SLEEP_MODE;
		} else {
			limit_lp_mode = POWEROFF_MODE;
		} // end if
		*/
} // end func

// for scenario4, using RTC, busywait and imitate working GPS to sleep
void busy_sleep_busy_wait(UINT32 working_time, UINT32 sleep_span) {
		my_busy_wait(working_time);
		RTC_Configuration(sleep_span); 
		RTC_WakeUpCmd(ENABLE);
		RTC_ClearFlag(RTC_FLAG_WUTF);	/* Clear WakeUp (WUTF) pending flag */		
		__WFI();
		my_busy_wait(working_time);		
} // end func

// 1ms busy wait
void my_busy_wait(UINT32 m) { 
	UINT32 i, n, a;
	
	a = 0;
	n = m * 6468;
	
	for (i = 0; i < n; i++) {
		a++;
	} // end for
} // end func

void my_idle_wait(UINT32 m) { 
	nos_delay_ms(m);
} // end func


// Thread body definition & alarm handler
void my_task1_handler(UINT32 dummy) {
	UINT32 task_no;
	
	task_no = (UINT32)dummy;
	thread_activate(my_task[task_no].tid);
} // end func

void my_task1(void *dummy) {
	UINT32 task_no;
	UINT32 i;
	
	task_no = (UINT32)dummy;
	
	uart_printf("=-=-=-=-=-=-=-=-=-=-=- my_task1 : boot count = %d\r\n", my_struct.bootcount);

	my_struct.bootcount++;

	if (my_task[task_no].flag) {
		my_idle_wait(my_task[task_no].busywait);
	} else {
		my_busy_wait(my_task[task_no].busywait);
	} // end if

} // end func



void myspi_init() {
	nos_spi_init(2, NULL);
} // end func



void mybutton_init() {
	int i;
	
	for (i = 0; i < BUTTON_NUM; i++) {
		nos_button_set_callback(i, button_cb);
	} // end for
} // end func



void app_init(void) {
	UINT32 tid1;
	UINT32 alid1;
	UINT32 sig;
	UINT32 resttime;
	
	
	UINT32 i, err;
	
	mybutton_init();
	myspi_init();

	bkpsram_init();
	
	__cpu_context_backup_point = (UINT32)&mysnapshot.cpu_reg[0];
	register_callback_save_standby(mysystem_save_standby);
	register_callback_save_pwroff(mysystem_save);
	
	/* 1. try to full cold restart */
	if ((err = ssl_open(MYCHIP_ID, MYCHIP_PART))) {
		uart_printf("app_init:: ssl_open() failed. try to half cold restart.\r\n");
		goto FAIL1;
	} // end if
	
	init_mysnapshot();
	
	err = ssl_restore(&mysnapshot);
	
	switch(err) {
		case SSL_OPEN_FAIL:
		case SSL_DEVICE_NOT_OPEN:
		case SSL_ERROR:
			uart_printf("app_init:: ssl_restore failure. try to half cold restart.\r\n");
			ssl_close();
			break;
		case SSL_NO_VALID_SNAPSHOT:
			uart_printf("app_init:: ssl_restore detects same snapshot. try to half cold restart.\r\n");
			ssl_close();
			break;
		case SSL_SUCCESS:
			uart_printf("app_init:: ssl_restore returns SSL_SUCCESS. try to half cold restart.\r\n");
			ssl_close();
			break;
		default:
			uart_printf("app_init:: ssl_restore returns an illegal err code. try to half cold restart.\r\n");
			ssl_close();
			break;
	} // end switch
	
FAIL1:

	/* 2. try to half cold restart */
	// read signature
	bkpsram_read(BKPSRAM_START_ADDR, (UINT32)&sig, sizeof(UINT32));
	
	if (sig == 0x55aa55aa) { // restore mode

		uart_printf("app_init:: >>>>>>>>>>>>>>>>>>>>>> entering half cold restart mode.\r\n");
		
		// reset signature
		sig = 0;
		bkpsram_write((UINT32)&sig, BKPSRAM_START_ADDR, sizeof(UINT32));
		
		// read data
		bkpsram_read(BKPSRAM_START_ADDR + 4, (UINT32)(&(my_struct.bootcount)), sizeof(UINT32));
		bkpsram_read(BKPSRAM_START_ADDR + 8, (UINT32)&resttime, sizeof(UINT32));
		//bkpsram_read(BKPSRAM_START_ADDR + 12, (UINT32)&__saved_alid, sizeof(UINT32));
		bkpsram_read(BKPSRAM_START_ADDR + 16, (UINT32)&my_task, sizeof(my_task));

		//uart_printf("app_init:: resttime = %d\r\n", resttime);

	} else {	// reset mode
		uart_printf("app_init:: finally, entering reset mode!!\r\n");

	} // endif

	for (i = 0; i < TASK_NUM; i++) {

		uart_printf("app_init:: task %d (%d, %d, %d, %d)\r\n", i, 
			my_task[i].busywait,
			my_task[i].period,
			my_task[i].offset,
			my_task[i].flag);

	
		thread_create(my_task1, (void *)i, 9, PRIORITY_NORMAL, FIFO, &tid1);
		my_task[i].tid = tid1;
	} // end for
	
	// first offset 0
	for (i = 0; i < TASK_NUM; i++) {
		
		if (my_task[i].offset == 0) {
		
			_alarm_create(my_task1_handler, i,
				1,
			//MSEC(my_task[i].offset),
				MSEC(my_task[i].period),
				&alid1, MSEC(my_task[i].busywait));
			
			my_task[i].alid = alid1;
			
			alarm_start(my_task[i].alid);
		} // end if
	} // end for

	// sedondary, offset is not 0
	for (i = 0; i < TASK_NUM; i++) {
		
		if (my_task[i].offset > 0) {
		
			_alarm_create(my_task1_handler, i,
				MSEC(my_task[i].offset),
				MSEC(my_task[i].period),
				&alid1, MSEC(my_task[i].busywait));
			
			my_task[i].alid = alid1;
			
			alarm_start(my_task[i].alid);
		} // end if
	} // end for

	
} // end func


extern DQUEUE tick_q;

UINT32 mysystem_save_standby(UINT32 resttime) {
	
	UINT32 sig, i;
	DNODE *dnode = tick_q.head;
	UINT32 alarm;
	UINT32 deltamsec;
	UINT32 firstdnode = 1;
	
	//uart_printf("mysystem_save_standby:: resttime = %d\r\n", resttime);
	
	for (; dnode ; dnode = dnode->next) {
		alarm = (UINT32)dnode - (0x10);
		deltamsec = dnode->delta * 10;
		// FIXME : optimize
		for (i = 0; i < TASK_NUM; i++) {
			
			if (my_task[i].alid == ((ALARM *)alarm)->alid) {
				
				if (firstdnode) {
					// first node, wakeup, invoke immediately
					my_task[i].offset = 0;
					firstdnode = 0;
				} else {
					// still remain time
					my_task[i].offset = deltamsec;
				} // end if
				/*
				uart_printf("mysystem_save_standby:: found it. my_task[%d].offset = %d\r\n", i, my_task[i].offset);
				uart_printf("mysystem_save_standby>>>>> deltamsec = %d\r\n", 
					deltamsec);
				*/	
				break;
			} // end if
		} // end for
		
	} // end for
	/*
	for (i = 0; i < TASK_NUM; i++) {
		uart_printf("mysystem_save_standby:: task %d (%d, %d, %d, %d)\r\n", i, 
			my_task[i].busywait,
			my_task[i].period,
			my_task[i].offset,
			my_task[i].flag);
	} // end for
	*/
	bkpsram_init();
	
	// write signature
	sig = 0x55aa55aa;
	bkpsram_write((UINT32)&sig, BKPSRAM_START_ADDR, sizeof(UINT32));
	
	// write data
	bkpsram_write((UINT32)(&(my_struct.bootcount)), BKPSRAM_START_ADDR + 4, sizeof(UINT32));
	
	bkpsram_write((UINT32)&resttime, BKPSRAM_START_ADDR + 8, sizeof(UINT32));
	//bkpsram_write((UINT32)&__saved_alid, BKPSRAM_START_ADDR + 12, sizeof(UINT32));
	
	bkpsram_write((UINT32)&my_task, BKPSRAM_START_ADDR + 16, sizeof(my_task));

	uart_printf("mysystem_save_save:: the system is going to standby. %d msec.\r\n", resttime);	
	
	return 0;
	
} // end func

UINT32 mysystem_save(unsigned long resttime) {
	
#ifdef CFD_M
	uint32_t err;
	
#if (MYCHIP_ID == 1)
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
		FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
#endif // MYCHIP_ID


	if ((err = ssl_open(MYCHIP_ID, MYCHIP_PART))) {
		uart_printf("mysystem_save:: ssl_open fail. return\r\n");
		return -1;
	} // endif
	
	init_mysnapshot();
	
	if ((err = ssl_save(&mysnapshot))) {
		printf("mysystem_save:: ssl_save fail. return.\r\n");
		ssl_close();
		return -1;
	} // endif
	
	uart_printf("mysystem_save:: ssl_save success.\r\n");

	ssl_close();
	
#endif // CFD_M

	uart_printf("mysystem_save:: the system is going to poweroff. %d msec.\r\n", resttime);	
	
	nos_spi_txrx_32(2, resttime);
	
	for (;;) {;}
	
	return 0;
	
} // end func