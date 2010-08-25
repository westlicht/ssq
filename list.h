
#ifndef __LIST_H__
#define __LIST_H__

/** doubly-linked list item */
struct dlist_item {
	struct dlist_item *next;
	struct dlist_item *prev;
	void *data;
};

/** 
 * doubly-list sort comparator - this function should return:
 * if a < b return < 0
 * if a == b return 0
 * if a > b return > 0
 */
typedef int (*dlist_compare_t) (struct dlist_item *a, struct dlist_item *b);

/** doubly-linked list */
struct dlist {
	struct dlist_item anchor, *head;      /**< list's anchor */
	unsigned int count;                   /**< number of items in the list */
	dlist_compare_t compare;              /**< compare callback function for sorting */ 
	unsigned int data_size;               /**< size of extra data */
};

/* loop through a list */
#define dlist_for_each(list, item) \
	for (item = (list)->head->next; item != (list)->head; item = item->next)
	
/* reverse loop through a list */
#define dlist_for_each_rev(list, item) \
	for (item = (list)->head->prev; item != (list)->head; item = item->prev)
	
/* loop through a list (safe against removal of list elements) */
#define dlist_for_each_safe(list, item, next_item) \
	for (item = (list)->head->next, next_item = item->next; item != (list)->head; item = next_item, next_item = item->next)

/* reverse loop through a list (safe against removal of list elements) */
#define dlist_for_each_rev_safe(list, item, prev_item) \
	for (item = (list)->head->prev, prev_item = item->prev; item != (list)->head; item = prev_item, prev_item = item->prev)

/**
 * Inits the linked list. If data_size is 0, each list item only contains a
 * data pointer which points to some data outside the list. When a data_size is
 * defined, additional memory is allocated per item, and the item's data pointer
 * is initialized to point to that extra data. Also extra data is copied, when
 * items are added to the list.
 * @param list Linked list
 * @param data_size Size of extra data
 */
void dlist_init(struct dlist *list, unsigned int data_size);

/**
 * Clears the linked list.
 * @param list Linked list
 */
void dlist_clear(struct dlist *list);

/**
 * Sets the sort compare function.
 * @param list Linked list
 * @param compare Compare function
 */
void dlist_set_compare(struct dlist *list, dlist_compare_t compare);

/**
 * Adds an item to the head of the linked list.
 * @param list Linked list
 * @param data Data
 * @return Returns the newly created list item.
 */
struct dlist_item *dlist_add(struct dlist *list, void *data);

/**
 * Adds an item to the tail of the linked list.
 * @param list Linked list
 * @param data Data
 * @return Returns the newly created list item.
 */
struct dlist_item *dlist_add_tail(struct dlist *list, void *data);

/**
 * Adds an item to the the linked list and keeps it sorted.
 * @param list Linked list
 * @param data Data
 * @return Returns the newly created list item.
 */
struct dlist_item *dlist_add_sorted(struct dlist *list, void *data);

/**
 * Inserts an item before another item in the linked list.
 * @param list Linked list
 * @param item Item
 * @param data Data
 * @param Returns the newly created list item.
 */
struct dlist_item *dlist_insert_before(struct dlist *list, struct dlist_item *item, void *data);

/**
 * Inserts an item after another item in the linked list.
 * @param list Linked list
 * @param item Item
 * @param data Data
 * @param Returns the newly created list item.
 */
struct dlist_item *dlist_insert_after(struct dlist *list, struct dlist_item *item, void *data);

/**
 * Remove an item from the linked list.
 * Attetion: Memory of item is not freed.
 * @param list Linked list
 * @param item Item to remove
 * @return Returns the removed item.
 */
struct dlist_item *dlist_remove(struct dlist *list, struct dlist_item *item);

/**
 * Destroys an item from the linked list.
 * Note: Memory is freed.
 * @param list Linked list
 * @param item Item to destroy
 */
void dlist_destroy(struct dlist *list, struct dlist_item *item);

/**
 * Moves an item to the head of another list.
 * @param list Linked list to remove item
 * @param item Item to move
 * @param dst Linked list to add item
 */
void dlist_move(struct dlist *list, struct dlist_item *item, struct dlist *dst);

/**
 * Moves an item to the tail of another list.
 * @param list Linked list to remove item
 * @param item Item to move
 * @param dst Linked list to add item
 */
void dlist_move_tail(struct dlist *list, struct dlist_item *item, struct dlist *dst);

/**
 * Moves an item to another list and keeps it sorted.
 * @param list Linked list to remove item
 * @param item Item to move
 * @param dst Linked list to add item
 */
void dlist_move_sorted(struct dlist *list, struct dlist_item *item, struct dlist *dst);

/**
 * Returns a value different than 0 when the list is empty.
 * @param list Linked list
 * @return Returns a value different than 0 when the list is empty.
 */
inline int dlist_empty(struct dlist *list);

/**
 * Returns the number of items in the list.
 * @param list Linked list
 * @return Returns the number of items in the list.
 */
inline int dlist_get_count(struct dlist *list);

/**
 * Returns the first item in the list.
 * @param list Linked list
 * @return Returns the first item in the list.
 */
inline struct dlist_item *dlist_get_first(struct dlist *list);

/**
 * Returns the last item in the list.
 * @param list Linked list
 * @return Returns the last item in the list.
 */
inline struct dlist_item *dlist_get_last(struct dlist *list);

/**
 * Returns the next item.
 * @param list Linked list
 * @param item Item
 * @return Returns the next item, or NULL if none is available.
 */
inline struct dlist_item *dlist_get_next(struct dlist *list, struct dlist_item *item);

/**
 * Returns the previous item.
 * @param list Linked list
 * @param item Item
 * @return Returns the previous item, or NULL if none is available.
 */
inline struct dlist_item *dlist_get_prev(struct dlist *list, struct dlist_item *item);

/**
 * Finds the item of the given data.
 * @param list Linked list
 * @param data Data to find
 * @return Returns the item or 0 if not found.
 */
inline struct dlist_item *dlist_find_item(struct dlist *list, void *data);

/**
 * Sorts the list by using the compare function.
 * @param list Linked list
 */
inline void dlist_sort(struct dlist *list);

#endif /*__LIST_H__*/
