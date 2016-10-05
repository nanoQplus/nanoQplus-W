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
 * $Id: linked_list.c 993 2012-10-11 02:12:57Z jsjeong $
 */

#include "linked_list.h"
#include "nos_common.h"

void ll_add_element(struct linked_list *list, struct list_head *h) {
    if (!list || !h) return;

    if (ll_has_element(list, h)) return;

    h->next = list->head;
    list->head = h;
    list->length++;
}

BOOL ll_remove_element(struct linked_list *list, struct list_head *h) {
    struct list_head *e, *prev = NULL;
    if (!list || !h) return 0;

    if (list->length == 0) return 0;
    e = list->head;
    while(e) {
        if (e == h) {
            if (prev) prev->next = e->next;
            else list->head = e->next;
            list->length--;
            INIT_LISTHEAD(h);
            return 1;
        }
        prev = e;
        e = NEXT_ELEMENT(e);
    }
    return 0;
}

BOOL ll_has_element(struct linked_list *list, struct list_head *h) {
    struct list_head *e;

    if (!list || !h || (list->length == 0)) return 0;

    e = list->head;

    while(e) {
        if (e == h) break;
        e = NEXT_ELEMENT(e);
    }
    if (e) return 1;
    else return 0;
}
