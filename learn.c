#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

// Structure to store data to make predictions
typedef struct {
	double* inputs;
	double* outputs;
	int count;
} Data;

// Returns a PRNG number in the defined range
double random(double min, double max) {
	return min + (max - min) * rand() / RAND_MAX;
}

// Function that the program attempts to predict
double function(double input) {
	return sin(cos(input * input));
}

// Data can be stored to increase the program's prediction accuracy
void feed(Data* data, double (*function)(double), double input) {
	// If the input is a duplicate, it is ignored
	for (int i = 0; i < data->count; i++) {
		if (input == data->inputs[i]) {
			return;
		}
	}
	
	// The input and output buffers are resized to accomodate the incoming data
	// When the buffers are empty, they are allocated to in the standard fashion
	if (data->count == 0) {
		data->inputs = malloc(sizeof(double));
		data->outputs = malloc(sizeof(double));
	}
	
	else {
		// The resizing is done by powers of two to increase efficiency at the cost of memory
		double two_power;
		for (int i = 0; (two_power = pow(2, i)) <= data->count; i++) {
			// If the number of input-output pairs is a power of two, the buffers are doubled in size
			if (two_power == data->count) {
				data->inputs = realloc(data->inputs, 2 * data->count * sizeof(double));
				data->outputs = realloc(data->outputs, 2 * data->count * sizeof(double));
			}
		}
	}
	
	// The location for data insertion is calculated to maintain the order of inputs
	int index = data->count;
	
	for (int i = 0; i < data->count; i++) {
		if (input < data->inputs[i]) {
			for (int j = data->count; j > i; j--) {
				data->inputs[j] = data->inputs[j - 1];
				data->outputs[j] = data->outputs[j - 1];
			}
			
			index = i;
			break;
		}
	}
	
	// The data is stored in the position that maintains order
	data->inputs[index] = input;
	data->outputs[index] = function(input);
	
	// The number of input-output pairs is incremented
	data->count++;
}

// Predicts an outputs based on fed input
double predict(Data data, double input) {
	// A linear regression from the outmost data points is used to predict data outside of the range of what was given
	if (input < data.inputs[0]) {
		return (data.outputs[1] - data.outputs[0]) * (input - data.inputs[0]) / (data.inputs[1] - data.inputs[0]) + data.outputs[0];
	}
	
	else if (input > data.inputs[data.count - 1]) {
		return (data.outputs[data.count - 2] - data.outputs[data.count - 1]) * (input - data.inputs[data.count - 1]) / (data.inputs[data.count - 2] - data.inputs[data.count - 1]) + data.outputs[data.count - 1];
	}
	
	// Finds where the data best fits to make a prediction
	for (int i = 0; i < data.count; i++) {
		// If the input is an exact match, the correct input is returned
		if (input == data.inputs[i]) {
			return data.outputs[i];
		}
		
		// Sees if the input fits between two data points and checks that non-program data will not be accessed (due to the i + 1 access)
		else if (i < data.count - 1 && data.inputs[i] < input && input < data.inputs[i + 1]) {
			// The prediction is based on the proximity to the predefined data points (assumed a linear relationship for this)
			return (data.outputs[i + 1] - data.outputs[i]) * (input - data.inputs[i]) / (data.inputs[i + 1] - data.inputs[i]) + data.outputs[i];
		}
	}
}

int main(int argc, char* argv) {
	// Data structure is created and initialised
	Data data = {0};
	
	// Loop for input
	while (true) {
		// User is given the option to let the program 'feed' on data to 'learn', use its knowledge to predict, display its current data, or quit
		printf("Feed (f), predict (p), display (d) or quit (q)? ");
		char option;
		scanf("%c", &option);
		// Used to clear stdin
		while (getchar() != '\n');
		
		// User chose to feed the program data
		if (option == 'f') {
			// The user feeds data in a range with intervals between each data point
			printf("Feed range and interval (min max step): ");
			double range[3];
			scanf("%lf%lf%lf", &range[0], &range[1], &range[2]);
			while (getchar() != '\n');
			
			// Each data point is systematically fed on by the program
			for (double i = range[0]; i <= range[1]; i += range[2]) {
				feed(&data, function, i);
				
				// If the interval is zero, the feeding ends to prevent an infinite loop
				if (range[2] == 0) {
					break;
				}
			}
		}
		
		// User chose to make a prediction using the program data
		else if (option == 'p') {
			while (true) {
				// The user has an option to use random input or their own
				printf("Random input (y/n) or go back (q)? ");
				scanf("%c", &option);
				while (getchar() != '\n');
				
				if (option == 'y') {
					// The user choses the number of random inputs and their range
					printf("Number and range of inputs (num min max): ");
					int number;
					double range[2];
					scanf("%d%lf%lf", &number, &range[0], &range[1]);
					while (getchar() != '\n');
					
					// The program predicts the output based on its data and compares the value to the actual value
					for (int i = 0; i < number; i++) {
						double input = random(range[0], range[1]);
						double prediction = predict(data, input);
						double actual = function(input);
						printf("Input: %f\nPrediction: %f\nActual: %f\nPercentage Error: %f%c\n", input, prediction, actual, fabs(100 * (prediction - actual) / actual), '%');
					}
				}
				
				else if (option == 'n') {
					printf("Input range and interval (min max step): ");
					double range[3];
					scanf("%lf%lf%lf", &range[0], &range[1], &range[2]);
					while (getchar() != '\n');
					
					for (double i = range[0]; i <= range[1]; i += range[2]) {
						double prediction = predict(data, i);
						double actual = function(i);
						printf("Input: %f\nPrediction: %f\nActual: %f\nPercentage Error: %f%c\n", i, prediction, actual, fabs(100 * (prediction - actual) / actual), '%');
						
						if (range[2] == 0) {
							break;
						}
					}
				}
				
				// If the user chooses to go back they do so
				else if (option == 'q') {
					break;
				}
				
				// If the user inputs invalid input, they are reprompted
				else {
					printf("Invalid input.\n");
				}
			}
		}
		
		// All of the data stored can be viewed
		else if (option == 'd') {
			for (int i = 0; i < data.count; i++) {
				printf("Input: %f\nOutput: %f\n", data.inputs[i], data.outputs[i]);
			}
		}
		
		// The program can be ended
		else if (option == 'q') {
			break;
		}
		
		// The user is prompted for input if their previous input was invalid
		else {
			printf("Invalid input.\n");
		}
	}
	
	free(data.inputs);
	free(data.outputs);
}