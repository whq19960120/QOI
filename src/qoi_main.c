#include <stdio.h>
#include "qoi.h"


#define  TO_LOWER(c)   ((((c) >= 'A') && ((c) <= 'Z')) ? ((c)+32) : (c))


// return:
//     1 : match
//     0 : mismatch
static int matchSuffixIgnoringCase (const char *string, const char *suffix) {
    const char *p1, *p2;
    for (p1=string; *p1; p1++);
    for (p2=suffix; *p2; p2++);
    while (TO_LOWER(*p1) == TO_LOWER(*p2)) {
        if (p2 <= suffix)
            return 1;
        if (p1 <= string)
            return 0;
        p1 --;
        p2 --;
    }
    return 0;
}


char *USAGE = 
  "To convert QOI to PPM :                       \n"
  "  qoi2ppm  input.qoi  output.ppm  [alpha.pgm] \n"
  "To convert PPM to QOI :                       \n"
  "  qoi2ppm  input.ppm  output.qoi              \n";


int main (int argc, char **argv) {
    FILE *fp_qoi, *fp_ppm, *fp_alpha=NULL;
    int w, h, ch;
    
    if (argc < 3) {
        printf(USAGE);
        return 1;
    }
    
    if ( matchSuffixIgnoringCase(argv[1], ".qoi") ) {
        fp_qoi = fopen(argv[1], "rb");
        
        if (fp_qoi == NULL) {
            printf(USAGE);
            return 1;
        }
        
        fp_ppm = fopen(argv[2], "wb");
        
        if (fp_ppm == NULL) {
            printf(USAGE);
            return 1;
        }
        
        if (argc > 3) {
            fp_alpha = fopen(argv[3], "wb");
            
            if (fp_alpha == NULL) {
                printf(USAGE);
                return 1;
            }
        }
        
        if ( qoi2ppm(fp_qoi, fp_ppm, fp_alpha, &w, &h, &ch) )
            return 1;
        
        printf("channels = %d\n", ch);
        
    } else {
        fp_ppm = fopen(argv[1], "rb");
        
        if (fp_ppm == NULL) {
            printf(USAGE);
            return 1;
        }
        
        fp_qoi = fopen(argv[2], "wb");
        
        if (fp_qoi == NULL) {
            printf(USAGE);
            return 1;
        }
        
        if ( ppm2qoi(fp_qoi, fp_ppm, &w, &h) )
            return 1;
    }
    
    printf("size = (%d x %d)\n", w, h);
    
    return 0;
}

