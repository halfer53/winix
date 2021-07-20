/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_LIST_H
#define _LINUX_LIST_H

#include <winix/type.h>

#ifndef container_of
/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 *
 */
 #define container_of(ptr, type, member)\
    ((type *)( (char *)ptr - offsetof(type,member) ))

#endif

#ifndef NULL
#define	NULL		((void *)0)
#endif

#ifndef WRITE_ONCE
#define WRITE_ONCE(left, right) (left = right)
#endif

#ifndef READ_ONCE
#define READ_ONCE(val) (val)
#endif
/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

#define LIST_POISON1  ((void *) 0)
#define LIST_POISON2  ((void *) 2)

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)


void INIT_LIST_HEAD(struct list_head *list);
#define INIT_LIST_HEAD(list)    do {\
	(list)->next = list;\
	(list)->prev = list;\
} while(0)


/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
void __list_add(struct list_head *new, struct list_head *prev, struct list_head *next);
#define   __list_add(new, prevl, nextl) do{\
\
	(nextl)->prev = new;\
	(new)->next = nextl;\
	(new)->prev = prevl;\
	WRITE_ONCE((prevl)->next, new);\
}while(0)

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
void list_add(struct list_head *new, struct list_head *head);
#define   list_add(new, head)\
do{\
	__list_add(new, head, (head)->next);\
}while(0)


/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
void list_add_tail(struct list_head *new, struct list_head *head);
#define   list_add_tail(new, head)\
do{\
	__list_add(new, (head)->prev, head);\
}while(0)

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
void __list_del(struct list_head * prev, struct list_head * next);
#define   __list_del( prevl,  nextl)\
do{\
	(nextl)->prev = prevl;\
	WRITE_ONCE((prevl)->next, nextl);\
}while(0)

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
void __list_del_entry(struct list_head *entry);
#define   __list_del_entry(entry)\
do{\
	__list_del((entry)->prev, (entry)->next);\
}while(0)

void list_del(struct list_head *entry);
#define   list_del(entry)\
do{\
	__list_del_entry(entry);\
	(entry)->next = LIST_POISON1;\
	(entry)->prev = LIST_POISON2;\
}while(0)

/**
 * list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwrittenREAD_ONCE
}while(0)

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
void list_del_init(struct list_head *entry);
#define   list_del_init(entry)\
do{\
	__list_del_entry(entry);\
	INIT_LIST_HEAD(entry);\
}while(0)

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
void list_move(struct list_head *list, struct list_head *head);
#define   list_move(list, head)\
do{\
	__list_del_entry(list);\
	list_add(list, head);\
}while(0)

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
void list_move_tail(struct list_head *list, struct list_head *head);
#define   list_move_tail(list, head)\
do{\
	__list_del_entry(list);\
	list_add_tail(list, head);\
}while(0)

/**
 * list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
int list_is_last(const struct list_head *list, const struct list_head *head);
#define   list_is_last( list,  head)	((list)->next == head)

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
int list_empty(const struct list_head *head);
#define   list_empty(head)	((head)->next == head)

/**
 * list_empty_careful - tests whether a list is empty and not being modified
 * @head: the list to test
 *
 * Description:
 * tests whether a list is empty _and_ checks that no other CPU might be
 * in the process of modifying either member (next or prev)
 *
 * NOTE: using list_empty_careful() without synchronization
 * can only be safe if the only activity that can happen
 * to the list entry is list_del_init(). Eg. it cannot be used
 * if another CPU could re-list_add() it.
 */
int list_empty_careful(const struct list_head *head);
#define   list_empty_careful( head)		(head->next == head) && (head->next == head->prev)

/**
 * list_rotate_left - rotate the list to the left
 * @head: the head of the list
 */
void list_rotate_left(struct list_head *head);
#define   list_rotate_left(head)\
do{\
	if (!list_empty(head)) {\
		list_move_tail(head->next, head);\
	}\
}while(0)

