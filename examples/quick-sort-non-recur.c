#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

#include "common.h"

static uint16_t values[MAX_LEN];

static void list_qsort_non_recursive(struct list_head *head)
{
    /* Question: what's the usage of pointers of array begin[] and end[]?
     *
     * They are used like a stack, to mark the to-be-sorted region in the
     * list; each run in the sorting will use the same index to the two arrays.
     */

    struct listitem *begin[MAX_LEN], *end[MAX_LEN], *L, *R;
    struct listitem pivot;
    int i = 0;

    if (list_empty(head) || list_is_singular(head))
        return;

    begin[0] = list_first_entry(head, struct listitem, list);
    end[0] = list_last_entry(head, struct listitem, list);

    while (i >= 0) {
        L = begin[i];
        R = end[i];

        // only do the following process on different numbers and
        // skip head, it's used as pivot
        if (L != R && &begin[i]->list != head) {
            // pivot is the actual address of L
            pivot = *begin[i];
            if (i == MAX_LEN - 1) {
                assert(-1);
                return;
            }

            while (L != R) {
                while (R->i >= pivot.i && L != R)
                    R = list_entry(R->list.prev, struct listitem, list);
                if (L != R) {
                    L->i = R->i;
                    L = list_entry(L->list.next, struct listitem, list);
                }

                while (L->i <= pivot.i && L != R)
                    L = list_entry(L->list.next, struct listitem, list);
                if (L != R) {
                    R->i = L->i;
                    R = list_entry(R->list.prev, struct listitem, list);
                }
            }

            /* Copy value to the item pointed by L.
             *
             * Forward `begin` entry to the item next to pivot, the pivot is on
             * the position which the list is sorted.
             *
             * The `end` entry is unchanged for sorting on the next run.
             *
             * Backward `end` entry to pivot for marking the last item of the
             * sub-list which has values smaller then the pivot
             */

            L->i = pivot.i;
            begin[i + 1] = list_entry(L->list.next, struct listitem, list);
            end[i + 1] = end[i];
            end[i++] = L;
        } else
            i--;  // the sub-list is sorted, move to the sub-list which has
                  // values smaller then the pivot; like operation of
                  // pop on stack
    }
}

int main(void)
{
    struct list_head testlist;
    struct listitem *item, *is = NULL;
    size_t i;

    random_shuffle_array(values, (uint16_t) ARRAY_SIZE(values));

    INIT_LIST_HEAD(&testlist);

    assert(list_empty(&testlist));

    printf("\n===Value of list before doing qsort===\n\n");
    for (i = 0; i < ARRAY_SIZE(values); i++) {
        item = (struct listitem *) malloc(sizeof(*item));
        assert(item);
        item->i = values[i];
        list_add_tail(&item->list, &testlist);
        printf("[%0ld]: %0d", i, item->i);
        if (i < ARRAY_SIZE(values) - 1)
            printf("->");
        else
            printf("\n\n");
    }

    assert(!list_empty(&testlist));

    qsort(values, ARRAY_SIZE(values), sizeof(values[0]), cmpint);
    list_qsort_non_recursive(&testlist);

    i = 0;

    printf("===Result of list_qsort_non_recursive===\n\n");
    list_for_each_entry_safe (item, is, &testlist, list) {
        assert(item->i == values[i]);
        printf("%0d", item->i);
        if (item != list_last_entry(&testlist, struct listitem, list))
            printf("->");
        else
            printf("\n");
        list_del(&item->list);
        free(item);
        i++;
    }

    assert(i == ARRAY_SIZE(values));
    assert(list_empty(&testlist));

    return 0;
}
