#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include <sys/types.h>

typedef struct LinkedListNode_ {
	void* content;
	struct LinkedListNode_* next;
} LinkedListNode;

typedef struct {
	u_int32_t len;
	LinkedListNode* head;
	LinkedListNode* tail;
} LinkedList;

#endif