/**
 * list_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 */
int list_is_singular(const struct list_head *head);
#define   list_is_singular(head)	(!list_empty(head) && (head->next == head->prev))

void __list_cut_position(struct list_head *list, struct list_head *head, struct list_head *entry);
#define   __list_cut_position(list, head, entry)\
do{\
	list->next = head->next;\
	list->next->prev = list;\
	list->prev = entry;\
	head->next = entry->next;\
	entry->next = list;\
	head->next->prev = head;\
}while(0)

/**
 * list_cut_position - cut a list into two
 * @list: a new list to add all removed entries
 * @head: a list with entries
 * @entry: an entry within head, could be the head itself
 *	and if so we won't cut the list
 *
 * This helper moves the initial part of @head, up to and
 * including @entry, from @head to @list. You should
 * pass on @entry an element you know is on @head. @list
 * should be an empty list or a list you do not care about
 * losing its data.
 *
 */
void list_cut_position(struct list_head *list, struct list_head *head, struct list_head *entry);
#define   list_cut_position(list, head, entry)\
do{\
	if (list_empty(head))\
		break;\
	if (list_is_singular(head) &&\
		((head)->next != entry && head != entry))\
		break;\
	if (entry == head)\
		INIT_LIST_HEAD(list);\
	else\
		__list_cut_position(list, head, entry);\
}while(0)

void __list_splice(const struct list_head *list, struct list_head *prev, struct list_head *next);
#define   __list_splice( list, prevl, nextl)\
do{\
	list->next->prev = prev;\
	prev->next = list->next;\
	list->prev->next = next;\
	next->prev = list->prev;\
}while(0)

/**
 * list_splice - join two lists, this is designed for stacks
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
void list_splice(const struct list_head *list, struct list_head *head);
#define   list_splice( list, head)\
do{\
	if (!list_empty(list))\
		__list_splice(list, head, (head)->next);\
}while(0)

/**
 * list_splice_tail - join two lists, each list being a queue
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
void list_splice_tail(struct list_head *list, struct list_head *head);
#define   list_splice_tail(list, head)\
do{\
	if (!list_empty(list))\
		__list_splice(list, (head)->prev, head);\
}while(0)

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
void list_splice_init(struct list_head *list, struct list_head *head);
#define   list_splice_init(list, head)\
do{\
	if (!list_empty(list)) {\
		__list_splice(list, head, (head)->next);\
		INIT_LIST_HEAD(list);\
	}\
}while(0)

/**
 * list_splice_tail_init - join two lists and reinitialise the emptied list
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */
void list_splice_tail_init(struct list_head *list, struct list_head *head);
#define   list_splice_tail_init(list, head)\
do{\
	if (!list_empty(list)) {\
		__list_splice(list, (head)->prev, head);\
		INIT_LIST_HEAD(list);\
	}\
}while(0)


/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
* list_first_entry - get the first element from a list
* @ptr:	the list head to take the element from.
* @type:	the type of the struct this is embedded in.
* @member:	the name of the list_head within the struct.
*
* Note, that list is expected to be not empty.
*/
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

/**
* list_last_entry - get the last element from a list
* @ptr:	the list head to take the element from.
* @type:	the type of the struct this is embedded in.
* @member:	the name of the list_head within the struct.
*
* Note, that list is expected to be not empty.
*/
#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)


/**
 * list_first_entry_or_null - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note that if the list is empty, it returns NULL.
 */
#define list_first_entry_or_null(ptr, type, member) ({ \
	struct list_head *head__ = (ptr); \
	struct list_head *pos__ = READ_ONCE(head__->next); \
	pos__ != head__ ? list_entry(pos__, type, member) : NULL; \
})

