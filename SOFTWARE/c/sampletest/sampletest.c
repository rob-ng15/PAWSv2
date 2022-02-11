#include <PAWSlibrary.h>

unsigned char samplesLEFT[] = { 50, 52, 54, 56, 58, 60, 62, 64 };
unsigned char samplesRIGHT[] = { 64, 62, 60, 58, 56, 54, 52, 50 };

int main( void ) {
    sample_upload( CHANNEL_LEFT, 8, samplesLEFT );
    sample_upload( CHANNEL_RIGHT, 8, samplesRIGHT );

    printf("Sending Normal Beep\n");
    beep( CHANNEL_LEFT, WAVE_SQUARE, 25, 500 );
    beep( CHANNEL_RIGHT, WAVE_SQUARE, 32, 1000 );

    sleep1khz( 2000, 0 );

    printf("Starting Sample Playback\n");
    beep( CHANNEL_LEFT, WAVE_SAMPLE, 0, 500 );
    beep( CHANNEL_RIGHT, WAVE_SAMPLE, 0, 1000 );

    sleep1khz( 10000, 0 );
}

// EXIT WILL RETURN TO BIOS
