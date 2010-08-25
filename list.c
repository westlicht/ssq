
#include <stdlib.h>
#include <string.h>

#include "list.h"

/* local functions */
static inline void __list_add(struct dlist *list, struct dlist_item *new, struct dlist_item *prev, struct dlist_item *next);
static inline void __list_del(struct dlist *list, struct dlist_item *prev, struct dlist_item *next);
static struct dlist_item *__list_alloc_item(struct dlist *list, void *data);

/*
 * Inits the linked list. If data_size is 0, each list item only contains a
 * data pointer which points to some data outside the list. When a data_size is
 * defined, additional memory is allocated per item, and the item's data pointer
 * is initialized to point to that extra data. Also extra data is copied, when
 * items are added to the list.
 */
void dlist_init(struct dlist *list, unsigned int data_size)
{
	list->head = &list->anchor;
	list->head->next = list->head;
	list->head->prev = list->head;
	list->count = 0;
	list->data_size = data_size;
}

/*
 * Clears the linked list.
 */
void dlist_clear(struct dlist *list)
{
	struct dlist_item *item, *next;

	dlist_for_each_safe(list, item, next) {
		dlist_destroy(list, item);
	}
	
	list->count = 0;
}

/*
 * Sets the sort compare function.
 */
void dlist_set_compare(struct dlist *list, dlist_compare_t compare)
{
	list->compare = compare;
}

/*
 * Adds an item to the head of the linked list.
 */
struct dlist_item *dlist_add(struct dlist *list, void *data)
{
	struct dlist_item *new = __list_alloc_item(list, data);
	__list_add(list, new, list->head, list->head->next);
	return new;
}

/*
 * Adds an item to the tail of the linked list.
 */
struct dlist_item *dlist_add_tail(struct dlist *list, void *data)
{
	struct dlist_item *new = __list_alloc_item(list, data);
	__list_add(list, new, list->head->prev, list->head);
	return new;
}

/*
 * Adds an item to the the linked list and keeps it sorted.
 */
struct dlist_item *dlist_add_sorted(struct dlist *list, void *data)
{
	struct dlist_item *new;
	struct dlist_item *pos;
	int added = 0;
	
	new = __list_alloc_item(list, data);
	
	pos = dlist_get_first(list);
	if (!pos) {
		__list_add(list, new, list->head, list->head->next);
	} else {
		while (pos != list->head) {
			if (list->compare(new, pos) <= 0) {
				__list_add(list, new, pos->prev, pos);
				added = 1;
				break;
			}
			pos = pos->next;
		}
		if (!added)
			__list_add(list, new, list->head->prev, list->head);
	}	
	
	return new;
}

/*
 * Inserts an item before another item in the linked list.
 */
struct dlist_item *dlist_insert_before(struct dlist *list, struct dlist_item *item, void *data)
{
	struct dlist_item *new = __list_alloc_item(list, data);
	__list_add(list, new, item->prev, item);
	return new;
}

/*
 * Inserts an item after another item in the linked list.
 */
struct dlist_item *dlist_insert_after(struct dlist *list, struct dlist_item *item, void *data)
{
	struct dlist_item *new = __list_alloc_item(list, data);
	__list_add(list, new, item, item->next);
	return new;
}

/*
 * Remove an item from the linked list.
 * Attetion: Memory of item is not freed.
 */
struct dlist_item *dlist_remove(struct dlist *list, struct dlist_item *item)
{
	__list_del(list, item->prev, item->next);
	return item;
}

/*
 * Destroys an item from the linked list.
 * Note: Memory is freed.
 */
void dlist_destroy(struct dlist *list, struct dlist_item *item)
{
	__list_del(list, item->prev, item->next);
	free(item);
}

/*
 * Moves an item to the head of another list.
 */
void dlist_move(struct dlist *list, struct dlist_item *item, struct dlist *dst)
{
	__list_del(list, item->prev, item->next);
	__list_add(dst, item, dst->head, dst->head->next);
}

