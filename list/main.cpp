#include "list.h"
#include <iostream>

void des(void* a)
{
	std::cout << "destroy: " << a << std::endl;
}

void fe_cb(void* a)
{
	std::cout << "foreach: " << a << std::endl;
}

int main()
{
	struct List* l = list_create(0, des);
	list_append(l, (void*)1);
	std::cout << "list size: " << list_size(l) << std::endl;

	list_append(l, (void*)2);
	list_append(l, (void*)3);
	list_append(l, (void*)4);

	std::cout << "list size: " << list_size(l) << std::endl;

	list_foreach(l, fe_cb);
	list_destory(l);
	return 0;
}