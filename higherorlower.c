#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MINIMUM 0
#define MAXIMUM 10
#define TRUE 1

int rand_num () {
    /* The number chosen ranges from the MINIMUM
       constant to the MAXIMUM constant. */
    int number = (MINIMUM + rand() % (MAXIMUM + 1));
    return number;
}

int main () {
    // RNG is seeded.
    srand(time(NULL));

    // The first number is randomly assigned.
    int number = rand_num();
    // The score starts at 0.
    int score = 0;

    // The guess and new_number are set in the game loop.
    int guess;
    int new_number;

    while (TRUE) {
        printf("The number is %d.\n", number);
        printf("Higher (1) or lower (0)?\n");
        scanf("%d", &guess);

        while (TRUE) {
            /* The new _number will not be accepted unless
               it is different from the old number. */
            new_number = rand_num();
            if (new_number != number) {
                break;
            }
        }

        printf("The new number is %d!\n", new_number);

        /* If the guess is correct, the player gets a point.
           If not, the game ends. */
        if (guess) {
            if (new_number > number) {
                printf("You were correct!\n");
                printf("The new number is higher!\n");
                printf("You get a point!\n");
                score++;
            }

            else {
                printf("You were incorrect...\n");
                printf("The new number is lower...\n");
                break;
            }
        }

        else {
            if (new_number < number) {
                printf("You were correct!\n");
                printf("The new number is lower!\n");
                printf("You get a point!\n");
                score++;
            }

            else {
                printf("You were incorrect...\n");
                printf("The new number is higher...\n");
                break;
            }
        }

        // The number for the next loop is the new number.
        number = new_number;
    }

    // Game end announcement and score display.
    printf("GAME OVER!\n");
    printf("Your score is %d\n", score);
}
