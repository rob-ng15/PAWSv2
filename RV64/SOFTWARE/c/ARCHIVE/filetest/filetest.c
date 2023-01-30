#include <stdio.h>
#include <sys/stat.h>
#include <PAWSlibrary.h>

int main( void ) {
    FILE *handle;
    struct stat st;

    handle = fopen( "/doom1.wad", "r" );
    if( handle != NULL ) {
        printf("File /doom1.wad opened for reading! Size: %0d\n",handle->filelength);
        if( !fstat( handle, &st ) ) {
            printf("fstat Size: %0d\n",st.st_size);
        } else {
            printf("Unable to fstat!\n");
        }

        fclose( handle );
    } else {
        printf("File /doom1.wad not opened for reading!\n");
    }

    if( !stat( "/doom1.wad", &st ) ) {
        printf("stat Size: %0d\n",st.st_size);
    } else {
        printf("Unable to stat!\n");
    }

    sleep1khz( 4000, 0 );
    return 0;
}

// EXIT WILL RETURN TO BIOS
