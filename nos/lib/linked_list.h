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
 * @date 2010. 2. 1.
 */

/*
 * $LastChangedDate: 2012-10-11 11:12:57 +0900 (Thu, 11 Oct 2012) $
 * $Id: linked_list.h 993 2012-10-11 02:12:57Z jsjeong $
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "kconf.h"
#include "nos_common.h"

struct list_head {
    struct list_head *next;
};

struct linked_list {
    struct list_head *head;
    UINT8 length;
};

#define INIT_LIST(p_list)  \
    do {      \
        (p_list)->head = NULL; \
        (p_list)->length = 0; \
    } while(0)

#define INIT_LISTHEAD(p_element)      \
    do {            \
        ((struct list_head *)(p_element))->next = NULL; \
    } while(0)

void ll_add_element(struct linked_list *list, struct list_head *h);
BOOL ll_remove_element(struct linked_list *list, struct list_head *h);
BOOL ll_has_element(struct linked_list *list, struct list_head *h);

#define GET_ITEM_FROM_ELEMENT(p_element) \
    ((void *)(((struct list_head *)p_element) + 1))
#define NEXT_ELEMENT(p_element) \
    (((struct list_head *)p_element)->next)
#define PREV_ELEMENT(p_element) \
    (((struct list_head *)p_element)->prev)
#define COPY_ELEMENT(p_dst_element,p_src_element,element_sz)  \
    memcpy((void *)(((struct list_head *)(p_dst_element)) + 1),  \
            (void *)(((struct list_head *)(p_src_element)) + 1), \
            element_sz - sizeof(struct list_head))

#endif // ~LINKED_LIST_H
