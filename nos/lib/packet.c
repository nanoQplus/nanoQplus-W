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

#include "packet.h"

#include <string.h>
#include "heap.h"
#include "arch.h"

#ifdef PACKET_DEBUG
#include <stdio.h>
#endif

BOOL nos_packet_set_buffer(PACKET *p, void *buf, UINT16 len, BOOL buf_alloc, UINT8 protocol)
{
    if (!p)
        return FALSE;

    if (buf)
    {
        p->buf = buf;
        p->buf_alloc = buf_alloc;
    }
    else
    {
        p->buf = nos_packet_buffer_alloc(len, TRUE);
        p->buf_alloc = TRUE;
    }

    if (!p->buf)
    {
        p->buf_alloc = FALSE;
        return FALSE;
    }

    p->buf_len = len;
    p->protocol = protocol;

    return TRUE;
}

void *nos_packet_buffer_alloc(UINT16 size, BOOL zero)
{
    void *ret;
    ret = (void *) nos_malloc(size);

#ifdef PACKET_DEBUG
    printf("%s()-size:%u, mem:0x%x\n\r", __FUNCTION__, size, (MEMADDR_T) ret);
#endif

    if (ret && zero)
        memset(ret, 0, size);

    return ret;
}

void nos_packet_buffer_free(void *p)
{
#ifdef PACKET_DEBUG
    printf("%s()-mem:0x%x\n\r", __FUNCTION__, (MEMADDR_T) p);
#endif
    nos_free(p);
}

PACKET *nos_packet_put(PACKET *prev_pkt, PACKET *new_pkt,
        void *buf, UINT16 buf_len, BOOL buf_alloc, UINT8 protocol)
{
    if (!prev_pkt || !new_pkt)
        return NULL;

    if (nos_packet_set_buffer(new_pkt, buf, buf_len, buf_alloc, protocol))
    {
        new_pkt->next = prev_pkt->next;
        prev_pkt->next = new_pkt;
        return new_pkt;
    }
    else
    {
        return NULL;
    }
}

UINT16 nos_packet_length(const PACKET *p)
{
    UINT16 total_len = 0;
    UINT8 pcnt = 0;
    while (p && p->buf && pcnt < MAX_PACKET_CHAIN_LENGTH)
    {
        total_len += p->buf_len;
        p = p->next;
        pcnt++;
    }

    if (pcnt >= MAX_PACKET_CHAIN_LENGTH)
    {
#ifdef PACKET_DEBUG
        printf("%s()-pkt chain too long\n\r", __FUNCTION__);
#endif
        return 0;
    }
    return total_len;
}

void nos_packet_kill(PACKET *head)
{
    PACKET *p;
    UINT8 pcnt = 0;

    if (!head)
        return;

    p = head;

    while (p && pcnt < MAX_PACKET_CHAIN_LENGTH)
    {
        PACKET *next = p->next;
        if (p->buf && p->buf_alloc)
        {
            nos_packet_buffer_free(p->buf);
            p->buf = NULL;
            p->buf_len = 0;
            p->buf_alloc = FALSE;
        }

        if (p->alloc)
        {
            nos_packet_buffer_free(p);
        }
        p = next;
        pcnt++;
    }
    if (pcnt >= MAX_PACKET_CHAIN_LENGTH)
    {
#ifdef PACKET_DEBUG
        printf("%s()-pkt chain too long\n\r", __FUNCTION__);
#endif
    }
}

INT32 nos_packet_get_offset(PACKET *head, void *ptr)
{
    PACKET *p;
    UINT8 pcnt = 0;
    UINT16 offset = 0;

    if (!head || !ptr)
        return -1;

    p = head;
    while (p && pcnt < MAX_PACKET_CHAIN_LENGTH)
    {
        if (p->buf_len > 0 &&
            ptr >= p->buf &&
            ptr < (void *) ((UINT8 *) p->buf + p->buf_len))
        {
            return (offset + ((MEMADDR_T) ptr - (MEMADDR_T) p->buf));
        }

        offset += p->buf_len;
        p = p->next;
        pcnt++;
    }

    return -1;
}

BOOL nos_packet_align(PACKET *p)
{
    MEMADDR_T bob; // begin of buffer

    if (!p || !p->buf)
        return FALSE;

    bob = (MEMADDR_T) p->buf;
    if (bob % ALIGN_MOD > 0)
    {
        void *tmp = p->buf;
        p->buf = nos_packet_buffer_alloc(p->buf_len, FALSE);
        if (p->buf)
        {
            memcpy(p->buf, tmp, p->buf_len);
            if (p->buf_alloc)
                nos_packet_buffer_free(tmp);
            else
                p->buf_alloc = TRUE;

            return TRUE;
        }
        else
        {
            p->buf = tmp;
            return FALSE;
        }
    }
    else
        return TRUE;
}

UINT16 nos_packet_dump(UINT8 *dst, const PACKET *src)
{
    UINT16 total_len = nos_packet_length(src);
    const PACKET *p;

    if (total_len == 0)
    {
#ifdef PACKET_DEBUG
        printf("%s()-invalid PACKET length?\n\r", __FUNCTION__);
#endif
        return 0;
    }

    p = src;

    while (p)
    {
        memcpy(dst, p->buf, p->buf_len);
        dst += p->buf_len;
        p = p->next;
    }

    return total_len;
}
