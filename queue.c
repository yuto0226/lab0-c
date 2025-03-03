#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(*head));
    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);

    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *entry = NULL, *safe = NULL;
    list_for_each_entry_safe (entry, safe, head, list) {
        q_release_element(entry);
    }

    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new_ele = malloc(sizeof(*new_ele));
    if (!new_ele)
        return false;

    new_ele->value = strdup(s);
    if (!new_ele->value) {
        free(new_ele);
        return false;
    }

    list_add(&new_ele->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    return q_insert_head(head->prev, s);
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *first_ele = list_first_entry(head, element_t, list);

    list_del(&first_ele->list);

    if (sp && first_ele->value) {
        strncpy(sp, first_ele->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return first_ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    return q_remove_head(head->prev->prev, sp, bufsize);
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *curr;

    list_for_each (curr, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    struct list_head *slow = head->next, *fast = slow->next;
    while (fast != head && fast->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }

    element_t *mid = list_entry(slow, element_t, list);
    list_del(&mid->list);
    q_release_element(mid);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;

    bool isdup = false;
    element_t *curr = NULL, *next = NULL;

    list_for_each_entry_safe (curr, next, head, list) {
        if (&next->list != head && !strcmp(curr->value, next->value)) {
            list_del(&curr->list);
            q_release_element(curr);
            isdup = true;
        } else if (isdup) {
            list_del(&curr->list);
            q_release_element(curr);
            isdup = false;
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *curr = NULL, *next = NULL;
    list_for_each_safe (curr, next, head) {
        list_move(curr, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || k == 1)
        return;

    struct list_head *prev = head, *cur, *next;
    int count = q_size(head);

    while (count >= k) {
        cur = prev->next;
        next = cur->next;

        /*
         * Reverse k nodes by moving the i-th node
         * to the front of the k-group each round.
         */
        for (int i = 1; i < k; i++) {
            list_move(next, prev);
            next = cur->next;
        }

        prev = cur;
        count -= k;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return q_size(head);

    struct list_head *min = head->prev, *curr = min->prev;

    for (struct list_head *prev = curr->prev; curr != head; prev = curr->prev) {
        element_t *curr_ele = list_entry(curr, element_t, list);
        const element_t *min_ele = list_entry(min, element_t, list);

        if (strcmp(curr_ele->value, min_ele->value) < 0) {
            min = curr;
        } else {
            list_del(curr);
            q_release_element(curr_ele);
        }
        curr = prev;
    }

    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return q_size(head);

    struct list_head *max = head->prev, *curr = max->prev;

    for (struct list_head *prev = curr->prev; curr != head; prev = curr->prev) {
        element_t *curr_ele = list_entry(curr, element_t, list);
        const element_t *max_ele = list_entry(max, element_t, list);

        if (strcmp(curr_ele->value, max_ele->value) > 0) {
            max = curr;
        } else {
            list_del(curr);
            q_release_element(curr_ele);
        }
        curr = prev;
    }

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
