#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// A heterogenous dynamically sized list
typedef struct {
	void* items;
	int* sizes;
	int count;
} List;

// An structure used to demonstrate the list's storage capabilities
typedef struct {
	char c;
	int i;
	double f;
} Object;

// Stores a single variable of any type
void store(List* list, void* item, int size) {
	// The old size of the list is assumed to be zero
	int old_size = 0;
	
	// The list's old size is calculated
	for (int i = 0; i < list->count; i++) {
		old_size += list->sizes[i];
	}
	
	// The list's items and sizes buffer are resized to fit the new data
	list->items = realloc(list->items, old_size + size);
	list->sizes = realloc(list->sizes, sizeof(int) * (list->count + 1));
	
	// The addresses where the new item and its size will be stored
	void* item_address = list->items + old_size;
	int* size_address = list->sizes + list->count;
	
	// The item is copied into the list and its size is recorded
	memcpy(item_address, item, size);
	*size_address = size;
	
	// The number of items in the list is incremented
	list->count++;
}

// Return a pointer to a list item
void* retrieve(List list, int index) {
	// The list's items are iterated through until the desired item is gotten
	for (int i = 0; i < list.count; i++) {
		if (i == index) {
			return list.items;
		}
		
		list.items += list.sizes[i];
	}
	
	// If no item is found, a NULL pointer is returned
	return NULL;
}

// Discard an item from the list
void discard(List* list, int index) {
	// A copy of the address for the first item is made
	void* item_address = list->items;
	
	// The address for the item to be discarded is calculated
	for (int i = 0; i < list->count; i++) {
		// If the item to be discarded is found then it is discarded
		if (i == index) {
			// The original address and the distance from it are stored
			void* original_address = item_address;
			int displacement = 0;
			
			// The data from subsequent items is copied into the discarded item's address
			for (int j = i; j < list->count - 1; j++) {
				displacement += list->sizes[j];
				// The next item is copied into the previous item
				memcpy(item_address, original_address + displacement, list->sizes[j + 1]);
				// The next size is copeid into the previous size
				list->sizes[j] = list->sizes[j + 1];
				// The address of the next item to copy is obtained
				item_address += list->sizes[j];
			}
			
			// The new total size of the list is calculated
			int total_size = 0;
			
			for (int j = 0; j < list->count - 1; j++) {
				total_size += list->sizes[j];
			}
			
			// The list's items and sizes are resized and the count of items is decremented
			list->items = realloc(list->items, total_size);
			list->sizes = realloc(list->sizes, sizeof(int) * --list->count);
			
			// The function is terminated after discarding the item
			break;
		}
		
		item_address += list->sizes[i];
	}
}

int main() {
	// The list is created and initialised
	List list = {0};
	
	// Some variables are created
	char c = 'a';
	int i = 1;
	double f = 3.14;
	Object o = {.c = 'z', .i = -1, .f = 2.72};
	
	// The variables are stored
	store(&list, &c, sizeof(c));
	store(&list, &i, sizeof(i));
	store(&list, &f, sizeof(f));
	store(&list, &o, sizeof(o));
	
	// The variables are retrieved and shown to retain their previous values
	printf(
		"%c\n%d\n%f\n%c %d %f\n",
		*((char*) retrieve(list, 0)), *((int*) retrieve(list, 1)), *((double*) retrieve(list, 2)),
		((Object*) retrieve(list, 3))->c, ((Object*) retrieve(list, 3))->i, ((Object*) retrieve(list, 3))->f
	);
	
	// All variables but one are discarded
	discard(&list, 0);
	discard(&list, 0);
	discard(&list, 0);
	
	// The variable left is show to retain its previous value
	printf("%c %d %f\n", ((Object*) retrieve(list, 0))->c, ((Object*) retrieve(list, 0))->i, ((Object*) retrieve(list, 0))->f);
	
	// Some further examples
	store(&list, &c, sizeof(c));
	store(&list, &c, sizeof(c));
	discard(&list, 1);
	
	printf("From %c to %c!\n", *((char*) retrieve(list, 1)), ((Object*) retrieve(list, 0))->c);
}