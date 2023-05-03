// vim:ft=c

#ifndef LINKED_LISTS_H
#define LINKED_LISTS_H

#include <stdlib.h>
#include <stdbool.h>

#include "types_macros.h"

typedef struct LinkedListNode_ {
	union {
		u64 i_val;
		double f_val;
		void* content;
	};
	struct LinkedListNode_* next;
} LinkedListNode;

typedef struct {
	u32 len;
	LinkedListNode* head;
	LinkedListNode* tail;
	void (*dealloc)(void*);
} LinkedList;

// Usefull type aliases.
typedef LinkedList LL_Points;
typedef LinkedList LL_Contours;
typedef LinkedList LL_Bezier2;
typedef LinkedList LL_Bezier3;

// Returns a new (empty) LinkedList.
// dealloc is the function used to free the `content` union feild
// (use NULL as argument if you don’t need to free the content)
LinkedList* LL_new_empty(void (*dealloc)(void*));

// Deletes both content of the list and the handle.
void LL_delete(LinkedList*);

// Returns a new LL_new_from with 
LinkedList* LL_from_i32(u32 n_values, ...);
LinkedList* LL_from_float(u32 n_values, ...);
LinkedList* LL_from_ptr(void (*dealloc)(void*), u32 n_values, ...);

#define LN_next(node) node = node->next

#define LL_for_each_i32(list_name, i32_name) \
	for (LinkedListNode* LL_for_each_i32 = (list_name)->head; \
			LL_for_each_i32 && (i32_name = LL_for_each_i32->i_val), LL_for_each_i32; \
			LN_next(LL_for_each_i32))

#define LL_for_each_float(list_name, float_name) \
	for (LinkedListNode* LL_for_each_float = (list_name)->head; \
			LL_for_each_float && (float_name = LL_for_each_float->f_val), LL_for_each_float; \
			LN_next(LL_for_each_float))

#define LL_for_each_ptr(list_name, ptr_name) \
	for (LinkedListNode* LL_for_each_ptr = (list_name)->head; \
			LL_for_each_ptr && (ptr_name = LL_for_each_ptr->content), LL_for_each_ptr; \
			LN_next(LL_for_each_ptr))

static inline bool LL_is_empty(LinkedList* list) {
	return !list->head;
}

// Manually create a new node, with nothing after
LinkedListNode* LL_new_node_i32(u64);
LinkedListNode* LL_new_node_float(double);
LinkedListNode* LL_new_node_ptr(void*);

// Returns a new single node containing a shallow copy of the content of `source`
LinkedListNode* LL_duplicate_node(LinkedListNode* source, size_t content_size);

// Creates a new node containing `new_val` and push it at the top of the list.
// Gives ownership of new_val to the list when passing a pointer
void LL_push_ptr(LinkedList* list, void* new_val);
void LL_push_i32(LinkedList* list, u64 new_val);
void LL_push_float(LinkedList* list, double new_val);

// Creates a new node containing `new_val` and push it at the end of the list.
// Gives ownership of new_val to the list when passing a pointer
void LL_push_tail_ptr(LinkedList* list, void* new_val);
void LL_push_tail_i32(LinkedList* list, u64 new_val);
void LL_push_tail_float(LinkedList* list, double new_val);

// Deletes the node at the top of the list and returns its value.
// Gives ownership of returned value when popping a pointer.
// (crashes if list is empty)
void* LL_pop_ptr(LinkedList* list);
u64 LL_pop_i32(LinkedList* list);
double LL_pop_float(LinkedList* list);

// Appends `source` at the end of `destination`.
// Frees the list handle of `source` (though not the content of course)
void LL_concat(LinkedList* destination, LinkedList* source);

#endif  // LINKED_LISTS_H
