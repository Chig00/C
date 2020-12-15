#include <stdbool.h> // Boolean values
#include <stdlib.h> // Memory allocation

/* A stack data structure for a single element.
   An element of the stack holds arbritrary data and a
     pointer to the previous element of the stack.
 */
typedef struct Stack {
	void* data;
	struct Stack* parent;
} Stack;

/* Returns the length of the stack.
   Does so by counting the number of iterations it takes
     to reach NULL through each stack element's parent member.
   This works because, the parent of the stack's first element
     is NULL.
   NULL represents an empty stack and will return zero.
 */
int stack_length(Stack* stack) {
	int length;
	
	for (length = 0; stack; length++) {
		stack = stack->parent;
	}
	
	return length;
}

/* Pushes data on top of a stack.
   Memory is dynamically allocated for another stack element
     and the new element will store the previous stack as its
	 parent.
   The data passed will be stored in the data member of the stack.
   If the stack passed (the stack, not the pointer to the stack)
     is NULL, then a new stack is produced.
   A pointer to a stack must be passed. A pointer to NULL is
     acceptable, but just passing NULL will cause a segmentation
	 fault.
   If a new stack is to be produced, stack_new() is safer to use,
     than this function.
 */
void stack_push(Stack** stack, void* data) {
	// Memory is allocated for a new stack element
	Stack* new_stack = malloc(sizeof(Stack));
	
	// The new stack element stores the data passed
	//   and the pointer to its parent
	new_stack->data = data;
	new_stack->parent = *stack;
	
	// The stack passed is then changed to point to the last element
	*stack = new_stack;
}

/* Pops data off of the top of the stack.
   Memory that was dynamically allocated to the last stack element
     is freed.
   The stack passed in is changed to the new last element of the stack.
   This function (unlike stack_push()) is safe to use with both NULL
     and pointers to NULL. Both cases will cause this function to
	 return NULL.
 */
void* stack_pop(Stack** stack) {
	// If NULL or a pointer to NULL wasn't passed
	if (stack && *stack) {
		void* data = (*stack)->data; // The data to be returned is stored
		Stack* child = *stack; // The child element to be freed is stored
		*stack = (*stack)->parent; // The pointer now points to the parent
		free(child); // The child is freed
		return data; // The data is returned
	}
	
	return NULL; // If NULL or a pointer to NULL was passed, NULL is returned
}

/* Creates a new stack and returns the pointer to it.
   This is the recommended way to create a new stack, rather
     than using stack_push().
   This function uses stack_push() to produce the new stack.
   If an empty stack is desired, one should simply set any Stack
     pointer to be equal to NULL to get the same effect.
 */
Stack* stack_new(void* data) {
	Stack* new_stack = NULL; // A pointer to NULL is created
	stack_push(&new_stack, data); // The data passed is pushed onto the stack
	
	return new_stack; // The new stack is returned
}

/* Frees all memory allocated to the stack's elements
     and sets the passed pointer to NULL (an empty stack).
   This function uses stack_pop() to empty the stack.
 */
void stack_delete(Stack** stack) {
	// While the stack is non-empty,
	while (*stack) {
		stack_pop(stack); // remove an element of the stack
	}
}

/* An array list data structure for the entire list.
   The array list stores data in a contiguous block
     and stores the number of elements that it holds.
 */
typedef struct ArrayList {
	void** data;
	int count;
} ArrayList;

/* Returns the length of the array list.
   Trivial, but included for consistency with other containers.
 */
int alist_length(ArrayList alist) {
	return alist.count;
}

/* Appends data to the end of the array list.
   Dynamically allocates memory to do so.
 */
void alist_append(ArrayList* alist, void* data) {
	// The data block is extended
	alist->data = realloc(alist->data, (alist->count + 1) * sizeof(void*));
	// The new data is stored at the end of the array list
	//   and the count of elements in incremented
	alist->data[alist->count++] = data;
}

/* Returns the data entry at the desired index.
   Trivial, but included for consistency with other containers.
 */
void* alist_access(ArrayList alist, int index) {
	return alist.data[index];
}

/* Returns an array list with members initialised.
   Allows for data passed to be stored at index zero.
 */
ArrayList alist_new(void* data) {
	ArrayList alist = {
		.data = NULL,
		.count = 0
	};
	
	alist_append(&alist, data);
	
	return alist;
}

/* Inserts data into the specified index.
   Not safe for use when the index exceeds the array list's length.
 */
void alist_insert(ArrayList* alist, void* data, int index) {
	// The data block is extended
	alist->data = realloc(alist->data, (alist->count + 1) * sizeof(void*));
	
	// The data at and behind the index are shifted back one position
	for (int i = alist->count; i > index; i--) {
		alist->data[i] = alist->data[i - 1];
	}
	
	// The data is inserted and the data count is incremented
	alist->data[index] = data;
	alist->count++;
}

/* Removes data from the specified index and frees allocated memory.
   Not safe for use if the index is out of range.
 */
void alist_remove(ArrayList* alist, int index) {
	// The data behind the index are shifted forward one position
	for (int i = index; i < alist->count; i++) {
		alist->data[i] = alist->data[i + 1];
	}
	
	// The data block is shortened and the count is decremented
	alist->data = realloc(alist->data, (--alist->count) * sizeof(void*));
}