/*
 * Moves an item to the tail of another list.
 */
void dlist_move_tail(struct dlist *list, struct dlist_item *item, struct dlist *dst)
{
	__list_del(list, item->prev, item->next);
	__list_add(dst, item, dst->head->prev, dst->head);
}

/*
 * Moves an item to another list and keeps it sorted.
 */
void dlist_move_sorted(struct dlist *list, struct dlist_item *item, struct dlist *dst)
{
	struct dlist_item *pos;
	int added = 0;

	__list_del(list, item->prev, item->next);
	
	pos = dlist_get_first(dst);
	if (!pos) {
		__list_add(dst, item, dst->head, dst->head->next);
	} else {
		while (pos != dst->head) {
			if (dst->compare(item, pos) <= 0) {
				__list_add(dst, item, pos->prev, pos);
				added = 1;
				break;
			}
			pos = pos->next;
		}
		if (!added)
			__list_add(dst, item, dst->head->prev, dst->head);
	}	
}

/*
 * Returns a value different than 0 when the list is empty.
 */
inline int dlist_empty(struct dlist *list)
{
	return list->head->next == list->head;
}

/*
 * Returns the number of items in the list.
 */
inline int dlist_get_count(struct dlist *list)
{
	return list->count;
}

/*
 * Returns the first item in the list.
 */
inline struct dlist_item *dlist_get_first(struct dlist *list)
{
	return list->head->next == list->head ? NULL : list->head->next;
}

/*
 * Returns the last item in the list.
 */
inline struct dlist_item *dlist_get_last(struct dlist *list)
{
	return list->head->prev == list->head ? NULL : list->head->prev;
}

/*
 * Returns the next item.
 */
inline struct dlist_item *dlist_get_next(struct dlist *list, struct dlist_item *item)
{
	return item->next == list->head ? NULL : item->next;
}

/*
 * Returns the previous item.
 */
inline struct dlist_item *dlist_get_prev(struct dlist *list, struct dlist_item *item)
{
	return item->prev == list->head ? NULL : item->prev;
}

/*
 * Finds the item of the given data.
 */
inline struct dlist_item *dlist_find_item(struct dlist *list, void *data)
{
	struct dlist_item *item;

	dlist_for_each(list, item) {	
		if (item->data == data)
			return item;
	}

	return NULL;	
}

/*
 * Sorts the list by using the compare function.
 */
inline void dlist_sort(struct dlist *list)
{
	struct dlist_item *a, *b;
	int sorted = 0;
	
	if (list->count < 2)
		return;

	while (!sorted) {
		sorted = 1;
		a = dlist_get_first(list);
		while (a && a->next) {
			b = a->next;
			if (list->compare(a, b) > 0) {
				/* swap items */
				a->prev->next = b;
				b->next->prev = a;
				
				b->prev = a->prev;
				a->prev = b;
				
				a->next = b->next;
				b->next = a;

				sorted = 0;
			} else {
				a = b;
			}
			if (a->next == list->head)
				break;
		}
	}
}

/**
 * Insert a new entry between two known consecutive entries.
 */
static inline void __list_add(struct dlist *list, struct dlist_item *new, struct dlist_item *prev, struct dlist_item *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
	list->count++;
}

/**
 * Delete a list entry by making the prev/next entries point to each other.
 */
static inline void __list_del(struct dlist *list, struct dlist_item *prev, struct dlist_item *next)
{
	next->prev = prev;
	prev->next = next;
	list->count--;
}

/**
 * Allocates a new item. Copy extra data if necessary.
 */
static struct dlist_item *__list_alloc_item(struct dlist *list, void *data)
{
	struct dlist_item *item = malloc(sizeof(struct dlist_item) + list->data_size);
	
	if (list->data_size) {
		item->data = ((void *) item) + sizeof(struct dlist_item);
		if (data)
			memcpy(item->data, data, list->data_size);
	} else {
		item->data = data;
	}
	
	return item;
}
