#ifndef __LIST_H__
#define __LIST_H__

#include <stdbool.h>

struct List {
	struct List *next;
};

struct HeadList{
	struct List* next;
	struct List* tail;
};

void append_list_tail(struct HeadList *list, struct List *item);
struct List* remove_list_head(struct HeadList *list);
bool is_list_empty(struct HeadList *list);
struct List* remove_list(struct HeadList *list, int wait);
#endif