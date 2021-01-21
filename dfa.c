#include <stdio.h>
#include <stdlib.h>

// A structure representing a state.
typedef struct State {
    int bit; // The bit returned if the program terminates with this state.
    int zero_state; // The state to advance to if zero is read.
    int one_state; // The state to advance to if one is read.
} State;

// The maximum length of an input string.
const int MAX_INPUT = 1000;

// The starting state index.
const int INITIAL_STATE = 0;

/*  A deterministic finite-state automata simulator.

    Creates a DFA from the given file.
    Prompts the user for any number of binary strings.
    Returns a bit for each string according to the DFA given.
   
    The format of a DFA source file is as follows:
        [number of states]
        [0 or 1] [state if 0 is read] [state if 1 is read]
        ...
        [0 or 1] [state if 0 is read] [state if 1 is read]
    
    The set of states is the range [0, state_count).
    The language is binary ({0, 1}).
    The initial state is 0.
    The accepted state set returns 1.

    For each state, 0 or 1 represents the returned
     bit if the execution terminates at that state.
 */
int main(int argc, char** argv) {
    // A single source file must be given.
    if (argc != 2) {
        fprintf(stderr, "A single source file must be given.\n");
        return EXIT_FAILURE;
    }
    
    // An attempt is made to open the source file.
    FILE* source = fopen(argv[1], "r");
    
    // The source file could not be opened.
    if (!source) {
        perror("Error");
        return EXIT_FAILURE;
    }
    
    // The number of states in the DFA.
    int state_count;
    
    // The number of states are found.
    fscanf(source, "%d", &state_count);
    
    // An attempt is made to allocate memory for the states.
    State* states = malloc(state_count * sizeof(State));
    
    // Memory could not be allocated for the states.
    if (!states) {
        fprintf(stderr, "Insufficient memory for the states.\n");
        return EXIT_FAILURE;
    }
    
    // The states are initialised according to the source file.
    for (int i = 0; i < state_count; ++i) {
        // The bit, zero transition, and one transition are extracted.
        fscanf(source, "%d%d%d", &states[i].bit, &states[i].zero_state, &states[i].one_state);
    }
    
    // A buffer to store an input string.
    char* string = malloc(MAX_INPUT);
    
    // Memory could not be allocated for the input string.
    if (!string) {
        fprintf(stderr, "Insufficient memory for the input string.\n");
        free(states);
        return EXIT_FAILURE;
    }
    
    // Instructions to terminate the input loop are given.
    printf("Input q to terminate.\n");
    
    // Loop to continuously prompt for input.
    while (1) {
        // The current state of the DFA.
        int state = INITIAL_STATE;
        
        // Input is prompted for.
        printf("\nInput: ");
        
        // The input is taken.
        if (fgets(string, MAX_INPUT, stdin)) {
            // The user chose to quit.
            if (string[0] == 'q' || string[0] == 'Q') {
                printf("\nTerminating.\n");
                break;
            }
            
            // The string of inputs is iterated through.
            for (int i = 0; string[i] != '\0'; ++i) {
                // A zero transition is performed.
                if (string[i] == '0') {
                    state = states[state].zero_state;
                }
                
                // A one transition is performed.
                else if (string[i] == '1') {
                    state = states[state].one_state;
                }
            }
            
            printf("Output: %d\n", states[state].bit);
        }
        
        // Input could not be read.
        else {
            fprintf(stderr, "The input string could not be read.\n");
            free(string);
            free(states);
            return EXIT_FAILURE;
        }
    }
    
    // The memory dynamically allocated is deallocated.
    free(string);
    free(states);
    
    // The program terminated correctly.
    return EXIT_SUCCESS;
}