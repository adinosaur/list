#ifndef DMLIB_LIST_H_
#define DMLIB_LIST_H_

#include <stdlib.h>
#define List_Malloc malloc
#define List_Free free

#include <stdint.h>
#define LIST_MAX_SIZE INT32_MAX

typedef void(*tdListDestroyFun)(void*);
typedef void(*tdListForeachFun)(void*);

struct List* list_create(size_t, tdListDestroyFun);
void list_destory(struct List*);
void* list_getitem(struct List*, size_t);
int list_setitem(struct List*, size_t, void*);
int list_append(struct List*, void*);
void* list_pop(struct List*);
int list_insert(struct List*, size_t, void*);
size_t list_size(struct List*);
void list_foreach(struct List*, tdListForeachFun);

#endif //DMLIB_LIST_H_