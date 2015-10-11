/**
 * linked_list.h
 * 
 * https://isis.poly.edu/kulesh/stuff/src/klist/
 *
 * \date    
 * \author  jtuki@foxmail.com
 */

#ifndef HADDOCK_LINKED_LIST_H_
#define HADDOCK_LINKED_LIST_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "haddock_types.h"

struct list_head {
    struct list_head *next;
    struct list_head *prev;
};

/**
 * Initialize a struct list_head.
 */
#define list_head_init(ptr) do { \
    (ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

/**
 * list_entry - get the struct for this entry.
 * @ptr:    the &struct list_head pointer.
 * @type:   the type of the struct which contains the list_head.
 * @member: the name of the list_struct within the struct.
 * 
 * \ref https://isis.poly.edu/kulesh/stuff/src/klist/
 *      Section "How Does This Work?"
 */
#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

/**
 * list_for_each - iterate over a list
 * @pos:    the &struct list_head to use as a loop counter.
 * @head:   the head for your list.
 */
#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_prev - iterate over a list backwards
 * @pos:    the &struct list_head to use as a loop counter.
 * @head:   the head for your list.
 */
#define list_for_each_prev(pos, head) \
    for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * list_for_each_safe - iterate over a list safe against removal of @pos.
 * @pos:    the &struct list_head to use as a loop counter.
 * @n:      another &struct list_head to use as temporary storage
 * @head:   the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

/**
 * Insert a new entry between two known consecutive entries. 
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_add(struct list_head *new,
                              struct list_head *prev,
                              struct list_head *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

/**
 * list_add - add a new entry
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void list_add(struct list_head *new, struct list_head *head)
{
    __list_add(new, head, head->next);
}

/**
 * list_add_tail - add a new entry
 *
 * Insert a new entry before the specified head (i.e. add to tail).
 * This is useful for implementing queues.
 */
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
    __list_add(new, head->prev, head);
}

/**
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * 
 * Note: list_empty on entry does not return OS_TRUE after this, the entry is in 
 * an undefined state.
 */
static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = (void *) 0;
    entry->prev = (void *) 0;
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void list_del_init(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    list_head_init(entry);
}

/**
 * list_move - delete from one list and add as another's head
 * @entry: the entry to move
 * @head: the head that will precede our entry
 */
static inline void list_move(struct list_head *entry, struct list_head *head)
{
    __list_del(entry->prev, entry->next);
    list_add(entry, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @entry: the entry to move
 * @head: the head that will follow our entry
 */
static inline void list_move_tail(struct list_head *entry,
                                  struct list_head *head)
{
    __list_del(entry->prev, entry->next);
    list_add_tail(entry, head);
}

/**
 * list_empty - tests whether a list is empty
 * @list_hdr: the list to test.
 */
static inline os_boolean list_empty(const struct list_head *list_hdr)
{
    return list_hdr->next == list_hdr;
}

/**
 * \ref list_splice(list, head)
 */
static inline void __list_splice(struct list_head *list,
                                 struct list_head *at)
{
    struct list_head *first = list->next;
    struct list_head *last = list->prev;
    struct list_head *after = at->next;

    first->prev = at;
    at->next = first;

    last->next = after;
    after->prev = last;
}

/**
 * list_splice - join two lists.
 * 
 * @list: the new list to add.
 * @at: the place to add @list.
 * 
 * Before call \func list_splice(list, B_entry):
 *      list_A: list, list->next (A_first), ..., list->prev (A_last)
 *      list_B: ..., B_entry, B_entry->next, ...
 * 
 * After function call list_splice(list, B_entry):
 *      list_B: ..., B_entry, A_first, ..., A_last, B_entry->next, ... 
 */
static inline void list_splice(struct list_head *list, struct list_head *at)
{
    if (!list_empty(list))
        __list_splice(list, at);
}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void list_splice_init(struct list_head *list,
                                    struct list_head *at)
{
    if (!list_empty(list)) {
        __list_splice(list, at);
        list_head_init(list);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* HADDOCK_LINKED_LIST_H_ */
