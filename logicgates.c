#include <stdio.h>
#include <stdbool.h>

/* a b a nand b
   f f t
   f t t
   t f t
   t t f
 */
// The most basic logic gate - all other logic gates can be made from a combination of this gate
bool nand(bool a, bool b) {
	return !(a && b);
}

/* a not a
   f t
   t f
 */
// Feeding the same input into the inputs for a NAND logic gate creates NOT
bool not(bool a) {
	return nand(a, a);
}

/* a b a and b
   f f f
   f t f
   t f f
   t t t
 */
// AND is NOT (A NAND B)
bool and(bool a, bool b) {
	return not(nand(a, b));
}

/* a b a or b
   f f f
   f t t
   t f t
   t t t
 */
// OR is (NOT A) NAND (NOT B)
bool or(bool a, bool b) {
	return nand(not(a), not(b));
}

/* a b a or b
   f f t
   f t f
   t f f
   t t f
 */
// NOR is NOT (A OR B)
bool nor(bool a, bool b) {
	return not(or(a, b));
}

/* a b a xor b
   f f f
   f t t
   t f t
   t t f
 */
// XOR is ((NOT A) NAND B) NAND (A NAND (NOT B))
bool xor(bool a, bool b) {
	return nand(nand(not(a), b), nand(a, not(b)));
}

// Tests the gate inputted
void test_gate(bool (*gate)(bool a, bool b)) {
	printf("A B RESULT\n");
	printf("F F %c\n", gate(false, false) ? 'T' : 'F');
	printf("F T %c\n", gate(false, true) ? 'T' : 'F');
	printf("T F %c\n", gate(true, false) ? 'T' : 'F');
	printf("T T %c\n", gate(true, true) ? 'T' : 'F');
	printf("\n");
}

int main() {
	// Test for the NAND gate
	printf("NAND:\n");
	test_gate(nand);
	
	// Test for the NOT gate (done manually as NOT only takes one input)
	printf("NOT:\n");
	printf("A RESULT\n");
	printf("F %c\n", not(false) ? 'T' : 'F');
	printf("T %c\n", not(true) ? 'T' : 'F');
	printf("\n");
	
	// Test for the AND gate
	printf("AND:\n");
	test_gate(and);
	
	// Test for the OR gate
	printf("OR:\n");
	test_gate(or);
	
	//Test for the NOR gate
	printf("NOR:\n");
	test_gate(nor);
	
	// Test for the XOR gate
	printf("XOR:\n");
	test_gate(xor);
}