// nanojpeg.c from https://keyj.emphy.de/nanojpeg/
#ifndef _NANOJPEG_H
typedef enum _nj_result {
    NJ_OK = 0,        // no error, decoding successful
    NJ_NO_JPEG,       // not a JPEG file
    NJ_UNSUPPORTED,   // unsupported format
    NJ_OUT_OF_MEM,    // out of memory
    NJ_INTERNAL_ERR,  // internal error
    NJ_SYNTAX_ERROR,  // syntax error
    __NJ_FINISHED,    // used internally, will never be reported
} nj_result_t;
extern void njInit(void);
extern nj_result_t njDecode(const void* jpeg, const int size);
extern int njGetWidth(void);
extern int njGetHeight(void);
extern int njIsColor(void);
extern unsigned char* njGetImage(void);
extern int njGetImageSize(void);
extern void njDone(void);
#define _NANOJPEG_H
#endif
