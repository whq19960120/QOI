
#ifndef   __QOI_H__
#define   __QOI_H__

#include <stdio.h>

extern int qoi2ppm (FILE *fp_qoi, FILE *fp_ppm, FILE *fp_alpha, int *p_w, int *p_h, int *p_ch);

extern int ppm2qoi (FILE *fp_qoi, FILE *fp_ppm, int *p_w, int *p_h);

#endif // __QOI_H__

