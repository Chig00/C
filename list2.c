#include <stdio.h> // printf()
#include <stdlib.h> // realloc()

// A heterogenous, dynamically sized, storage structure
typedef struct {
	void** items;
	int count;
} List;

// An abitrary structure to demonstrate the list's storage capabilities
typedef struct {
	char c;
	int i;
	double f;
} Structure;

// Constants to demonstrate the list' storage capabilities
const char CHAR = 'c';
const int INT = 9;
const double FLOAT = 6.12;
const char CHAR2 = 'C';
const int INT2 = -9;
const double FLOAT2 = -6.12;

void store(List* list, void* item) {
	// The memory needed to store the pointer is allocated
	list->items = realloc(list->items, sizeof(void*) * (list->count + 1));
	// The pointer is stored and the count of items is incremented
	list->items[list->count++] = item;
}

void* retrieve(List list, int index) {
	// The desired item's pointer is returned
	return list.items[index];
}

void discard(List* list, int index) {
	// An item is discarded by replacing it by subsequent items
	for (int i = index; i < list->count - 1; i++) {
		list->items[i] = list->items[i + 1];
	}
	
	// The items' buffer is resized and the count is decremented
	list->items = realloc(list->items, sizeof(void*) * --list->count);
}

int main() {
	// A list is created and initialised
	List list = {0};
	// Some other variables for the list to store are initialised
	char c = CHAR;
	int i = INT;
	double f = FLOAT;
	Structure s = {
		.c = CHAR2,
		.i = INT2,
		.f = FLOAT2
	};
	
	// The variables are stored in the list
	store(&list, &c);
	store(&list, &i);
	store(&list, &f);
	store(&list, &s);
	
	// The list's items are shown to have kept their data
	printf(
		"%c = %c\n%d = %d\n%f = %f\n%c = %c\n%d = %d\n%f = %f\n",
		c, *((char*) retrieve(list, 0)),
		i, *((int*) retrieve(list, 1)),
		f, *((double*) retrieve(list, 2)),
		s.c, ((Structure*) retrieve(list, 3))->c,
		s.i, ((Structure*) retrieve(list, 3))->i,
		s.f, ((Structure*) retrieve(list, 3))->f
	);
	
	// The list's items are shown to be references rather than copies of value
	c++;
	i--;
	f++;
	s.c--;
	s.i++;
	s.f--;
	
	printf(
		"%c = %c\n%d = %d\n%f = %f\n%c = %c\n%d = %d\n%f = %f\n",
		c, *((char*) retrieve(list, 0)),
		i, *((int*) retrieve(list, 1)),
		f, *((double*) retrieve(list, 2)),
		s.c, ((Structure*) retrieve(list, 3))->c,
		s.i, ((Structure*) retrieve(list, 3))->i,
		s.f, ((Structure*) retrieve(list, 3))->f
	);
	
	// The list's deletion capabilities are displayed
	discard(&list, 0);
	discard(&list, 1);
	discard(&list, 0);
	store(&list, &s);
	
	printf(
		"%c = %c\n%d = %d\n%f = %f\n%c = %c\n%d = %d\n%f = %f\n",
		s.c, ((Structure*) retrieve(list, 0))->c,
		s.i, ((Structure*) retrieve(list, 1))->i,
		s.f, ((Structure*) retrieve(list, 0))->f,
		s.c, ((Structure*) retrieve(list, 1))->c,
		s.i, ((Structure*) retrieve(list, 0))->i,
		s.f, ((Structure*) retrieve(list, 1))->f
	);
	
	// The list's ability to store itself is displayed
	store(&list, &list);
	discard(&list, 1);
	
	printf(
		"%c = %c\n%d = %d\n%f = %f\n%c = %c\n%d = %d\n%f = %f\n",
		s.c, ((Structure*) retrieve(*((List*) retrieve(list, 1)), 0))->c,
		s.i, ((Structure*) retrieve(*((List*) retrieve(list, 1)), 0))->i,
		s.f, ((Structure*) retrieve(*((List*) retrieve(list, 1)), 0))->f,
		s.c, ((Structure*) retrieve(*((List*) retrieve(*((List*) retrieve(list, 1)), 1)), 0))->c,
		s.i, ((Structure*) retrieve(*((List*) retrieve(*((List*) retrieve(list, 1)), 1)), 0))->i,
		s.f, ((Structure*) retrieve(*((List*) retrieve(*((List*) retrieve(list, 1)), 1)), 0))->f
	);
}