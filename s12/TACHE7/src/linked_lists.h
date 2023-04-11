// vim:ft=c

#ifndef LINKED_LISTS_H
#define LINKED_LISTS_H

#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>

typedef struct LinkedListNode_ {
	union {
		u_int64_t i_val;
		double f_val;
		void* content;
	};
	struct LinkedListNode_* next;
} LinkedListNode;

typedef struct {
	u_int32_t len;
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
LinkedList* new_linked_list(void (*dealloc)(void*));

// Deletes both content of the list and the handle.
void delete_linked_list(LinkedList*);

#define LN_next(node) node = node->next

#define LL_for_each_node(list_name, node_name) \
	for (LinkedListNode* node_name = (list_name)->head; node_name; LN_next(node_name))

#define LL_for_each_int(list_name, int_name) \
	for (LinkedListNode* LL_for_each_int = (list_name)->head; \
			LL_for_each_int && (int_name = LL_for_each_int->i_val), LL_for_each_int; \
			LN_next(LL_for_each_int))

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
LinkedListNode* LL_new_node_int(u_int64_t);
LinkedListNode* LL_new_node_float(double);
LinkedListNode* LL_new_node_ptr(void*);

// Creates a new node containing `new_val` and push it at the top of the list.
// Gives ownership of new_val to the list when passing a pointer
void LL_push_ptr(LinkedList* list, void* new_val);
void LL_push_int(LinkedList* list, u_int64_t new_val);
void LL_push_float(LinkedList* list, double new_val);

// Creates a new node containing `new_val` and push it at the end of the list.
// Gives ownership of new_val to the list when passing a pointer
void LL_push_tail_ptr(LinkedList* list, void* new_val);
void LL_push_tail_int(LinkedList* list, u_int64_t new_val);
void LL_push_tail_float(LinkedList* list, double new_val);

// Deletes the node at the top of the list and returns its value.
// Gives ownership of returned value when popping a pointer.
// (crashes if list is empty)
void* LL_pop_ptr(LinkedList* list);
u_int64_t LL_pop_int(LinkedList* list);
double LL_pop_float(LinkedList* list);

/* // Deletes the node at the end of the list and returns its value. */
/* // Gives ownership of returned value when popping a pointer. */
/* // (crashes if list is empty and has O(n) execution time) */
/* void* LL_pop_tail_ptr(LinkedList* list); */
/* u_int64_t LL_pop_tail_int(LinkedList* list); */
/* double LL_pop_tail_float(LinkedList* list); */

// Appends `source` at the end of `destination`.
// Frees the list handle of `source` (though not the content of course)
void LL_concat(LinkedList* destination, LinkedList* source);

#endif  // LINKED_LISTS_H
