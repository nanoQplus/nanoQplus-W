/*
 * Copyright (c) 2006-2012
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
 * @author Jongsoo Jeong (ETRI)
 * @date 2009. 6. 19.
 */

/*
 * $LastChangedDate: 2012-10-11 11:12:57 +0900 (Thu, 11 Oct 2012) $
 * $Id: fota.c 993 2012-10-11 02:12:57Z jsjeong $
 */

#include "fota.h"

#ifdef FOTA_M

#include "arch.h"
#include "uart.h"

#ifdef ATMEGA128
#include <avr/eeprom.h>
#endif

// Port number
#define FOTA_PORT    0xF0
#define FOTA_STARTCODE   0xAA

// State Machine
#define STATE_READY    1
#define STATE_CMD    2
#define STATE_LEN    3
#define STATE_DATA    4
#define STATE_FCS    5
#define STATE_DONE    6

// Rx State
#define RECV_SUCCESS_NORMAL  1
#define RECV_SUCCESS_PERFECT 2
#define RECV_FAILURE_CHECKSUM 3
#define RECV_FAILURE_SEQUENCE 4
#define RECV_FAILURE_CRC  5
#define RECV_TARTGET_NO_ANSWER 6
#define REEV_FAILURE_UPDATE  7

// Command Type
#define START_PACKET   1
#define DATA_PACKET    2
#define ACK_PACKET    3
#define TARGET_RUN_PACKET  4

// Common Packet Field
#define FIELD_START_CODE  0
#define FIELD_CMD_TYPE   1
#define FIELD_DATA_LENGTH  2

// Ack Packet Field
#define FIELD_SEQ_HIGH   3
#define FIELD_SEQ_LOW   4
#define FIELD_ACK_RESULT  5
#define FIELD_ACK_CS   6

#define ACK_PACKET_LENGTH  7

// Start Packet Fields
#define FIELD_START_DESTADDR  3 // 2 byte
#define FIELD_START_TOTAL_DATA_SIZE 5 // 4 byte
#define FIELD_START_TOTAL_PKT_CNT 9 // 2 byte
#define FIELD_START_CRC    11 // 2 byte

// EEPROM indices where packet fields are stored at.
#define FOTA_EEPROM_INDEX   5
#define FOTA_EEPROM_LENGTH   17

#define IDX_CHANNEL    0 // 1 byte
#define IDX_PAN_ID    1 // 2 byte
#define IDX_NODE_ID    3 // 2 byte

#define IDX_BOOT_SECTION   5 // 1 byte 
#define IDX_HOST_ID    6 // 2 byte
#define IDX_ROUTER_ID    8 // 2 byte
#define IDX_TOTAL_DATA_SIZE  10 // 4 byte
#define IDX_TOTAL_PACKET_COUNT 14 // 2 byte
#define IDX_CRC     16 // 2 byte
//#define IDX_BROADCASTING   18 // 1 byte

void fota_check(UINT8 rf_ch, UINT16 pan_id, UINT16 my_id, 
        UINT16 host_id, UINT16 router_id, UINT8 *packet) {

    //UINT8 buf[FOTA_EEPROM_LENGTH];
    UINT32 total_size;
    UINT16 pkt_cnt;
    UINT16 crc;

    // Filter packets which are not for me.
    if (packet[FIELD_START_CODE] != FOTA_STARTCODE) return;
    if (packet[FIELD_CMD_TYPE] != START_PACKET) return;

    total_size = ((UINT32)packet[FIELD_START_TOTAL_DATA_SIZE] << 24)
        | ((UINT32)packet[FIELD_START_TOTAL_DATA_SIZE + 1] << 16)
        | ((UINT32)packet[FIELD_START_TOTAL_DATA_SIZE + 2] << 8)
        | ((UINT32)packet[FIELD_START_TOTAL_DATA_SIZE + 3]);
    pkt_cnt = ((UINT16)packet[FIELD_START_TOTAL_PKT_CNT] << 8)
        | ((UINT16)packet[FIELD_START_TOTAL_PKT_CNT + 1]);
    crc = ((UINT16)packet[FIELD_START_CRC] << 8)
        | ((UINT16)packet[FIELD_START_CRC + 1]);

#ifdef ATMEGA128
    eeprom_write_byte((void *)IDX_CHANNEL, rf_ch);
    eeprom_write_word((void *)IDX_PAN_ID, pan_id);
    eeprom_write_word((void *)IDX_NODE_ID, my_id);

    eeprom_write_byte((void *)IDX_BOOT_SECTION, 1);
    eeprom_write_word((void *)IDX_HOST_ID, host_id);
    eeprom_write_word((void *)IDX_ROUTER_ID, router_id);
    eeprom_write_dword((void *)IDX_TOTAL_DATA_SIZE, total_size);
    eeprom_write_word((void *)IDX_TOTAL_PACKET_COUNT, pkt_cnt);
    eeprom_write_word((void *)IDX_CRC, crc);
#endif
    NOS_RESET();
}

#endif // ~FOTA_M
