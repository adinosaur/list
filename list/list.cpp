#include "list.h"
#include <stdio.h>	// fprintf
#include <string.h> // memset
#include <assert.h>

struct List {
	size_t size;
	size_t capacity;
	tdListDestroyFun destory_fun;

	void **ob_item;
};

static int
list_resize(struct List* l, size_t newsize)
{
	void **items;
	size_t new_capacity, capacity = l->capacity;

	/* Bypass realloc() when a previous overallocation is large enough
	to accommodate the newsize.  If the newsize falls lower than half
	the allocated size, then proceed with the realloc() to shrink the list.
	*/
	if (capacity >= newsize && newsize >= (capacity >> 1)) {
		assert(l->ob_item != NULL || newsize == 0);
		l->size = newsize;
		return 0;
	}

	/* This over-allocates proportional to the list size, making room
	* for additional growth.  The over-allocation is mild, but is
	* enough to give linear-time amortized behavior over a long
	* sequence of appends() in the presence of a poorly-performing
	* system realloc().
	* The growth pattern is:  0, 4, 8, 16, 25, 35, 46, 58, 72, 88, ...
	*/
	new_capacity = (newsize >> 3) + (newsize < 9 ? 3 : 6);
	
	/* 无符号整数溢出 */
	if (new_capacity > (size_t)-1 - newsize)
		return -1;
	else
		new_capacity += newsize;

	if (newsize == 0)
		new_capacity = 0;
	
	items = l->ob_item;
	if (new_capacity <= (LIST_MAX_SIZE / sizeof(void*)))
		items = (void**)realloc(items, sizeof(void*) * new_capacity);
	else
		items = NULL;

	if (items == NULL)
		return -1;

	l->ob_item = items;
	l->size = newsize;
	l->capacity = new_capacity;
	return 0;
}

//============================ 模块接口 ============================

struct List*
list_create(size_t size, tdListDestroyFun fun)
{
	struct List* l;
	size_t nbytes;
	if (size < 0)
		return NULL;

	if (size > LIST_MAX_SIZE / sizeof(void*))
		return NULL;

	l = (struct List*)malloc(sizeof(*l));
	assert(l != NULL);

	nbytes = size * sizeof(void*);
	if (size <= 0)
		l->ob_item = NULL;
	else
	{
		l->ob_item = (void**)malloc(nbytes);
		assert(l->ob_item != NULL);
		memset(l->ob_item, 0, nbytes);
	}

	l->size = size;
	l->capacity = size;
	l->destory_fun = fun;
	return l;
}

void
list_destory(struct List* l)
{
	if (l == NULL)
		return;
	void** ob_item = l->ob_item;
	if (l->size > 0 && ob_item != NULL)
	{
		for (size_t i = 0; i != l->size; ++i)
			l->destory_fun(ob_item[i]);
		free(ob_item);
	}
	free(l);
}

void*
list_getitem(struct List* l, size_t i)
{
	if (i >= l->size)
	{
		fprintf(stderr, "list out of range");
		return NULL;
	}
	return l->ob_item[i];
}

int
list_setitem(struct List* l, size_t i, void* newitem)
{
	void* olditem;
	if (i >= l->size)
	{
		fprintf(stderr, "list out of range");
		return -1;
	}
	olditem = l->ob_item[i];
	l->destory_fun(olditem);
	l->ob_item[i] = newitem;
	return 0;
}

int
list_append(struct List* l, void *v)
{
	size_t n = l->size;
	//assert (v != NULL);
	if (n == LIST_MAX_SIZE)
	{
		fprintf(stderr, "cannot add more objects to list");
		return -1;
	}
	if (list_resize(l, n + 1) == -1)
		return -1;
	l->ob_item[n] = v;
	return 0;
}

void *
list_pop(struct List* l)
{
	size_t i;
	void *v;
	int status;
	if (l->size == 0)
	{
		/* Special-case most common failure cause */
		fprintf(stderr, "pop from empty list");
		return NULL;
	}
	i = l->size - 1;
	v = l->ob_item[i];
	l->destory_fun(v);
	status = list_resize(l, l->size - 1);
	assert(status >= 0);
	return v;
}

int
list_insert(struct List* l, size_t where, void *v)
{
	size_t i, n = l->size;
	if (v == NULL)
		return -1;

	if (n == LIST_MAX_SIZE)
	{
		fprintf(stderr, "cannot add more objects to list");
		return -1;
	}

	if (list_resize(l, n + 1) == -1)
		return -1;

	if (where > n)
		where = n;
	
	for (i = n; --i >= where; )
		l->ob_item[i + 1] = l->ob_item[i];
	l->ob_item[where] = v;
	return 0;
}

size_t list_size(struct List* l)
{
	return l->size;
}

void list_foreach(struct List* l, tdListForeachFun fun)
{
	for (size_t i = 0; i != l->size; ++i)
		fun(l->ob_item[i]);
}