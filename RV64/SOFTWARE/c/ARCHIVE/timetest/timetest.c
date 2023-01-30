#include <stdio.h>
#include <PAWSlibrary.h>

int main( void ) {
    struct paws_timeval tp;
    unsigned short      seconds;
    unsigned int        milliseconds;

    paws_gettimeofday(&tp, NULL);
    printf("sizeof timeval = %0d bytes\n\n",sizeof(struct paws_timeval));

    seconds = *SYSTEMSECONDS;
    milliseconds = *SYSTEMMILLISECONDS;
    printf("seconds = %0d, milliseconds = %0d\n\n",seconds,milliseconds);

    printf("paws_gettimeofday seconds = %0d, milliseconds = %0d\n\n",tp.ptv_sec,tp.ptv_usec);


    unsigned char *buffer = (unsigned char *)&tp;
    printf("Dumping tp\n");
    for( int i = 0; i < sizeof(struct paws_timeval); i++ )
        printf("  byte %d = %02x\n",i,buffer[i]);

    sleep1khz(4000,0);
}

// EXIT WILL RETURN TO BIOS
