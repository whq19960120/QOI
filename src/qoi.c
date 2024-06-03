#include "qoi.h"


// return : 0=success  1=failed
static int parse_ppm_header (FILE *fp_ppm, int *p_w, int *p_h) {
    int c;
    
    if (fgetc(fp_ppm) != 'P')
        return 1;
    if (fgetc(fp_ppm) != '6')
        return 1;
    
    if ( fscanf(fp_ppm, "%d%d%d", p_w, p_h, &c) < 3 )
        return 1;
    
    if (*p_w <= 0 || *p_h <= 0 || c < 2 || c > 255)
        return 1;
    
    c = fgetc(fp_ppm);
    if (c != ' ' && c != '\n' && c != '\r' && c != '\t')
        return 1;
    
    return 0;
}


// return : 0=success  1=failed
static int parse_qoi_header (FILE *fp_qoi, int *p_w, int *p_h, int *p_ch) {
    if (fgetc(fp_qoi) != 'q')
        return 1;
    if (fgetc(fp_qoi) != 'o')
        return 1;
    if (fgetc(fp_qoi) != 'i')
        return 1;
    if (fgetc(fp_qoi) != 'f')
        return 1;
    
    *p_w =               fgetc(fp_qoi);
    *p_w = ((*p_w)<<8) + fgetc(fp_qoi);
    *p_w = ((*p_w)<<8) + fgetc(fp_qoi);
    *p_w = ((*p_w)<<8) + fgetc(fp_qoi);
    
    *p_h =               fgetc(fp_qoi);
    *p_h = ((*p_h)<<8) + fgetc(fp_qoi);
    *p_h = ((*p_h)<<8) + fgetc(fp_qoi);
    *p_h = ((*p_h)<<8) + fgetc(fp_qoi);
    
    *p_ch = fgetc(fp_qoi);
    fgetc(fp_qoi);
    
    if (*p_w <= 0 || *p_h <= 0 || *p_ch < 3 || *p_ch > 4)
        return 1;
    
    return 0;
}


static void write_qoi_header (FILE *fp_qoi, int w, int h) {
    fprintf(fp_qoi, "qoif");
    fputc((w>>24)&0xff, fp_qoi);
    fputc((w>>16)&0xff, fp_qoi);
    fputc((w>>8 )&0xff, fp_qoi);
    fputc((w    )&0xff, fp_qoi);
    fputc((h>>24)&0xff, fp_qoi);
    fputc((h>>16)&0xff, fp_qoi);
    fputc((h>>8 )&0xff, fp_qoi);
    fputc((h    )&0xff, fp_qoi);
    fputc(        0x03, fp_qoi);
  //fputc(        0x04, fp_qoi);
    fputc(        0x00, fp_qoi);
}


