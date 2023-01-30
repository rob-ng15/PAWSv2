#include <stdio.h>
#include <math.h>
#include <PAWSlibrary.h>

#define NUMBERS 6

float op1[] = { 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
float op2[] = { -5.0f, 4.0f, -3.0f, 2.0f, 1.0f, 0.0f };

int main( void ) {
    printf("Floating Point Tests\n\n");

    for( int i = 0; i < NUMBERS; i++ ) {
        printf("%f + %f = %f | %f - %f = %f\n%f * %f = %f | %f / %f = %f | sqrt(%f) = %f\n\n",
               op1[i], op2[i], op1[i] + op2[i],
               op1[i], op2[i], op1[i] - op2[i],
               op1[i], op2[i], op1[i] * op2[i],
               op1[i], op2[i], op1[i] / op2[i],
               op1[i], sqrtf(op1[i]) );
        fprintf(stderr,"%f + %f = %f | %f - %f = %f\n%f * %f = %f | %f / %f = %f | sqrt(%f) = %f\n\n",
               op1[i], op2[i], op1[i] + op2[i],
               op1[i], op2[i], op1[i] - op2[i],
               op1[i], op2[i], op1[i] * op2[i],
               op1[i], op2[i], op1[i] / op2[i],
               op1[i], sqrtf(op1[i]) );

        sleep1khz( 2000, 0 );
    }

    sleep1khz( 8000, 0 );
}

// EXIT WILL RETURN TO BIOS