/**
 * list_next_entry - get the next element in list
 * @struct:	typeof(*(pos))
 * @pos:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define list_next_entry(struct, pos, member) \
	list_entry((pos)->member.next, struct, member)

/**
 * list_prev_entry - get the prev element in list
 * @struct:	typeof(*(pos))
 * @pos:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define list_prev_entry(struct, pos, member) \
	list_entry((pos)->member.prev, struct, member)

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_prev_safe(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev; \
	     pos != (head); \
	     pos = n, n = pos->prev)

/**
 * list_for_each_entry	-	iterate over list of given type
 * @struct:	typeof(*(pos))
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry(struct, pos, head, member)				\
	for (pos = list_first_entry(head, struct, member);	\
	     &pos->member != (head);					\
	     pos = list_next_entry(struct, pos, member))

/**
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @struct:	typeof(*(pos))
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_reverse(struct, pos, head, member)			\
	for (pos = list_last_entry(head, struct, member);		\
	     &pos->member != (head); 					\
	     pos = list_prev_entry(struct, pos, member))

/**
 * list_prepare_entry - prepare a pos entry for use in list_for_each_entry_continue()
 * @struct:	typeof(*(pos))
 * @pos:	the type * to use as a start point
 * @head:	the head of the list
 * @member:	the name of the list_head within the struct.
 *
 * Prepares a pos entry for use as a start point in list_for_each_entry_continue().
 */
#define list_prepare_entry(struct, pos, head, member) \
	((pos) ? : list_entry(head, struct, member))

/**
 * list_for_each_entry_continue - continue iteration over list of given type
 * @struct:	typeof(*(pos))
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define list_for_each_entry_continue(struct, pos, head, member) 		\
	for (pos = list_next_entry(struct, pos, member);			\
	     &pos->member != (head);					\
	     pos = list_next_entry(struct, pos, member))

/**
 * list_for_each_entry_continue_reverse - iterate backwards from the given point
 * @struct:	typeof(*(pos))
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define list_for_each_entry_continue_reverse(struct, pos, head, member)		\
	for (pos = list_prev_entry(struct, pos, member);			\
	     &pos->member != (head);					\
	     pos = list_prev_entry(struct, pos, member))

/**
 * list_for_each_entry_from - iterate over list of given type from the current point
 * @struct:	typeof(*(pos))
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define list_for_each_entry_from(struct, pos, head, member) 			\
	for (; &pos->member != (head);					\
	     pos = list_next_entry(struct, pos, member))

/**
 * list_for_each_entry_from_reverse - iterate backwards over list of given type
 *                                    from the current point
 * @struct:	typeof(*(pos))
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate backwards over list of given type, continuing from current position.
 */