// return : 0=success  1=failed
int ppm2qoi (FILE *fp_qoi, FILE *fp_ppm, int *p_w, int *p_h) {
    unsigned char idx, run=0;
    unsigned char pr=0, pg=0, pb=0, pa=255;
    unsigned char cr  , cg  , cb  , ca;
    unsigned char dr  , dg  , db      ;
    unsigned char ar [64] = {0};
    unsigned char ag [64] = {0};
    unsigned char ab [64] = {0};
    unsigned char aa [64] = {0};
    unsigned long long int count;
    
    if ( parse_ppm_header(fp_ppm, p_w, p_h) )
        return 1;
    
    write_qoi_header(fp_qoi, *p_w, *p_h);
    
    count = (unsigned long long int)(*p_w) * (unsigned long long int)(*p_h);
    
    for (; count>0; count--) {
        cr = fgetc(fp_ppm);
        cg = fgetc(fp_ppm);
        cb = fgetc(fp_ppm);
        ca = 255;
        
        idx = (3*cr + 5*cg + 7*cb + 11*ca) & 0x3f;
        
        if (cr==pr && cg==pg && cb==pb && ca==pa) {
            run ++;
            if (run >= 62) {
                fputc((0xc0 | (run-1)), fp_qoi);      // QOI_OP_RUN
                run = 0;
            }
            
        } else {
            if (run > 0) {
                fputc((0xc0 | (run-1)), fp_qoi);      // QOI_OP_RUN
                run = 0;
            }
            
            if (cr == ar[idx] && cg == ag[idx] && cb == ab[idx] && ca == aa[idx]) {
                fputc(idx, fp_qoi);                   // QOI_OP_INDEX
            
            } else {
                dr = cr - pr + 2;
                dg = cg - pg + 2;
                db = cb - pb + 2;
                
                if (dr<4 && dg<4 && db<4 && ca==pa) {
                    fputc((0x40 | (dr<<4) | (dg<<2) | db), fp_qoi);   // QOI_OP_DIFF
                    
                } else {
                    dr = dr - dg + 8;
                    db = db - dg + 8;
                    dg += 30;
                    
                    if (dr<16 && dg<64 && db<16 && ca==pa) {
                        fputc((   0x80 | dg), fp_qoi);                // QOI_OP_LUMA
                        fputc(((dr<<4) | db), fp_qoi);
                        
                    } else {
                        fputc(((ca!=pa)?0xff:0xfe) , fp_qoi);         // QOI_OP_RGB or QOI_OP_RGBA
                        fputc(cr, fp_qoi);
                        fputc(cg, fp_qoi);
                        fputc(cb, fp_qoi);
                        if (ca != pa)
                            fputc(ca, fp_qoi);
                    }
                }
            }
        }
        
        ar[idx] = pr = cr;
        ag[idx] = pg = cg;
        ab[idx] = pb = cb;
        aa[idx] = pa = ca;
    }
    
    if (run > 0)
        fputc((0xc0 | (run-1)), fp_qoi);              // QOI_OP_RUN
    
    return 0;
}


// return : 0=success  1=failed
int qoi2ppm (FILE *fp_qoi, FILE *fp_ppm, FILE *fp_alpha, int *p_w, int *p_h, int *p_ch) {
    unsigned char tag, type, run;
    unsigned char r=0, g=0, b=0, a=255;
    unsigned char ar [64] = {0};
    unsigned char ag [64] = {0};
    unsigned char ab [64] = {0};
    unsigned char aa [64] = {0};
    unsigned long long int count;
    
    if ( parse_qoi_header(fp_qoi, p_w, p_h, p_ch) )
        return 1;
    
    count = (unsigned long long int)(*p_w) * (unsigned long long int)(*p_h);
    
    fprintf(fp_ppm, "P6\n%d %d\n255\n", *p_w, *p_h);
    
    if (fp_alpha)
        fprintf(fp_alpha, "P5\n%d %d\n255\n", *p_w, *p_h);
    
    for (;;) {
        tag  = fgetc(fp_qoi);
        type = (tag >> 6);
        tag &= 0x3f;
        run  = 1;
        
        switch (type) {
            case 0 :                     // QOI_OP_INDEX
                r = ar[tag];
                g = ag[tag];
                b = ab[tag];
                a = aa[tag];
                break;
            
            case 1 :                     // QOI_OP_DIFF
                r += ((tag >> 4)      ) - 2;
                g += ((tag >> 2) & 0x3) - 2;
                b += ((tag     ) & 0x3) - 2;
                break;
            
            case 2 :                     // QOI_OP_LUMA
                type = fgetc(fp_qoi);
                g += tag - 32;
                r += tag - 40 + (type >> 4 );
                b += tag - 40 + (type & 0xf);
                break;
            
            default :
                if (tag < 62) {          // QOI_OP_RUN
                    run = 1 + tag;
                } else {                 // QOI_OP_RGB or QOI_OP_RGBA
                    r = fgetc(fp_qoi);
                    g = fgetc(fp_qoi);
                    b = fgetc(fp_qoi);
                    if (tag == 63)
                        a = fgetc(fp_qoi);
                }
                break;
        }
        
        tag = (3*r + 5*g + 7*b + 11*a) & 0x3f;
        ar[tag] = r;
        ag[tag] = g;
        ab[tag] = b;
        aa[tag] = a;
        
        for (; run>0; run--) {
            fputc(r, fp_ppm);
            fputc(g, fp_ppm);
            fputc(b, fp_ppm);
            
            if (fp_alpha)
                fputc(a, fp_alpha);
            
            count --;
            if (count == 0)
                return 0;
        }
    }
}

