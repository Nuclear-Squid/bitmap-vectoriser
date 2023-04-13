#include <stdlib.h>
#include <sys/types.h>
#include <stdarg.h>
#include <string.h>

#include "linked_lists.h"

LinkedList* LL_new_empty(void (*dealloc)(void*)) {
	LinkedList* rv = calloc(1, sizeof(LinkedList));
	rv->dealloc = dealloc;
	return rv;
}

void LL_delete(LinkedList* list) {
	LinkedListNode* current_node = list->head;

	while (current_node) {
		LinkedListNode* next_node = current_node->next;
		if (list->dealloc) list->dealloc(current_node->content);
		free(current_node);
		current_node = next_node;
	}

	free(list);
}

LinkedList* LL_from_int(void (*dealloc)(void*), u_int32_t n_values, ...) {
	LinkedList* rv = LL_new_empty(dealloc);
	va_list va_list_ptr;
	va_start(va_list_ptr, n_values);
	for (u_int32_t i = 0; i < n_values; i++) {
		LL_push_tail_int(rv, va_arg(va_list_ptr, u_int64_t));
	}
	return rv;
}

LinkedList* LL_from_float(void (*dealloc)(void*), u_int32_t n_values, ...) {
	LinkedList* rv = LL_new_empty(dealloc);
	va_list va_list_ptr;
	va_start(va_list_ptr, n_values);
	for (u_int32_t i = 0; i < n_values; i++) {
		LL_push_tail_float(rv, va_arg(va_list_ptr, double));
	}
	return rv;
}

LinkedList* LL_from_ptr(void (*dealloc)(void*), u_int32_t n_values, ...) {
	LinkedList* rv = LL_new_empty(dealloc);
	va_list va_list_ptr;
	va_start(va_list_ptr, n_values);
	for (u_int32_t i = 0; i < n_values; i++) {
		LL_push_tail_ptr(rv, va_arg(va_list_ptr, void*));
	}
	return rv;
}

LinkedListNode* LL_new_node_int(u_int64_t val) {
	LinkedListNode* rv = calloc(1, sizeof(LinkedListNode));
	rv->i_val = val;
	return rv;
}

LinkedListNode* LL_new_node_float(double val) {
	LinkedListNode* rv = calloc(1, sizeof(LinkedListNode));
	rv->f_val = val;
	return rv;
}

LinkedListNode* LL_new_node_ptr(void* ptr) {
	LinkedListNode* rv = calloc(1, sizeof(LinkedListNode));
	rv->content = ptr;
	return rv;
}

void LL_push_int(LinkedList* list, u_int64_t new_val) {
	list->len++;
	if (LL_is_empty(list)) {
		list->head = LL_new_node_int(new_val);
		list->tail = list->head;
	} else {
		LinkedListNode* new_head = LL_new_node_int(new_val);
		new_head->next = list->head;
		list->head = new_head;
	}
}

void LL_push_float(LinkedList* list, double new_val) {
	list->len++;
	if (LL_is_empty(list)) {
		list->head = LL_new_node_float(new_val);
		list->tail = list->head;
	} else {
		LinkedListNode* new_head = LL_new_node_float(new_val);
		new_head->next = list->head;
		list->head = new_head;
	}
}

void LL_push_ptr(LinkedList* list, void* new_val) {
	list->len++;
	if (LL_is_empty(list)) {
		list->head = LL_new_node_ptr(new_val);
		list->tail = list->head;
	} else {
		LinkedListNode* new_head = LL_new_node_ptr(new_val);
		new_head->next = list->head;
		list->head = new_head;
	}
}

void LL_push_tail_int(LinkedList* list, u_int64_t new_val) {
	list->len++;
	if (LL_is_empty(list)) {
		list->head = LL_new_node_int(new_val);
		list->tail = list->head;
	} else {
		list->tail->next = LL_new_node_int(new_val);
		list->tail = list->tail->next;
	}
}

void LL_push_tail_float(LinkedList* list, double new_val) {
	list->len++;
	if (LL_is_empty(list)) {
		list->head = LL_new_node_float(new_val);
		list->tail = list->head;
	} else {
		list->tail->next = LL_new_node_float(new_val);
		list->tail = list->tail->next;
	}
}

void LL_push_tail_ptr(LinkedList* list, void* new_val) {
	list->len++;
	if (LL_is_empty(list)) {
		list->head = LL_new_node_ptr(new_val);
		list->tail = list->head;
	} else {
		list->tail->next = LL_new_node_ptr(new_val);
		list->tail = list->tail->next;
	}
}

void* LL_pop_ptr(LinkedList* list) {
	void* rv = list->head->content;
	LinkedListNode* del = list->head;
	list->head = list->head->next;
	free(del);
	list->len--;
	return rv;
}

u_int64_t LL_pop_int(LinkedList* list) {
	u_int64_t rv = list->head->i_val;
	LinkedListNode* del = list->head;
	list->head = list->head->next;
	free(del);
	list->len--;
	return rv;
}

double LL_pop_float(LinkedList* list) {
	double rv = list->head->f_val;
	LinkedListNode* del = list->head;
	list->head = list->head->next;
	free(del);
	list->len--;
	return rv;
}

void LL_concat(LinkedList* destination, LinkedList* source) {
	destination->tail->next = source->head;
	destination->tail = source->tail;
	destination->len += source->len;
	free(source);
}
