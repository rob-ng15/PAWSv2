#include <math.h>
#include <PAWSlibrary.h>

// TRANSLATION of https://rosettacode.org/wiki/Julia_set#C to PAWSlibrary

typedef struct{
    float x,y;
} complex;

complex add( complex a, complex b ) {
    complex c;
    c.x = a.x + b.x;
    c.y = a.y + b.y;
    return c;
}

complex sqr(complex a){
	complex c;
	c.x = a.x*a.x - a.y*a.y;
	c.y = 2*a.x*a.y;
	return c;
}

float mod(complex a){
	return sqrt(a.x*a.x + a.y*a.y);
}

complex mapPoint(int width,int height,float radius,int x,int y,float scale){
	complex c;
	int l = (width<height)?width:height;

	c.x = 2*radius*(x - width/2.0)/scale;
	c.y = 2*radius*(y - height/2.0)/scale;

	return c;
}

void juliaSet(int width,int height,complex c,float radius,int n,float scale){
	int x,y,i;
	complex z0,z1;

	for(x=0;x<=width;x++)
		for(y=0;y<=height;y++){
			z0 = mapPoint(width,height,radius,x,y,scale);
			for(i=1;i<=n;i++){
				z1 = add(sqr(z0),c);
				if(mod(z1)>radius){
					gpu_pixel(i,x,y);
					break;
				}
				z0 = z1;
			}
		}
}

// TRANSLATION OF http://www.rosettacode.org/wiki/Mandelbrot_set#BASIC256 to C and PAWSlibrary

void mandelbrot( void ) {
    const int graphwidth = 320, graphheight = 240, kt = 64;
    float m = 4.0;
    float xmin = -2.1, xmax = 0.6, ymin = -1.35, ymax = 1.35;
    float dx = (xmax - xmin) / graphwidth, dy = (ymax - ymin) / graphheight;
    float jx, jy, tx, ty, wx, wy, r;
    int k;

    for( int step = 16; step > 0; step = step >> 1 ) {
        int offset = step - 1;
        if( !offset ) gpu_pixelblock_start( 0, 0, graphwidth );

        for( int y = 0; y < graphheight; y += step) {
            jy = ymin + y * dy;
            for( int x = 0; x < graphwidth; x += step ) {
                jx = xmin + x * dx;
                k = 0; wx = 0.0; wy = 0.0;
                do {
                    tx = wx * wx - wy * wy + jx;
                    ty = 2.0 * wx * wy + jy;
                    wx = tx;
                    wy = ty;
                    r = wx * wx + wy * wy;
                    k = k + 1;
                } while( ( r < m ) && ( k < kt ) );

                if( offset ) {
                    gpu_rectangle( k, x, y, x + offset, y + offset );
                } else {
                    gpu_pixelblock_pixel( k );
                }
            }
        }
    }

    gpu_pixelblock_stop();
}

int main( int argc, char **argv ) {
    screen_mode( 0, MODE_RGBM, 0 );

    mandelbrot(); sleep1khz( 4000, 0 );

    complex c; c.x = -0.7; c.y = 0.27015;
    int bitmap = 2;

    for( int scale = 720; scale <= 1040; scale += 40 ) {
        bitmap_draw( bitmap );
        juliaSet(320,240,c,4,64,scale);
        bitmap_display( bitmap );
        bitmap = 3 - bitmap;
    }

    sleep1khz( 4000, 0 );

}
