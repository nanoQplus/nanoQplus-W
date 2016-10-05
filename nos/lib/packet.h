// -*- c-file-style:"bsd"; c-basic-offset:4; indent-tabs-mode:nil; -*-
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
 * @brief PACKET vector for NanoQplus IPv6 Suite.
 * @author Jongsoo Jeong (ETRI)
 * @date 2011. 9. 21.
 */

#ifndef PACKET_H_
#define PACKET_H_

#include <string.h>
#include "nos_common.h"

/// Packet container
struct _pkt_container
{
    struct _pkt_container *next;
    void *buf;                      // pointer of buffer
    UINT16 buf_len;                 // buffer length
    UINT8 protocol;                 // same values as next header field in IPv6 header

    BOOL alloc:1;
    BOOL buf_alloc:1;
    BOOL compressed:1;
};
typedef struct _pkt_container PACKET;

enum { MAX_PACKET_CHAIN_LENGTH = 10 };

#define NOS_PACKET_INIT(p)                      \
    do {                                        \
        (p)->next = NULL;                       \
        (p)->buf = NULL;                        \
        (p)->buf_len = 0;                       \
        (p)->protocol = 0;                      \
        (p)->alloc = 0;                         \
        (p)->buf_alloc = 0;                     \
        (p)->compressed = 0;                    \
    } while (0)

PACKET *nos_packet_put(PACKET *prev_pkt, PACKET *new_pkt,
        void *buf, UINT16 buf_len, BOOL buf_alloc, UINT8 protocol);

UINT16 nos_packet_length(const PACKET *p);

void nos_packet_kill(PACKET *head);

INT32 nos_packet_get_offset(PACKET *head, void *ptr);

BOOL nos_packet_set_buffer(PACKET *p, void *buf, UINT16 len, BOOL buf_alloc, UINT8 protocol);

BOOL nos_packet_align(PACKET *p);

void *nos_packet_buffer_alloc(UINT16 size, BOOL zero);

void nos_packet_buffer_free(void *p);

UINT16 nos_packet_dump(UINT8 *dst, const PACKET *src);

extern UINT8 pkt_in_stack;

#endif /* PACKET_H_ */
