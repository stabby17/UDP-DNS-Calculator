#include "Validation.h"

// Check the length of the string
int lenght_check(int length)
{
	if(length>BUFFER_SIZE)
    {
        printf("The string is too long, please enter a string with a maximum of %d characters\n\n", BUFFER_SIZE);
        return 1;
    }
    return 0;
}

// Check if the operator is correct
int operator_check(char* str)
{
	if (strchr("+-xX*/", str[0]) == NULL) // First token is not an operand
	{
		printf("An incorrect operator was entered, check the format\n\n");
		return 1;
	}
	return 0;
}

// Check if the operands (numbers) are correct
int num_check(char* str)
{
	for (unsigned int i = 0; i < strlen(str); i++)
	{
		if (!isdigit(str[i]))
		{
			printf("The operands entered are not in the correct format.\n\n");
			if (str[i] == ' ')
				printf("Please ensure there are no spaces after the last character.\n\n");

			return 1;
		}
	}

	return 0;
}

// Check if the string is in the correct format
// Returns 1 if incorrect, 0 if correct
// Performs the following checks: operator_check, num_check
int string_check(const char* input_string)
{
	char str[BUFFER_SIZE];
	strcpy(str, input_string);

	if (!strcmp(str, "=") == 0)
	{
		char* token = strtok(str, " ");
		unsigned int check_op_number = 0;
		for (unsigned int i = 1; i <= 3 && token != NULL; i++)
		{
			switch (i)
			{
				case 1:
					if (operator_check(token)) // Check if the first token is a valid operator
						return 1;
					token = strtok(NULL, " ");
					break;

				case 2:
					if (num_check(token)) // Check if the second token is a valid number
						return 1;
					token = strtok(NULL, "\n");
					break;

				case 3:
					if (num_check(token)) // Check if the third token is a valid number
						return 1;
					check_op_number = 1;
					break;
			}
		}
		if (check_op_number == 0) // Less than two operands have been entered
		{
			printf("Fewer than two operands have been entered\n\n");
			return 1;
		}
	}

	return 0;
}