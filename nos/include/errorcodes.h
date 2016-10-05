/*
 * Copyright (C) 2006-2015  Electronics and Telecommunications Research Institute (ETRI) 
 *
 * This file is subject to the terms and conditions of the GNU General Public License V3
 * See the file LICENSE in the top level directory for more details.
 *
 * This file is part of the NanoQplus3 operating system.
 */


/**
 * @file errorcodes.h
 * @brief Error codes
 * @author Jongsoo Jeong (ETRI)
 * @date 2011. 6. 20.
 * @ingroup nos_include
 * @copyright GNU General Public License v3
 */


#ifndef ERRORCODES_H
#define ERRORCODES_H
#include "nos_common.h"

enum {
    SUCCESS_BYPASS = 4,
    SUCCESS_OPERATION_RESERVED = 3,
    SUCCESS_6SIXLO_REASSEMBLY_COMPLETE = 2,
    SUCCESS_NOTHING_HAPPENED = 1,
    ERROR_SUCCESS = 0,
    ERROR_FAIL = -1,
    ERROR_GENERAL_FAILURE = -1, //TODO will be replaced to ERROR_FAIL.
    ERROR_NOT_ENOUGH_MEMORY = -2,
    ERROR_INVALID_ARGS = -3,
    ERROR_MORE_ARGS_REQUIRED = -4,
    ERROR_NOT_SUPPORTED = -5,
    ERROR_NOT_FOUND = -6,
    ERROR_BUSY = -7,

    //802.15.4 Communication errors: -31 ~ -40
    ERROR_802154_FRAME_TOO_BIG = -31,
    ERROR_802154_SECURING_FAIL = -32,
    ERROR_802154_UNSECURING_FAIL = -33,
    ERROR_802154_INVALID_SEC_FRAME = -34,
    ERROR_802154_INVALID_KEY_IDX = -35,
    ERROR_802154_TX_FAIL = -36,

    //6LoWPAN errors: -41 ~ -50
    ERROR_6SIXLO_INVALID_FRAME = -41,
    ERROR_6SIXLO_SEND_FAILURE = -42,
    ERROR_6SIXLO_COMPRESSION_FAIL = -43,
    ERROR_6SIXLO_DECOMPRESSION_FAIL = -44,
    ERROR_6SIXLO_INVALID_CONTEXT = -45,

    //IP Communication errors: -51 ~ -60
    ERROR_IP6_TIME_EXCEEDED = -52,
    ERROR_IP6_INVALID_CHECKSUM = -53,
    ERROR_IP6_INVALID_PACKET = -54,
    ERROR_IP6_ADDR_DUPLICATE = -55,
    ERROR_IP6_UNKNOWN_SCOPE = -56,
    ERROR_IP6_NOT_READY = -57,
    ERROR_IP6_LINK_FAIL = -58,
    ERROR_ICMP6_INVALID_MESSAGE = -61,
    ERROR_COAP_REQUEST_TIMEOUT = -62,
    ERROR_RPL_IS_NOT_WORKING = -63,
};

typedef INT8 ERROR_T;

#endif
