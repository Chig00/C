#include <stdio.h> // Output
#include "datastructs.h" // Data structures

/* A test program to show the capabilities of different
     data structures in action.
 */
int main() {
	// Some data to be stored
	int data[] = {3, 21, 5, -6, 100, -5497, 5};
	
	// Stack section
	printf("Stack\n\n");
	
	// The stack is made and the data is stored
	Stack* stack = stack_new(data);
	
	for (int i = 1; i < 7; i++) {
		stack_push(&stack, data + i);
	}
	
	// The length and memory location of the stack is displayed
	printf("%d %x\n", stack_length(stack), stack);
	
	printf("\n"); // Separator for clarity
	
	// The stack's elements are popped off
	// Their values, the length, and the memory location
	//   of the stack are displayed
	for (int i = 0; i < 7; i++) {
		printf("%d ", *(int*)stack_pop(&stack));
		printf("%d ", stack_length(stack));
		printf("%x\n", stack);
	}
	
	printf("\n"); // Separator for clarity
	
	// The data is pushed back onto the stack (in reverse order this time)
	for (int i = 6; i > -1; i--) {
		stack_push(&stack, data + i);
	}
	
	// The stack is deleted and its memory location is displayed
	//   before and after the deletion
	printf("%x\n", stack);
	stack_delete(&stack);
	printf("%x\n", stack);
	
	printf("\n"); // Separator for clarity
	
	// Array list section
	printf("Array List\n\n");
	
	// The array list is made and the data is stored
	ArrayList alist = alist_new(data);
	
	for (int i = 1; i < 7; i++) {
		alist_append(&alist, data + i);
	}
	
	// The length of the array list is displayed
	printf("%d\n", alist_length(alist));
	
	printf("\n"); // Separator for clarity
	
	// The contents of the array list are displayed
	for (int i = 0; i < alist_length(alist); i++) {
		printf("%d\n", *(int*)alist_access(alist, i));
	}
	
	printf("\n"); // Separator for clarity
	
	// The five middle elements are removed and the two elements on the
	//   end are inserted into the middle
	for (int i = 0; i < 5; i++) {
		alist_remove(&alist, 1);
	}
	
	alist_insert(&alist, alist_access(alist, 0), 1);
	alist_insert(&alist, alist_access(alist, 2), 1);
	
	// The contents of the array list are displayed
	for (int i = 0; i < alist_length(alist); i++) {
		printf("%d\n", *(int*)alist_access(alist, i));
	}
	
	printf("\n"); // Separator for clarity
	
	// The array list is deleted and its length is displyed
	alist_delete(&alist);
	printf("%d\n\n", alist_length(alist));
	
	// Linked List section
	printf("Linked List\n\n");
	
	// The Linked List is initialised and the data is stored
	LinkedList* llist = llist_new(data);
	
	for (int i = 1; i < 7; i++) {
		llist_append(&llist, data + i);
	}
	
	// The length and data of the linked list are displayed
	printf("%d\n\n", llist_length(llist));
	
	for (int i = 0; i < llist_length(llist); i++) {
		printf("%d\n", *(int*)llist_access(llist, i));
	}
	
	printf("\n"); // Separator for clarity
	
	// The five middle elements are removed and the two elements on the
	//   end are inserted into the middle
	for (int i = 0; i < 5; i++) {
		llist_remove(&llist, 1);
	}
	
	// The contents of the linked list are displayed
	for (int i = 0; i < llist_length(llist); i++) {
		printf("%d\n", *(int*)llist_access(llist, i));
	}
	
	printf("\n"); // Separator for clarity
	
	llist_insert(&llist, llist_access(llist, 0), 1);
	llist_insert(&llist, llist_access(llist, 2), 1);
	
	// The contents of the linked list are displayed
	for (int i = 0; i < llist_length(llist); i++) {
		printf("%d\n", *(int*)llist_access(llist, i));
	}
	
	printf("\n"); // Separator for clarity
	
	// The linked list is deleted and its length is displyed
	llist_delete(&llist);
	printf("%d\n", llist_length(llist));
}