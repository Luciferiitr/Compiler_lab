#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void main()
{
    char input[100];
    char c;
    int state = 1;
    int i = 0;
    printf("Enter a string: ");
    scanf("%s", input);

    while (input[i] != '\0')
    {

        switch (state)
        {
        case 1:
            c = input[i++];
            if (c == 'a')
                state = 2;
            else if (c == 'b')
                state = 3;
            else
                state = 0;
            break;
        case 2:
            c = input[i++];
            if (c == 'a')
                state = 7;
            else if (c == 'b')
                state = 5;
            else
                state = 0;
            break;
        case 3:
            c = input[i++];
            if (c == 'a')
                state = 4;
            else if (c == 'b')
                state = 3;
            else
                state = 0;
            break;
        case 4:
            printf("Output: %s is not recognised by any pattern.\n", input);
            exit(0);
        case 5:
            c = input[i++];
            if (c == 'a')
                state = 4;
            else if (c == 'b')
                state = 6;
            else
                state = 0;
            break;
        case 6:
            c = input[i++];
            if (c == 'a')
                state = 4;
            else if (c == 'b')
                state = 8;
            else
                state = 0;
            break;
        case 7:
            c = input[i++];
            if (c == 'a')
                state = 7;
            else if (c == 'b')
                state = 8;
            else
                state = 0;
            break;
        case 8:
            c = input[i++];
            if (c == 'a')
                state = 4;
            else if (c == 'b')
                state = 8;
            else
                state = 0;
            break;
        default:
            printf("Output: %s is not recognised by any pattern.\n", input);
            exit(0);
        }
    }

    if ((state == 1) || (state == 2) || (state == 7))
        printf("Output: %s is accepted under rule 'a*'\n", input);
    else if ((state == 3) || (state == 5) || (state == 8))
        printf("Output: %s is accepted under rule 'a*b+'\n", input);
    else if (state == 6)
        printf("Output: %s is accepted under rule 'abb'\n", input);
}