#include <stdio.h>
#include <stdlib.h> 
#include <string.h>


int main() {

    int number = 0;

    while (1) { 

        printf("Enter a number: ");
        
        if (scanf("%d", &number) == 1){
            printf("Updated number: %d\n", number); 

        } else{
            while (getchar() != '\n');
            printf("Wrong input!"); 
        }
    }

    return 0;
}
