//
//  crt.h
//  Chocolate Wolfenstein 3D
//
//  Created by fabien sanglard on 2014-08-26.
//
//
#ifndef crt_h
#define crt_h

extern SDL_Color curpal[256];

/*
 *   Trigger the Digital To Analogic convertion
 */
void CRT_DAC(void);

void CRT_Screenshot(void);
#endif