#define list_for_each_entry_from_reverse(struct, pos, head, member)		\
	for (; &pos->member != (head);					\
	     pos = list_prev_entry(struct, pos, member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @struct:	typeof(*(pos))
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_safe(struct, pos, n, head, member)			\
	for (pos = list_first_entry(head, struct, member),	\
		n = list_next_entry(struct, pos, member);			\
	     &pos->member != (head); 					\
	     pos = n, n = list_next_entry(struct, n, member))

/**
 * list_for_each_entry_safe_continue - continue list iteration safe against removal
 * @struct:	typeof(*(pos))
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define list_for_each_entry_safe_continue(struct, pos, n, head, member) 		\
	for (pos = list_next_entry(struct, pos, member), 				\
		n = list_next_entry(struct, pos, member);				\
	     &pos->member != (head);						\
	     pos = n, n = list_next_entry(struct, n, member))

/**
 * list_for_each_entry_safe_from - iterate over list from current point safe against removal
 * @struct:	typeof(*(pos))
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define list_for_each_entry_safe_from(struct, pos, n, head, member) 			\
	for (n = list_next_entry(struct, pos, member);					\
	     &pos->member != (head);						\
	     pos = n, n = list_next_entry(struct, n, member))

/**
 * list_for_each_entry_safe_reverse - iterate backwards over list safe against removal
 * @struct:	typeof(*(pos))
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define list_for_each_entry_safe_reverse(struct, pos, n, head, member)		\
	for (pos = list_last_entry(head, struct, member),		\
		n = list_prev_entry(struct, pos, member);			\
	     &pos->member != (head); 					\
	     pos = n, n = list_prev_entry(struct, n, member))

/**
 * list_safe_reset_next - reset a stale list_for_each_entry_safe loop
 * @struct:	typeof(*(pos))
 * @pos:	the loop cursor used in the list_for_each_entry_safe loop
 * @n:		temporary storage used in list_for_each_entry_safe
 * @member:	the name of the list_head within the struct.
 *
 * list_safe_reset_next is not safe to use in general if the list may be
 * modified concurrently (eg. the lock is dropped in the loop body). An
 * exception to this is if the cursor element (pos) is pinned in the list,
 * and list_safe_reset_next is called after re-taking the lock and before
 * completing the current iteration of the loop body.
 */
#define list_safe_reset_next(struct, pos, n, member)				\
	n = list_next_entry(struct, pos, member)

// /*
//  * Double linked lists with a single pointer list head.
//  * Mostly useful for hash tables where the two pointer list head is
//  * too wasteful.
//  * You lose the ability to access the tail in O(1).
//  */

// #define HLIST_HEAD_INIT { .first = NULL }
// #define HLIST_HEAD(name) struct hlist_head name = {  .first = NULL }
// #define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)
// void INIT_HLIST_NODE(struct hlist_node *h);
// #define   INIT_HLIST_NODE(h)\
// do{\
// 	(h)->next = NULL;\
// 	(h)->pprevl = NULL;\
// }while(0)

// int hlist_unhashed(const struct hlist_node *h);
// #define   hlist_unhashed( h)	!(h)->pprevl;\

// int hlist_empty(const struct hlist_head *h);
// #define   hlist_empty( h)\
// do{\
// 	return !READ_ONCE((h)->first);\
// }while(0)

// void __hlist_del(struct hlist_node *n);
// #define   __hlist_del(n)\
// do{\
// 	nextl = (n)->next;\
// 	*pprevl = (n)->pprevl;\
// \
// 	WRITE_ONCE(*pprevl, nextl);\
// 	if (nextl)\
// 		(nextl)->pprevl = pprevl;\
// }while(0)

// void hlist_del(struct hlist_node *n);
// #define   hlist_del(n)\
// do{\
// 	__hlist_del(n);\
// 	(n)->next = LIST_POISON1;\
// 	(n)->pprevl = LIST_POISON2;\
// }while(0)

// void hlist_del_init(struct hlist_node *n);
// #define   hlist_del_init(n)\
// do{\
// 	if (!hlist_unhashed(n)) {\
// 		__hlist_del(n);\
// 		INIT_HLIST_NODE(n);\
// 	}\
// }while(0)

// void hlist_add_head(struct hlist_node *n, struct hlist_head *h);
// #define   hlist_add_head(n, h)\
// do{\
// 	first = (h)->first;\
// 	(n)->next = first;\
// 	if (first)\
// 		(first)->pprevl = &(n)->next;\
// 	WRITE_ONCE((h)->first, n);\
// 	(n)->pprevl = &(h)->first;\
// }while(0)

// /* next must be != NULL */
// void hlist_add_before(struct hlist_node *n, struct hlist_node *next);
// #define   hlist_add_before(n, nextl)\
// do{\
// 	(n)->pprevl = (nextl)->pprevl;\
// 	(n)->next = nextl;\
// 	(nextl)->pprevl = &(n)->next;\
// 	WRITE_ONCE(*((n)->pprevl), n);\
// }while(0)

// void hlist_add_behind(struct hlist_node *n, struct hlist_node *prev);
// #define   hlist_add_behind(n, prevl)\
// do{\
// 	(n)->next = (prevl)->next;\
// 	WRITE_ONCE((prevl)->next, n);\
// 	(n)->pprevl = &(prevl)->next;\
// \
// 	if ((n)->next)\
// 		(n)->(next)->pprevl  = &(n)->next;\
// }while(0)

// /* after that we'll appear to be on some hlist and hlist_del will work */
// void hlist_add_fake(struct hlist_node *n);
// #define   hlist_add_fake(n)\
// do{\
// 	(n)->pprevl = &(n)->next;\
// }while(0)

// bool hlist_fake(struct hlist_node *h);
// #define   hlist_fake(h)\
// do{\
// 	return (h)->pprevl == &(h)->next;\
// }while(0)

// /*
//  * Check whether the node is the only node of the head without
//  * accessing head:
//  */
// bool hlist_is_singular_node(struct hlist_node *n, struct hlist_head *h);
// #define   hlist_is_singular_node(n, h)\
// do{\
// 	return !(n)->next && (n)->pprevl == &(h)->first;\
// }while(0)

// /*
//  * Move a list from one list head to another. Fixup the pprev
//  * reference of the first entry if it exists.
//  */
// void hlist_move_list(struct hlist_head *old, struct hlist_head *new);
// #define   hlist_move_list(old, new)\
// do{\
// 	(new)->first = (old)->first;\
// 	if ((new)->first)\
// 		(new)->(first)->pprevl = &(new)->first;\
// 	(old)->first = NULL;\
// }while(0)

// #define hlist_entry(struct, ptr, type, member) \
// 		container_of(ptr,type,member)

// #define hlist_for_each(pos, head) \
// 	for (pos = (head)->first; pos ; pos = pos->next)

// #define hlist_for_each_safe(pos, n, head) \
// 	for (pos = (head)->first; pos && ({ n = pos->next; 1; }); \
// 	     pos = n)

// #define hlist_entry_safe(ptr, type, member) \
// 	({ typeof(ptr) ____ptr = (ptr); \
// 	   ____ptr ? hlist_entry(____ptr, type, member) : NULL; \
// 	})

// /**
//  * hlist_for_each_entry	- iterate over list of given type
//  * @struct:	typeof(*(pos))
//  * @pos:	the type * to use as a loop cursor.
//  * @head:	the head for your list.
//  * @member:	the name of the hlist_node within the struct.
//  */
// #define hlist_for_each_entry(struct, pos, head, member)				\
// 	for (pos = hlist_entry_safe((head)->first, struct, member);\
// 	     pos;							\
// 	     pos = hlist_entry_safe((pos)->member.next, struct, member))

// /**
//  * hlist_for_each_entry_continue - iterate over a hlist continuing after current point
//  * @struct:	typeof(*(pos))
//  * @pos:	the type * to use as a loop cursor.
//  * @member:	the name of the hlist_node within the struct.
//  */
// #define hlist_for_each_entry_continue(struct, pos, member)			\
// 	for (pos = hlist_entry_safe((pos)->member.next, struct, member);\
// 	     pos;							\
// 	     pos = hlist_entry_safe((pos)->member.next, struct, member))

// /**
//  * hlist_for_each_entry_from - iterate over a hlist continuing from current point
//  * @struct:	typeof(*(pos))
//  * @pos:	the type * to use as a loop cursor.
//  * @member:	the name of the hlist_node within the struct.
//  */
// #define hlist_for_each_entry_from(struct,pos, member)				\
// 	for (; pos;							\
// 	     pos = hlist_entry_safe((pos)->member.next, struct, member))

// /**
//  * hlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
//  * @struct:	typeof(*(pos))
//  * @pos:	the type * to use as a loop cursor.
//  * @n:		another &struct hlist_node to use as temporary storage
//  * @head:	the head for your list.
//  * @member:	the name of the hlist_node within the struct.
//  */
// #define hlist_for_each_entry_safe(struct, pos, n, head, member) 		\
// 	for (pos = hlist_entry_safe((head)->first, struct, member);\
// 	     pos && ({ n = pos->member.next; 1; });			\
// 	     pos = hlist_entry_safe(n, struct, member))

 #endif