/* Removes all of the elements from the array list and frees memory
     allocated to the array list.
   Uses alist_remove() to do so.
 */
void alist_delete(ArrayList* alist) {
	// While the array list contains elements, remove the first one.
	while (alist->count) {
		alist_remove(alist, 0);
	}
}

/* A linked list data structure for a single element.
   A linked list element holds arbitrary data and information
     to find the next element in the linked list.
   NULL represents an empty linked list.
 */
typedef struct LinkedList {
	void* data;
	struct LinkedList* next;
} LinkedList;

/* Returns the length of the linked list.
   NULL (the empty linked list) is of length zero.
 */
int llist_length(LinkedList* llist) {
	int length; // Length is declared outside of the loop for scope
	
	// Loop incrementing length, until NULL is reached
	for (length = 0; llist; length++) {
		llist = llist->next;
	}
	
	return length; // The length is returned (length is still in scope)
}

/* Appends the data passed to the dnd of the linked list.
   Passing a pointer to NULL (not NULL itself), will produce a
     new linked list, but this is not recommended.
   Using llist_new() is recommended to produce new linked lists.
 */
void llist_append(LinkedList** llist, void* data) {
	// If NULL was not passed
	if (llist) {
		// If a pointer to NULL was not passed (non-empty linked list)
		if (*llist) {
			// The original head of the list is stored
			LinkedList* head = *llist;
			
			// While the linked list element is not the last one,
			//   it becomes the next one
			while ((*llist)->next) {
				*llist = (*llist)->next;
			}
			
			// The memory for another linked list element is allocated
			(*llist)->next = malloc(sizeof(LinkedList));
			(*llist)->next->data = data; // The data is stored
			(*llist)->next->next = NULL; // This is the new last element
			
			*llist = head; // The list returns to index 0
		}
		
		// Else, an empty list was passed
		else {
			// The memory for a linked list element is allocated
			*llist = malloc(sizeof(LinkedList));
			(*llist)->data = data; // The data passed is storeed
			(*llist)->next = NULL; // This is the last element of the list
		}
	}
}

/* Returns the data at the specified index.
 */
void* llist_access(LinkedList* llist, int index) {
	for (int i = 0; i < index; i++) {
		llist = llist->next;
	}
	
	return llist->data;
}

/* Returns a new linked list and stores passed data.
   This is the recommended way to make new linked lists.
 */
LinkedList* llist_new(void* data) {
	// The new linked list is initialised empty
	LinkedList* new_llist = NULL;
	// Data is appended to the end of the linked list
	llist_append(&new_llist, data);
	
	// The new linked list is returned
	return new_llist;
}

/* Inserts data into a specified index in the linked list.
   All indexes at or after are shifted back one position.
   Works with empty linked lists (NULL).
 */
void llist_insert(LinkedList** llist, void* data, int index) {
	// If not NULL
	if (llist) {
		// If non-empty
		if (*llist) {
			// Index zero is stored
			LinkedList* head = *llist;
			
			// The index is iterated to
			for (int i = 0; i < index; i++) {
				(*llist) = (*llist)->next;
			}
			
			// The linked list is extended with arbitrary data
			llist_append(llist, NULL);
			
			// The insertion index is stored
			LinkedList* base = *llist;
			
			// Each element is shifted back an index
			for (int i = llist_length(*llist) - 2; i > -1; i--) {
				
				// The last unmoved element is iterated to
				for (int j = 0; j < i; j++) {
					*llist = (*llist)->next;
				}
				
				// Data is copied forwards
				(*llist)->next->data = (*llist)->data;
				
				// The list is returned back to the insertion index
				*llist = base;
			}
			
			// The data is inserted
			base->data = data;
			
			// Index zero is returned to
			*llist = head;
		}
		
		// If empty and the insertion index is zero, the data is appended
		else if (!index) {
			llist_append(llist, data);
		}
	}
}

/* Removes data from the specified index in the linked list.
   Frees allocated memory for a linked list element.
   Does not free memory allocated to data.
 */
void llist_remove(LinkedList** llist, int index) {
	// If not NULL and not empty
	if (llist && *llist) {
		LinkedList* head = *llist; // Index zero is stored
			
		// The element to be removed is iterated to
		for (int i = 0; i < index; i++) {
			*llist = (*llist)->next;
		}
		
		LinkedList* tail = NULL; // The new final element
		
		// The subsequent elements' data are copied forwards
		while ((*llist)->next) {
			// If this is the penultimate element, it will become the last one
			if (!(*llist)->next->next) {
				tail = *llist;
			}
			
			(*llist)->data = (*llist)->next->data;
			*llist = (*llist)->next;
		}
		
		// The pointer to the last element to be freed is stored
		LinkedList* to_free = *llist;
		
		// If the list had more than one element
		if (tail) {
			// The tail becomes the new final element
			tail->next = NULL;
			// The linked list returns to index zero
			*llist = head;
		}
		
		// Else, the linked list is now empty
		else {
			*llist = NULL;
		}
		
		// The element to be free is freed
		free(to_free);
	}
}

/* Deletes a linked list and frees all memory allocated to its elements.
   An empty linked list will remain (NULL).
 */
void llist_delete(LinkedList** llist) {
	// While the linked list is not NULL and not empty
	while (llist && *llist) {
		llist_remove(llist, 0);
	}
}