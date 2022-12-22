#include "funkey_prod_screens.h"
#ifdef __ARM_FP
    #warning ARM NEON enabled
    #include <arm_neon.h>
#endif //__ARM_FP


/// Defines
#define AT(i, j)    ((i) * N + (j))

/// Static variables


/// -------------- FUNCTIONS IMPLEMENTATION --------------
uint16_t *transpose4x4(uint16_t * pixels_4x4){
    int N = 4;
    uint16_t pixels_4x4_out[4*4] = {
            pixels_4x4[AT(0, 0)], pixels_4x4[AT(1, 0)], pixels_4x4[AT(2, 0)], pixels_4x4[AT(3, 0)], 
            pixels_4x4[AT(0, 1)], pixels_4x4[AT(1, 1)], pixels_4x4[AT(2, 1)], pixels_4x4[AT(3, 1)], 
            pixels_4x4[AT(0, 2)], pixels_4x4[AT(1, 2)], pixels_4x4[AT(2, 2)], pixels_4x4[AT(3, 2)], 
            pixels_4x4[AT(0, 3)], pixels_4x4[AT(1, 3)], pixels_4x4[AT(2, 3)], pixels_4x4[AT(3, 3)], 
        };
    memcpy(pixels_4x4, pixels_4x4_out, 4*4*sizeof(uint16_t));
    return pixels_4x4;
}

void disp4(uint16_t * pixels_1x4){
    int N = 4;
    printf("%d, %d, %d, %d\n", pixels_1x4[AT(0, 0)], pixels_1x4[AT(0, 1)], pixels_1x4[AT(0, 2)], pixels_1x4[AT(0, 3)]);
}

void disp4x4(uint16_t * pixels_4x4){
    int N = 4;
    printf("%d, %d, %d, %d\n", pixels_4x4[AT(0, 0)], pixels_4x4[AT(0, 1)], pixels_4x4[AT(0, 2)], pixels_4x4[AT(0, 3)]);
    printf("%d, %d, %d, %d\n", pixels_4x4[AT(1, 0)], pixels_4x4[AT(1, 1)], pixels_4x4[AT(1, 2)], pixels_4x4[AT(1, 3)]);
    printf("%d, %d, %d, %d\n", pixels_4x4[AT(2, 0)], pixels_4x4[AT(2, 1)], pixels_4x4[AT(2, 2)], pixels_4x4[AT(2, 3)]);
    printf("%d, %d, %d, %d\n", pixels_4x4[AT(3, 0)], pixels_4x4[AT(3, 1)], pixels_4x4[AT(3, 2)], pixels_4x4[AT(3, 3)]);
}

/* Soft Matrix Rotation with only 1 pixel of extra RAM needed
Works only on 2D square matrices */
void fbtft_rotate_soft_square(uint16_t *mat, int size, int rotation)
{
    int i, j;
    uint16_t temp;
    int N = size;

    if (rotation == 90) {
        /* Rotate screen 90° Clockwise */
        for (i = 0; i < N / 2; i++) {
            for (j = i; j < N - i - 1; j++) {
                temp = mat[AT(i, j)];
                mat[AT(i, j)] = mat[AT(N - 1 - j, i)];
                mat[AT(N - 1 - j, i)] = mat[AT(N - 1 - i, N - 1 - j)];
                mat[AT(N - 1 - i, N - 1 - j)] = mat[AT(j, N - 1 - i)];
                mat[AT(j, N - 1 - i)] = temp;
            }
        }
    } else if (rotation == 270) {
        /* Rotate screen 270° Clockwise */
        for (i = 0; i < N / 2; i++) {
            for (j = i; j < N - i - 1; j++) {
                temp = mat[AT(i, j)];
                mat[AT(i, j)] = mat[AT(j, N - 1 - i)];
                mat[AT(j, N-1-i)] = mat[AT(N - 1 - i, N - 1 - j)];
                mat[AT(N - 1 - i, N - 1 - j)] = mat[AT(N - 1 - j, i)];
                mat[AT(N - 1 - j, i)] = temp;
            }
        }
    }
}

/* Soft Matrix Rotation with export to other buf
Works only on 2D square matrices */
void fbtft_rotate_soft_square_export(uint16_t *src, uint16_t *dst, int size, int rotation)
{
    int i, j;
    uint16_t temp;
    int N = size;

    if (rotation == 90) {
        /* Rotate screen 90° Clockwise */
        for (i = 0; i < N / 2; i++) {
            for (j = i; j < N - i - 1; j++) {
                dst[AT(i, j)] = src[AT(N - 1 - j, i)];
                dst[AT(N - 1 - j, i)] = src[AT(N - 1 - i, N - 1 - j)];
                dst[AT(N - 1 - i, N - 1 - j)] = src[AT(j, N - 1 - i)];
                dst[AT(j, N - 1 - i)] = src[AT(i, j)];
            }
        }
    } else if (rotation == 270) {
        /* Rotate screen 270° Clockwise */
        for (i = 0; i < N / 2; i++) {
            for (j = i; j < N - i - 1; j++) {
                dst[AT(i, j)] = src[AT(j, N - 1 - i)];
                dst[AT(j, N-1-i)] = src[AT(N - 1 - i, N - 1 - j)];
                dst[AT(N - 1 - i, N - 1 - j)] = src[AT(N - 1 - j, i)];
                dst[AT(N - 1 - j, i)] = src[AT(i, j)];
            }
        }
    }
}

/*  
    Soft transpose 
*/
void fbtft_transpose(uint16_t* src, uint16_t* dst, int w, int h){
    
    /* Vars */
    int y, x;

    /* Main loop */
    for (y=0; y<h; y++){
        for (x=0; x<w; x++){
            dst[x*h + y] = src[y*w+x];
        }
    }
}

/*  
    Soft transpose with 4x4 bocks a la mano
*/
void fbtft_transpose_mano4x4(uint16_t* src, uint16_t* dst, int w, int h){
    
    /* Vars */
    int y, x;
    uint16_t tmp_tansp[4*4*sizeof(uint16_t)];
    int N = 4;

    /* Main loop */
    for (y=0; y<h; y+=4){
        for (x=0; x<w; x+=4){
            memcpy( &tmp_tansp[AT(0,0)],  &src[ w*(y+0) + (x+0) ], 4*sizeof(uint16_t));
            memcpy( &tmp_tansp[AT(1,0)],  &src[ w*(y+1) + (x+0) ], 4*sizeof(uint16_t));
            memcpy( &tmp_tansp[AT(2,0)],  &src[ w*(y+2) + (x+0) ], 4*sizeof(uint16_t));
            memcpy( &tmp_tansp[AT(3,0)],  &src[ w*(y+3) + (x+0) ], 4*sizeof(uint16_t));
            transpose4x4(tmp_tansp);         
            memcpy( &dst[ h*(x+0) + y ], &tmp_tansp[AT(0,0)], 4*sizeof(uint16_t));
            memcpy( &dst[ h*(x+1) + y ], &tmp_tansp[AT(1,0)], 4*sizeof(uint16_t));
            memcpy( &dst[ h*(x+2) + y ], &tmp_tansp[AT(2,0)], 4*sizeof(uint16_t));
            memcpy( &dst[ h*(x+3) + y ], &tmp_tansp[AT(3,0)], 4*sizeof(uint16_t));
        }
    }
}

#ifdef __ARM_FP
/*  
    NEON optimized matrix transpose 
    (dimensions multiple of 4, 16bits pixels)
*/
void fbtft_transpose_neon(uint16_t* src, uint16_t* dst, int w, int h){
    
    /* Vars */
    uint16x4x4_t v_tmp;
    int y, x;

    /* Main loop */
    for (y=0; y<h; y+=4){
        for (x=0; x<w; x+=4){

            /* 1% CPU gain */
            __builtin_prefetch(src + (y+0)*w + x + 4);
            __builtin_prefetch(src + (y+1)*w + x + 4);
            __builtin_prefetch(src + (y+2)*w + x + 4);
            __builtin_prefetch(src + (y+3)*w + x + 4);

            /* Neon Load */
            v_tmp.val[0] = vld1_u16(src + (y+0)*w + x );
            v_tmp.val[1] = vld1_u16(src + (y+1)*w + x );
            v_tmp.val[2] = vld1_u16(src + (y+2)*w + x );
            v_tmp.val[3] = vld1_u16(src + (y+3)*w + x );

            /* Neon store (4 interleaved) */
            vst4_lane_u16(dst + (x+0)*h + y, v_tmp, 0);
            vst4_lane_u16(dst + (x+1)*h + y, v_tmp, 1);
            vst4_lane_u16(dst + (x+2)*h + y, v_tmp, 2);
            vst4_lane_u16(dst + (x+3)*h + y, v_tmp, 3);
        }
    }
}

/*  
    NEON optimized matrix transpose inverse
    (dimensions multiple of 4, 16bits pixels)
*/
void fbtft_transpose_inv_neon(uint16_t* src, uint16_t* dst, int w, int h){
    
    /* Vars */
    uint16x4x4_t v_tmp;
    int y, x;

    /* Main loop */
    for (y=0; y<h; y+=4){
        for (x=0; x<w; x+=4){

            /* 1% CPU gain */
            __builtin_prefetch(src + (y+0)*w + x + 4);
            __builtin_prefetch(src + (y+1)*w + x + 4);
            __builtin_prefetch(src + (y+2)*w + x + 4);
            __builtin_prefetch(src + (y+3)*w + x + 4);

            /* Neon Load */
            v_tmp.val[0] = vld1_u16(src + (y+3)*w + x );
            v_tmp.val[1] = vld1_u16(src + (y+2)*w + x );
            v_tmp.val[2] = vld1_u16(src + (y+1)*w + x );
            v_tmp.val[3] = vld1_u16(src + (y+0)*w + x );

            /* Neon store (4 interleaved) */
            vst4_lane_u16(dst + ( (w-1) - x - 3 )*h + (h-y-3-1), v_tmp, 3);
            vst4_lane_u16(dst + ( (w-1) - x - 2 )*h + (h-y-3-1), v_tmp, 2);
            vst4_lane_u16(dst + ( (w-1) - x - 1 )*h + (h-y-3-1), v_tmp, 1);
            vst4_lane_u16(dst + ( (w-1) - x - 0 )*h + (h-y-3-1), v_tmp, 0);
        }
    }
}

/*  
    NEON optimized matrix rotate 90° CW 
    (dimensions multiple of 4, 16bits pixels)
*/
void fbtft_rotate_90cw_neon(uint16_t* src, uint16_t* dst, int w, int h){
    
    /* Vars */
    uint16x4x4_t v_tmp;
    int y, x;

    /* Main loop */
    for (y=0; y<h; y+=4){
        for (x=0; x<w; x+=4){

            /* 1% CPU gain */
            __builtin_prefetch(src + (y+0)*w + x + 4);
            __builtin_prefetch(src + (y+1)*w + x + 4);
            __builtin_prefetch(src + (y+2)*w + x + 4);
            __builtin_prefetch(src + (y+3)*w + x + 4);

            /* Neon Load */
            v_tmp.val[0] = vld1_u16(src + (y+3)*w + x );
            v_tmp.val[1] = vld1_u16(src + (y+2)*w + x );
            v_tmp.val[2] = vld1_u16(src + (y+1)*w + x );
            v_tmp.val[3] = vld1_u16(src + (y+0)*w + x );

            /* Neon store (4 interleaved) */
            vst4_lane_u16(dst + (x+0)*h + (h-y-3-1), v_tmp, 0);
            vst4_lane_u16(dst + (x+1)*h + (h-y-3-1), v_tmp, 1);
            vst4_lane_u16(dst + (x+2)*h + (h-y-3-1), v_tmp, 2);
            vst4_lane_u16(dst + (x+3)*h + (h-y-3-1), v_tmp, 3);
        }
    }
}

/*  
    NEON optimized matrix rotate 270° CW
    (dimensions multiple of 4, 16bits pixels)
*/
void fbtft_rotate_270cw_neon(uint16_t* src, uint16_t* dst, int w, int h){
    
    /* Vars */
    uint16x4x4_t v_tmp;
    int y, x;

    /* Main loop */
    for (y=0; y<h; y+=4){
        for (x=0; x<w; x+=4){

            /* Neon Load */
            v_tmp.val[0] = vld1_u16(src + (y+0)*w + x );
            v_tmp.val[1] = vld1_u16(src + (y+1)*w + x );
            v_tmp.val[2] = vld1_u16(src + (y+2)*w + x );
            v_tmp.val[3] = vld1_u16(src + (y+3)*w + x );

            /* Neon store (4 interleaved) */
            vst4_lane_u16(dst + ( (w-1) - x - 3 )*h + y, v_tmp, 3);
            vst4_lane_u16(dst + ( (w-1) - x - 2 )*h + y, v_tmp, 2);
            vst4_lane_u16(dst + ( (w-1) - x - 1 )*h + y, v_tmp, 1);
            vst4_lane_u16(dst + ( (w-1) - x - 0 )*h + y, v_tmp, 0);
        }
    }
}

/*  
    NEON optimized matrix rotate 270° CW
    (dimensions multiple of 4, 16bits pixels)
*/
#define PREFETCH_ORDER_X    32
#define PREFETCH_ORDER_Y    4
void fbtft_rotate_270cw_neon_prefetch(uint16_t* src, uint16_t* dst, int w, int h){
    
    /* Vars */
    uint16x4x4_t v_tmp;
    int y, x;

    /* Main loop */
    for (y=0; y<h; y+=4){

        /* Prefetch src */
        __builtin_prefetch(src + (y+PREFETCH_ORDER_Y+0)*w + x);
        __builtin_prefetch(src + (y+PREFETCH_ORDER_Y+1)*w + x);
        __builtin_prefetch(src + (y+PREFETCH_ORDER_Y+2)*w + x);
        __builtin_prefetch(src + (y+PREFETCH_ORDER_Y+3)*w + x);

        /* Prefetch dst */
        /*__builtin_prefetch(dst + ( (w-1) - x - 3 + PREFETCH_ORDER_Y )*h + y + 4, 1);
        __builtin_prefetch(dst + ( (w-1) - x - 2 + PREFETCH_ORDER_Y )*h + y + 4, 1);
        __builtin_prefetch(dst + ( (w-1) - x - 1 + PREFETCH_ORDER_Y )*h + y + 4, 1);
        __builtin_prefetch(dst + ( (w-1) - x - 0 + PREFETCH_ORDER_Y )*h + y + 4, 1);*/

        for (x=0; x<w; x+=4){

            /* Prefetch src */
            __builtin_prefetch(src + (y+0)*w + x + PREFETCH_ORDER_X);
            __builtin_prefetch(src + (y+1)*w + x + PREFETCH_ORDER_X);
            __builtin_prefetch(src + (y+2)*w + x + PREFETCH_ORDER_X);
            __builtin_prefetch(src + (y+3)*w + x + PREFETCH_ORDER_X);

            /* Prefetch dst */
            /*__builtin_prefetch(dst + ( (w-1) - x - 3 + PREFETCH_ORDER_X )*h + y, 1);
            __builtin_prefetch(dst + ( (w-1) - x - 2 + PREFETCH_ORDER_X )*h + y, 1);
            __builtin_prefetch(dst + ( (w-1) - x - 1 + PREFETCH_ORDER_X )*h + y, 1);
            __builtin_prefetch(dst + ( (w-1) - x - 0 + PREFETCH_ORDER_X )*h + y, 1);*/

            /* Neon Load */
            v_tmp.val[0] = vld1_u16(src + (y+0)*w + x );
            v_tmp.val[1] = vld1_u16(src + (y+1)*w + x );
            v_tmp.val[2] = vld1_u16(src + (y+2)*w + x );
            v_tmp.val[3] = vld1_u16(src + (y+3)*w + x );

            /* Neon store (4 interleaved) */
            vst4_lane_u16(dst + ( (w-1) - x - 3 )*h + y, v_tmp, 3);
            vst4_lane_u16(dst + ( (w-1) - x - 2 )*h + y, v_tmp, 2);
            vst4_lane_u16(dst + ( (w-1) - x - 1 )*h + y, v_tmp, 1);
            vst4_lane_u16(dst + ( (w-1) - x - 0 )*h + y, v_tmp, 0);
        }
    }
}

/*  
    NEON optimized matrix rotate 270° CW
    (dimensions multiple of 4, 16bits pixels)
*/
#define PREFETCH_ORDER_X    32
#define PREFETCH_ORDER_Y    4
void fbtft_rotate_270cw_neon_prefetch2(uint16_t* src, uint16_t* dst, int w, int h){
    
    /* Vars */
    uint16x4x4_t v_tmp, v_tmp2, v_tmp3, v_tmp4;
    int y, x;

    /* Main loop */
    for (y=0; y<h; y+=4){

        /* Prefetch src */
        __builtin_prefetch(src + (y+PREFETCH_ORDER_Y+0)*w + x);
        __builtin_prefetch(src + (y+PREFETCH_ORDER_Y+1)*w + x);
        __builtin_prefetch(src + (y+PREFETCH_ORDER_Y+2)*w + x);
        __builtin_prefetch(src + (y+PREFETCH_ORDER_Y+3)*w + x);

        /* Prefetch dst */
        /*__builtin_prefetch(dst + ( (w-1) - x - 3 + PREFETCH_ORDER_Y )*h + y + 4, 1);
        __builtin_prefetch(dst + ( (w-1) - x - 2 + PREFETCH_ORDER_Y )*h + y + 4, 1);
        __builtin_prefetch(dst + ( (w-1) - x - 1 + PREFETCH_ORDER_Y )*h + y + 4, 1);
        __builtin_prefetch(dst + ( (w-1) - x - 0 + PREFETCH_ORDER_Y )*h + y + 4, 1);*/

        for (x=0; x<w; x+=16){

            /* Prefetch src */
            __builtin_prefetch(src + (y+0)*w + x + PREFETCH_ORDER_X);
            __builtin_prefetch(src + (y+1)*w + x + PREFETCH_ORDER_X);
            __builtin_prefetch(src + (y+2)*w + x + PREFETCH_ORDER_X);
            __builtin_prefetch(src + (y+3)*w + x + PREFETCH_ORDER_X);

            /* Prefetch dst */
            /*__builtin_prefetch(dst + ( (w-1) - x - 3 + PREFETCH_ORDER_X )*h + y, 1);
            __builtin_prefetch(dst + ( (w-1) - x - 2 + PREFETCH_ORDER_X )*h + y, 1);
            __builtin_prefetch(dst + ( (w-1) - x - 1 + PREFETCH_ORDER_X )*h + y, 1);
            __builtin_prefetch(dst + ( (w-1) - x - 0 + PREFETCH_ORDER_X )*h + y, 1);*/

            /* Neon Load */
            v_tmp.val[0] = vld1_u16(src + (y+0)*w + x );
            v_tmp.val[1] = vld1_u16(src + (y+1)*w + x );
            v_tmp.val[2] = vld1_u16(src + (y+2)*w + x );
            v_tmp.val[3] = vld1_u16(src + (y+3)*w + x );

            /* Neon store (4 interleaved) */
            vst4_lane_u16(dst + ( (w-1) - x - 3 )*h + y, v_tmp, 3);
            vst4_lane_u16(dst + ( (w-1) - x - 2 )*h + y, v_tmp, 2);
            vst4_lane_u16(dst + ( (w-1) - x - 1 )*h + y, v_tmp, 1);
            vst4_lane_u16(dst + ( (w-1) - x - 0 )*h + y, v_tmp, 0);

            /* Neon Load */
            v_tmp2.val[0] = vld1_u16(src + (y+0)*w + x+4 );
            v_tmp2.val[1] = vld1_u16(src + (y+1)*w + x+4 );
            v_tmp2.val[2] = vld1_u16(src + (y+2)*w + x+4 );
            v_tmp2.val[3] = vld1_u16(src + (y+3)*w + x+4 );

            /* Neon store (4 interleaved) */
            vst4_lane_u16(dst + ( (w-1) - x+4 - 3 )*h + y, v_tmp2, 3);
            vst4_lane_u16(dst + ( (w-1) - x+4 - 2 )*h + y, v_tmp2, 2);
            vst4_lane_u16(dst + ( (w-1) - x+4 - 1 )*h + y, v_tmp2, 1);
            vst4_lane_u16(dst + ( (w-1) - x+4 - 0 )*h + y, v_tmp2, 0);

            /* Neon Load */
            v_tmp3.val[0] = vld1_u16(src + (y+0)*w + x+8 );
            v_tmp3.val[1] = vld1_u16(src + (y+1)*w + x+8 );
            v_tmp3.val[2] = vld1_u16(src + (y+2)*w + x+8 );
            v_tmp3.val[3] = vld1_u16(src + (y+3)*w + x+8 );

            /* Neon store (4 interleaved) */
            vst4_lane_u16(dst + ( (w-1) - x+8 - 3 )*h + y, v_tmp3, 3);
            vst4_lane_u16(dst + ( (w-1) - x+8 - 2 )*h + y, v_tmp3, 2);
            vst4_lane_u16(dst + ( (w-1) - x+8 - 1 )*h + y, v_tmp3, 1);
            vst4_lane_u16(dst + ( (w-1) - x+8 - 0 )*h + y, v_tmp3, 0);

            /* Neon Load */
            v_tmp4.val[0] = vld1_u16(src + (y+0)*w + x+12 );
            v_tmp4.val[1] = vld1_u16(src + (y+1)*w + x+12 );
            v_tmp4.val[2] = vld1_u16(src + (y+2)*w + x+12 );
            v_tmp4.val[3] = vld1_u16(src + (y+3)*w + x+12 );

            /* Neon store (4 interleaved) */
            vst4_lane_u16(dst + ( (w-1) - x+12 - 3 )*h + y, v_tmp4, 3);
            vst4_lane_u16(dst + ( (w-1) - x+12 - 2 )*h + y, v_tmp4, 2);
            vst4_lane_u16(dst + ( (w-1) - x+12 - 1 )*h + y, v_tmp4, 1);
            vst4_lane_u16(dst + ( (w-1) - x+12 - 0 )*h + y, v_tmp4, 0);
        }
    }
}






#define prefetch(x) __builtin_prefetch(x)
#ifndef PREFETCH_STRIDE
#define PREFETCH_STRIDE (2*32)
#endif
static inline void prefetch_range(void *addr, size_t len)
{
    char *cp;
    char *end = addr + len;

    for (cp = addr; cp < end; cp += PREFETCH_STRIDE)
        prefetch(cp);
}



/*  
    NEON optimized matrix rotate 270° CW
    (dimensions multiple of 4, 16bits pixels)
*/
#define PREFETCH_ORDER_X    20
#define PREFETCH_ORDER_Y    4
#define PREFETCH_RANGE      1
void fbtft_rotate_270cw_neon_prefetch3(uint16_t* src, uint16_t* dst, int w, int h){
    
    /* Vars */
    uint16x4x4_t v_tmp;
    int y, x;

    /* Main loop */
    for (y=0; y<h; y+=4){

        /* Prefetch src */
        prefetch_range(src + (y+PREFETCH_ORDER_Y+0)*w + x, PREFETCH_RANGE);
        prefetch_range(src + (y+PREFETCH_ORDER_Y+1)*w + x, PREFETCH_RANGE);
        prefetch_range(src + (y+PREFETCH_ORDER_Y+2)*w + x, PREFETCH_RANGE);
        prefetch_range(src + (y+PREFETCH_ORDER_Y+3)*w + x, PREFETCH_RANGE);

        /* Prefetch dst */
        /*__builtin_prefetch(dst + ( (w-1) - x - 3 + PREFETCH_ORDER_Y )*h + y + 4, 1);
        __builtin_prefetch(dst + ( (w-1) - x - 2 + PREFETCH_ORDER_Y )*h + y + 4, 1);
        __builtin_prefetch(dst + ( (w-1) - x - 1 + PREFETCH_ORDER_Y )*h + y + 4, 1);
        __builtin_prefetch(dst + ( (w-1) - x - 0 + PREFETCH_ORDER_Y )*h + y + 4, 1);*/

        for (x=0; x<w; x+=4){

            /* Prefetch src */
            prefetch_range(src + (y+0)*w + x + PREFETCH_ORDER_X, PREFETCH_RANGE);
            prefetch_range(src + (y+1)*w + x + PREFETCH_ORDER_X, PREFETCH_RANGE);
            prefetch_range(src + (y+2)*w + x + PREFETCH_ORDER_X, PREFETCH_RANGE);
            prefetch_range(src + (y+3)*w + x + PREFETCH_ORDER_X, PREFETCH_RANGE);

            /* Prefetch dst */
            /*__builtin_prefetch(dst + ( (w-1) - x - 3 + PREFETCH_ORDER_X )*h + y, 1);
            __builtin_prefetch(dst + ( (w-1) - x - 2 + PREFETCH_ORDER_X )*h + y, 1);
            __builtin_prefetch(dst + ( (w-1) - x - 1 + PREFETCH_ORDER_X )*h + y, 1);
            __builtin_prefetch(dst + ( (w-1) - x - 0 + PREFETCH_ORDER_X )*h + y, 1);*/

            /* Neon Load */
            v_tmp.val[0] = vld1_u16( &src[(y+0)*w + x] );
            v_tmp.val[1] = vld1_u16( &src[(y+1)*w + x] );
            v_tmp.val[2] = vld1_u16( &src[(y+2)*w + x] );
            v_tmp.val[3] = vld1_u16( &src[(y+3)*w + x] );

            /* Neon store (4 interleaved) */
            vst4_lane_u16( &dst[ ( (w-1) - x - 3 )*h + y ], v_tmp, 3);
            vst4_lane_u16( &dst[ ( (w-1) - x - 2 )*h + y ], v_tmp, 2);
            vst4_lane_u16( &dst[ ( (w-1) - x - 1 )*h + y ], v_tmp, 1);
            vst4_lane_u16( &dst[ ( (w-1) - x - 0 )*h + y ], v_tmp, 0);
        }
    }
}
#endif //__ARM_FP











int launch_prod_screen_tests(int argc, char *argv[]){
    SDL_Event event;
    SDL_Surface *text_surface = NULL;
    SDL_Rect text_pos;
    int res = EXIT_FAILURE;
    int stop_menu_loop = 0;

    /* Fill screen white */
    //SDL_FillRect(hw_surface, NULL, SDL_MapRGBA(hw_surface->format, bg_color.r, bg_color.g, bg_color.b, 0) );

    /* Load Img */
    SDL_Surface *image=IMG_Load(IMG_CONSOLE_COLOR_CHART);
    if(!image) {
        printf("ERROR IMG_Load: %s\n", IMG_GetError());
        printf("IMG path is: %s\n", IMG_CONSOLE_COLOR_CHART);
        exit(1);
    }
    SDL_SetAlpha( image, 0, SDL_ALPHA_OPAQUE );

    printf("Original Img is %dx%d, %dbpp\n", image->w, image->h, image->format->BitsPerPixel);

    /* Convert to RGB bits*/
    SDL_Surface *image_rgb_16b = SDL_CreateRGBSurface(SDL_SWSURFACE, image->w, image->h, 16, 0,0,0,0);
    SDL_BlitSurface(image, NULL, image_rgb_16b, NULL);
    SDL_FreeSurface(image);

    printf("Converted Img is %dx%d, %dbpp\n", image_rgb_16b->w, image_rgb_16b->h, image_rgb_16b->format->BitsPerPixel);

    /* Resize image */
    /*SDL_Surface *image_rgb16b_resized = zoomSurface(image_rgb_16b, hw_surface->w, hw_surface->h);
    SDL_FreeSurface(image_rgb_16b);*/



    /****************/
    /* NEONS tests */
    /****************/

/******************************************     1 (neon transpose)      ********************************/
#if 0
    /* Print original values */
    /*const int x_offset = 4*20;
    const int y_offset = 4*20;
    uint16_t *line_y0 = (uint16_t *)image_rgb_16b->pixels + x_offset + (image_rgb_16b->w * y_offset);
    uint16_t *line_y1 = (uint16_t *)image_rgb_16b->pixels + x_offset + (image_rgb_16b->w * (y_offset + 1) );
    uint16_t *line_y2 = (uint16_t *)image_rgb_16b->pixels + x_offset + (image_rgb_16b->w * (y_offset + 2) );
    uint16_t *line_y3 = (uint16_t *)image_rgb_16b->pixels + x_offset + (image_rgb_16b->w * (y_offset + 3) );*/

    uint16_t line_y0[4] = {0, 1, 2, 3};
    uint16_t line_y1[4] = {4, 5, 6, 7};
    uint16_t line_y2[4] = {8, 9, 10, 11};
    uint16_t line_y3[4] = {12, 13, 14, 15};

    printf("\nOriginal 4x4 block:\n");
    printf("    %02d, %02d, %02d, %02d\n", line_y0[0], line_y0[1], line_y0[2], line_y0[3]);
    printf("    %02d, %02d, %02d, %02d\n", line_y1[0], line_y1[1], line_y1[2], line_y1[3]);
    printf("    %02d, %02d, %02d, %02d\n", line_y2[0], line_y2[1], line_y2[2], line_y2[3]);
    printf("    %02d, %02d, %02d, %02d\n", line_y3[0], line_y3[1], line_y3[2], line_y3[3]);


    /* NEON */
    uint16x4x4_t v_tmp;
    v_tmp.val[0] = vld1_u16(line_y0);
    v_tmp.val[1] = vld1_u16(line_y1);
    v_tmp.val[2] = vld1_u16(line_y2);
    v_tmp.val[3] = vld1_u16(line_y3);

    /*uint16x4_t d0 = vld1_u16(line_y0);
    uint16x4_t d1 = vld1_u16(line_y1);
    uint16x4_t d2 = vld1_u16(line_y2);
    uint16x4_t d3 = vld1_u16(line_y3);
    uint16x4x4_t v_tmp = {d0, d1, d2, d3};*/

    uint16_t out_line_y0[4], out_line_y1[4], out_line_y2[4], out_line_y3[4];
    vst4_lane_u16(out_line_y0, v_tmp, 0);
    vst4_lane_u16(out_line_y1, v_tmp, 1);
    vst4_lane_u16(out_line_y2, v_tmp, 2);
    vst4_lane_u16(out_line_y3, v_tmp, 3);


    printf("\nFinal 4x4 block:\n");
    printf("    %02d, %02d, %02d, %02d\n", out_line_y0[0], out_line_y0[1], out_line_y0[2], out_line_y0[3]);
    printf("    %02d, %02d, %02d, %02d\n", out_line_y1[0], out_line_y1[1], out_line_y1[2], out_line_y1[3]);
    printf("    %02d, %02d, %02d, %02d\n", out_line_y2[0], out_line_y2[1], out_line_y2[2], out_line_y2[3]);
    printf("    %02d, %02d, %02d, %02d\n", out_line_y3[0], out_line_y3[1], out_line_y3[2], out_line_y3[3]);
#endif
/**********************************************************************************/




/******************************************     1 bis (neon transpose inverse)      ********************************/
#if 0
    uint16_t line_y0[4] = {0, 1, 2, 3};
    uint16_t line_y1[4] = {4, 5, 6, 7};
    uint16_t line_y2[4] = {8, 9, 10, 11};
    uint16_t line_y3[4] = {12, 13, 14, 15};

    printf("\nOriginal 4x4 block:\n");
    /*printf("    %02d, %02d, %02d, %02d\n", line_y0[0], line_y0[1], line_y0[2], line_y0[3]);
    printf("    %02d, %02d, %02d, %02d\n", line_y1[0], line_y1[1], line_y1[2], line_y1[3]);
    printf("    %02d, %02d, %02d, %02d\n", line_y2[0], line_y2[1], line_y2[2], line_y2[3]);
    printf("    %02d, %02d, %02d, %02d\n", line_y3[0], line_y3[1], line_y3[2], line_y3[3]);*/
    disp4(line_y0); disp4(line_y1); disp4(line_y2); disp4(line_y3);


    /* NEON */
    /*uint16x4x4_t v_tmp;
    v_tmp.val[0] = vrev64_u16(vld1_u16(line_y3));
    v_tmp.val[1] = vrev64_u16(vld1_u16(line_y2));
    v_tmp.val[2] = vrev64_u16(vld1_u16(line_y1));
    v_tmp.val[3] = vrev64_u16(vld1_u16(line_y0));*/

    uint16x4_t d0 = vld1_u16(line_y3);
    uint16x4_t d1 = vld1_u16(line_y2);
    uint16x4_t d2 = vld1_u16(line_y1);
    uint16x4_t d3 = vld1_u16(line_y0);
    
    uint16x4x4_t v_tmp = {d0, d1, d2, d3};

    uint16_t out_line_y0[4], out_line_y1[4], out_line_y2[4], out_line_y3[4];
    vst4_lane_u16(out_line_y0, v_tmp, 3);
    vst4_lane_u16(out_line_y1, v_tmp, 2);
    vst4_lane_u16(out_line_y2, v_tmp, 1);
    vst4_lane_u16(out_line_y3, v_tmp, 0);


    printf("\nFinal 4x4 block:\n");
    /*printf("    %02d, %02d, %02d, %02d\n", out_line_y0[0], out_line_y0[1], out_line_y0[2], out_line_y0[3]);
    printf("    %02d, %02d, %02d, %02d\n", out_line_y1[0], out_line_y1[1], out_line_y1[2], out_line_y1[3]);
    printf("    %02d, %02d, %02d, %02d\n", out_line_y2[0], out_line_y2[1], out_line_y2[2], out_line_y2[3]);
    printf("    %02d, %02d, %02d, %02d\n", out_line_y3[0], out_line_y3[1], out_line_y3[2], out_line_y3[3]);*/
    disp4(out_line_y0); disp4(out_line_y1); disp4(out_line_y2); disp4(out_line_y3); 
#endif
/**********************************************************************************/




/******************************************     1 bis (neon rotate 270 CW)      ********************************/
#if 0
    const uint16_t line_y0[4] = {0, 1, 2, 3};
    const uint16_t line_y1[4] = {4, 5, 6, 7};
    const uint16_t line_y2[4] = {8, 9, 10, 11};
    const uint16_t line_y3[4] = {12, 13, 14, 15};


    printf("\nOriginal 4x4 block:\n");
    disp4(line_y0); disp4(line_y1); disp4(line_y2); disp4(line_y3);

    /* NEON */
    /*uint16x4x4_t v_tmp;
    v_tmp.val[0] = vld1_u16(line_y0);
    v_tmp.val[1] = vld1_u16(line_y1);
    v_tmp.val[2] = vld1_u16(line_y2);
    v_tmp.val[3] = vld1_u16(line_y3);*/

    uint16x4_t d0 = vld1_u16(line_y0);
    uint16x4_t d1 = vld1_u16(line_y1);
    uint16x4_t d2 = vld1_u16(line_y2);
    uint16x4_t d3 = vld1_u16(line_y3);
    
    uint16x4x4_t v_tmp = {d0, d1, d2, d3};

    uint16_t out_line_y0[4], out_line_y1[4], out_line_y2[4], out_line_y3[4];
    vst4_lane_u16(out_line_y0, v_tmp, 3);
    vst4_lane_u16(out_line_y1, v_tmp, 2);
    vst4_lane_u16(out_line_y2, v_tmp, 1);
    vst4_lane_u16(out_line_y3, v_tmp, 0);


    printf("Final 4x4 block:\n");
    disp4(out_line_y0); disp4(out_line_y1); disp4(out_line_y2); disp4(out_line_y3); 



    /*const uint16_t mat_src[] = {line_y0, line_y1, line_y2, line_y3};
    const uint16_t mat_dst[4*4];
    printf("\nmat src:\n");
    disp4x4(mat_src);

    fbtft_rotate_270cw_neon(mat_src, mat_dst, 4, 4);
    printf("mat dst:\n");
    disp4x4(mat_dst);*/

#endif
/**********************************************************************************/



/******************************************     1 bis (neon rotate 90 CW)      ********************************/
#if 0
    uint16_t line_y0[4] = {0, 1, 2, 3};
    uint16_t line_y1[4] = {4, 5, 6, 7};
    uint16_t line_y2[4] = {8, 9, 10, 11};
    uint16_t line_y3[4] = {12, 13, 14, 15};

    printf("\nOriginal 4x4 block:\n");
    disp4(line_y0); disp4(line_y1); disp4(line_y2); disp4(line_y3);


    /* NEON */
    /*uint16x4x4_t v_tmp;
    v_tmp.val[0] = vld1_u16(line_y3);
    v_tmp.val[1] = vld1_u16(line_y2);
    v_tmp.val[2] = vld1_u16(line_y1);
    v_tmp.val[3] = vld1_u16(line_y0);*/

    uint16x4_t d0 = vld1_u16(line_y3);
    uint16x4_t d1 = vld1_u16(line_y2);
    uint16x4_t d2 = vld1_u16(line_y1);
    uint16x4_t d3 = vld1_u16(line_y0);
    
    uint16x4x4_t v_tmp = {d0, d1, d2, d3};

    uint16_t out_line_y0[4], out_line_y1[4], out_line_y2[4], out_line_y3[4];
    vst4_lane_u16(out_line_y0, v_tmp, 0);
    vst4_lane_u16(out_line_y1, v_tmp, 1);
    vst4_lane_u16(out_line_y2, v_tmp, 2);
    vst4_lane_u16(out_line_y3, v_tmp, 3);


    printf("\nFinal 4x4 block:\n");
    disp4(out_line_y0); disp4(out_line_y1); disp4(out_line_y2); disp4(out_line_y3); 
#endif
/**********************************************************************************/








/******************************************     2     ********************************/
#if 0
    uint16_t src[4*4]={
            0,1,2,3,
            4,5,6,7,
            8,9,10,11,
            12,13,14,15
        };
    
    disp4x4(src);
    transpose4x4(src);
    disp4x4(src);    

#endif
/**********************************************************************************/















/******************************************     3     ********************************/
#if 0
    /* Vars */
    int h = image_rgb_16b->h, w = image_rgb_16b->w;
    uint16_t * p = (uint16_t *)image_rgb_16b->pixels;
    uint16x4x4_t v_tmp;
    int y, x;

    /* Create new transposed image */
    SDL_Surface *image_rgb_16b_transposed = SDL_CreateRGBSurface(SDL_SWSURFACE, h, w, 16, 0,0,0,0);
    uint16_t * p2 = (uint16_t *)image_rgb_16b_transposed->pixels;
    printf("Transposed Img is %dx%d, %dbpp, %d pitch\n", image_rgb_16b_transposed->w, 
        image_rgb_16b_transposed->h, image_rgb_16b_transposed->format->BitsPerPixel,
        image_rgb_16b_transposed->pitch);

    /* Main loop */
    for (y=0; y<h; y+=4){
        for (x=0; x<w; x+=4){

            /* Neon Load */
            v_tmp.val[0] = vld1_u16(p + (y+0)*w + x );
            v_tmp.val[1] = vld1_u16(p + (y+1)*w + x );
            v_tmp.val[2] = vld1_u16(p + (y+2)*w + x );
            v_tmp.val[3] = vld1_u16(p + (y+3)*w + x );

            /* Neon store (4 interleaved) */
            vst4_lane_u16(p2 + (x+0)*h + y, v_tmp, 0);
            vst4_lane_u16(p2 + (x+1)*h + y, v_tmp, 1);
            vst4_lane_u16(p2 + (x+2)*h + y, v_tmp, 2);
            vst4_lane_u16(p2 + (x+3)*h + y, v_tmp, 3);

            /*vst4_lane_u16(p + w*(y+0) + x, v_tmp, 0);
            vst4_lane_u16(p + w*(y+1) + x, v_tmp, 1);
            vst4_lane_u16(p + w*(y+2) + x, v_tmp, 2);
            vst4_lane_u16(p + w*(y+3) + x, v_tmp, 3);*/


            /*printf("\n4x4 block at (%d, %d) put back at (%d, %d):\n", y, x, x, y);        
            printf("    %d, %d, %d, %d    ->    %d, %d, %d, %d\n", 
                    *(p + w*(y+0) + x), *(p + w*(y+0) + x+1), *(p + w*(y+0) + x+2), *(p + w*(y+0) + x+3),
                    *(p2 + h*(x+0) + y),*(p2 + h*(x+0) + y+1),*(p2 + h*(x+0) + y+2),*(p2 + h*(x+0) + y+3)
                    );
            printf("    %d, %d, %d, %d    ->    %d, %d, %d, %d\n", 
                    *(p + w*(y+1) + x), *(p + w*(y+1) + x+1), *(p + w*(y+1) + x+2), *(p + w*(y+1) + x+3),
                    *(p2 + h*(x+1) + y),*(p2 + h*(x+1) + y+1),*(p2 + h*(x+1) + y+2),*(p2 + h*(x+1) + y+3)
                    );
            printf("    %d, %d, %d, %d    ->    %d, %d, %d, %d\n", 
                    *(p + w*(y+2) + x), *(p + w*(y+2) + x+1), *(p + w*(y+2) + x+2), *(p + w*(y+2) + x+3),
                    *(p2 + h*(x+2) + y),*(p2 + h*(x+2) + y+1),*(p2 + h*(x+2) + y+2),*(p2 + h*(x+2) + y+3)
                    );
            printf("    %d, %d, %d, %d    ->    %d, %d, %d, %d\n", 
                    *(p + w*(y+3) + x), *(p + w*(y+3) + x+1), *(p + w*(y+3) + x+2), *(p + w*(y+3) + x+3),
                    *(p2 + h*(x+3) + y),*(p2 + h*(x+3) + y+1),*(p2 + h*(x+3) + y+2),*(p2 + h*(x+3) + y+3)
                    );*/

        }
    }





    /* Transpose by block of 4x4 à la mano */
    SDL_Surface *image_rgb_16b_transposed_4x4mano = SDL_CreateRGBSurface(SDL_SWSURFACE, h, w, 16, 0,0,0,0);
    uint16_t * p4 = (uint16_t *)image_rgb_16b_transposed_4x4mano->pixels;
    int N = 4;
    for (y=0; y<h; y+=4){
        for (x=0; x<w; x+=4){

            /* mano 4x4 transpose */
            /*uint16_t tmp_tansp[4*4*sizeof(uint16_t)];
            transpose4x4(tmp_tansp, &p[AT(y,x)]);*/

            uint16_t tmp_tansp[4*4*sizeof(uint16_t)];
            memcpy( &tmp_tansp[AT(0,0)],  &p[ w*(y+0) + (x+0) ], 4*sizeof(uint16_t));
            memcpy( &tmp_tansp[AT(1,0)],  &p[ w*(y+1) + (x+0) ], 4*sizeof(uint16_t));
            memcpy( &tmp_tansp[AT(2,0)],  &p[ w*(y+2) + (x+0) ], 4*sizeof(uint16_t));
            memcpy( &tmp_tansp[AT(3,0)],  &p[ w*(y+3) + (x+0) ], 4*sizeof(uint16_t));

            /*printf("\nreal data 4x4 at (%d, %d):\n", y, x);
            disp4(&p[ w*(y+0) + (x+0) ]);
            disp4(&p[ w*(y+1) + (x+0) ]);
            disp4(&p[ w*(y+2) + (x+0) ]);
            disp4(&p[ w*(y+3) + (x+0) ]);*/

            /*printf("block 4x4 at (%d, %d):\n", y, x);
            disp4x4(tmp_tansp);*/

            //printf("block transposed 4x4 at (%d, %d):\n", y, x);
            transpose4x4(tmp_tansp);
            //disp4x4(tmp_tansp);            

            memcpy( &p4[ h*(x+0) + y ], &tmp_tansp[AT(0,0)], 4*sizeof(uint16_t));
            memcpy( &p4[ h*(x+1) + y ], &tmp_tansp[AT(1,0)], 4*sizeof(uint16_t));
            memcpy( &p4[ h*(x+2) + y ], &tmp_tansp[AT(2,0)], 4*sizeof(uint16_t));
            memcpy( &p4[ h*(x+3) + y ], &tmp_tansp[AT(3,0)], 4*sizeof(uint16_t));

            /*memcpy( &p4[ w*(y+0) + (x+0) ], &tmp_tansp[AT(0,0)], 4*sizeof(uint16_t));
            memcpy( &p4[ w*(y+1) + (x+0) ], &tmp_tansp[AT(1,0)], 4*sizeof(uint16_t));
            memcpy( &p4[ w*(y+2) + (x+0) ], &tmp_tansp[AT(2,0)], 4*sizeof(uint16_t));
            memcpy( &p4[ w*(y+3) + (x+0) ], &tmp_tansp[AT(3,0)], 4*sizeof(uint16_t));*/

            /*printf("\nreal transposed data 4x4 at (%d, %d):\n", y, x);
            disp4(&p4[ w*(y+0) + (x+0) ]);
            disp4(&p4[ w*(y+1) + (x+0) ]);
            disp4(&p4[ w*(y+2) + (x+0) ]);
            disp4(&p4[ w*(y+3) + (x+0) ]);*/
            
            /*SDL_BlitSurface(image_rgb_16b_transposed_4x4mano, NULL, hw_surface, NULL);
            SDL_Flip(hw_surface);
            SDL_Delay(100);*/
        }
    }





    /* Create new transposed image pour faire à la mano */
    SDL_Surface *image_rgb_16b_transposed_mano = SDL_CreateRGBSurface(SDL_SWSURFACE, h, w, 16, 0,0,0,0);
    uint16_t * p3 = (uint16_t *)image_rgb_16b_transposed_mano->pixels;
    printf("Transposed Img mano is %dx%d, %dbpp\n", image_rgb_16b_transposed_mano->w, image_rgb_16b_transposed_mano->h, image_rgb_16b_transposed_mano->format->BitsPerPixel);

    /* Main loop */
    for (y=0; y<h; y++){
        for (x=0; x<w; x++){
            p3[x*h + y] = p[y*w+x];
        }
    }





    /* Debug: pixel values compared to a la mano */
    /*for (y=0; y<h; y+=4){
        for (x=0; x<w; x+=4){

            printf("\n4x4 block at (%d, %d)\n", y, x);        
            printf("    %d, %d, %d, %d    ->    %d, %d, %d, %d\n", 
                    *(p3 + w*(y+0) + x), *(p3 + w*(y+0) + x+1), *(p3 + w*(y+0) + x+2), *(p3 + w*(y+0) + x+3),
                    *(p2 + w*(y+0) + x), *(p2 + w*(y+0) + x+1), *(p2 + w*(y+0) + x+2), *(p2 + w*(y+0) + x+3)
                    );
            printf("    %d, %d, %d, %d    ->    %d, %d, %d, %d\n", 
                    *(p3 + w*(y+1) + x), *(p3 + w*(y+1) + x+1), *(p3 + w*(y+1) + x+2), *(p3 + w*(y+1) + x+3),
                    *(p2 + w*(y+1) + x), *(p2 + w*(y+1) + x+1), *(p2 + w*(y+1) + x+2), *(p2 + w*(y+1) + x+3)
                    );
            printf("    %d, %d, %d, %d    ->    %d, %d, %d, %d\n", 
                    *(p3 + w*(y+2) + x), *(p3 + w*(y+2) + x+1), *(p3 + w*(y+2) + x+2), *(p3 + w*(y+2) + x+3),
                    *(p2 + w*(y+2) + x), *(p2 + w*(y+2) + x+1), *(p2 + w*(y+2) + x+2), *(p2 + w*(y+2) + x+3)
                    );
            printf("    %d, %d, %d, %d    ->    %d, %d, %d, %d\n", 
                    *(p3 + w*(y+3) + x), *(p3 + w*(y+3) + x+1), *(p3 + w*(y+3) + x+2), *(p3 + w*(y+3) + x+3),
                    *(p2 + w*(y+3) + x), *(p2 + w*(y+3) + x+1), *(p2 + w*(y+3) + x+2), *(p2 + w*(y+3) + x+3)
                    );

        }
    }*/










    
    /* Blit image */
    SDL_Surface * imgs_to_blit[] =  {
                                        image_rgb_16b,
                                        image_rgb_16b_transposed_4x4mano,
                                        image_rgb_16b_transposed, 
                                        image_rgb_16b_transposed_mano
                                    };
    int nb_images_to_blit = sizeof(imgs_to_blit)/sizeof(imgs_to_blit[0]);
    int cur_idx_blit = 1;                                                


    SDL_BlitSurface(imgs_to_blit[cur_idx_blit], NULL, hw_surface, NULL);
    //SDL_BlitSurface(image_rgb_16b, NULL, hw_surface, NULL);
    //SDL_BlitSurface(image_rgb_16b_transposed, NULL, hw_surface, NULL);
    //SDL_BlitSurface(image_rgb_16b_transposed_mano, NULL, hw_surface, NULL);

    /// -------- Main loop ---------
    while (!stop_menu_loop)
    {
        /// -------- Handle Keyboard Events ---------
        while (SDL_PollEvent(&event))
        switch(event.type)
        {
            case SDL_QUIT:
                stop_menu_loop = 1;
        break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
                case SDLK_m:
                    stop_menu_loop = 1;
                    res = ERROR_MANUAL_FAIL;
                    break;

                case SDLK_q:
                case SDLK_n:
                case SDLK_ESCAPE:
                    stop_menu_loop = 1;
                    res = 0;
                    break;

                case SDLK_l:
                    cur_idx_blit = cur_idx_blit?cur_idx_blit-1:nb_images_to_blit-1;
                    SDL_BlitSurface(imgs_to_blit[cur_idx_blit], NULL, hw_surface, NULL);
                    break;

                case SDLK_r:
                    cur_idx_blit = (cur_idx_blit+1)%nb_images_to_blit;
                    SDL_BlitSurface(imgs_to_blit[cur_idx_blit], NULL, hw_surface, NULL);
                    break;

                default:                
                    break;
            }
        }

        /* To investigate but with Buildroot, we need this: */
        SDL_Flip(hw_surface);

        /* Sleep for some time */
        SDL_Delay(1000/60);
    }

    /* free */ 
    SDL_FreeSurface(image_rgb_16b);
    SDL_FreeSurface(image_rgb_16b_transposed);
#endif //0
/*************************************************************************************/















/******************************************     3 bis     ********************************/
#if 0
    /* Vars */
    int h = image_rgb_16b->h, w = image_rgb_16b->w;
    uint16_t * p = (uint16_t *)image_rgb_16b->pixels;
    int y, x;

    /* Rotate 90 CW */
    SDL_Surface *image_rgb_16b_rotated_90_neon = SDL_CreateRGBSurface(SDL_SWSURFACE, h, w, 16, 0,0,0,0);
    uint16_t * p2 = (uint16_t *)image_rgb_16b_rotated_90_neon->pixels;
    fbtft_rotate_90cw_neon(p, p2, w, h);

    /* Transpose inv */
    SDL_Surface *image_rgb_16b_transp_inv_neon = SDL_CreateRGBSurface(SDL_SWSURFACE, h, w, 16, 0,0,0,0);
    uint16_t * p3 = (uint16_t *)image_rgb_16b_transp_inv_neon->pixels;
    fbtft_transpose_inv_neon(p, p3, w, h);

    /* Rotate 270 CW */
    SDL_Surface *image_rgb_16b_rotated_270_neon = SDL_CreateRGBSurface(SDL_SWSURFACE, h, w, 16, 0,0,0,0);
    uint16_t * p4 = (uint16_t *)image_rgb_16b_rotated_270_neon->pixels;
    fbtft_rotate_270cw_neon(p, p4, w, h);
    
    /* Blit image */
    SDL_Surface * imgs_to_blit[] =  {
                                        image_rgb_16b,
                                        image_rgb_16b_rotated_270_neon,
                                        image_rgb_16b_transp_inv_neon,
                                        image_rgb_16b_rotated_90_neon,
                                    };
    int nb_images_to_blit = sizeof(imgs_to_blit)/sizeof(imgs_to_blit[0]);
    int cur_idx_blit = 1;                                                


    SDL_BlitSurface(imgs_to_blit[cur_idx_blit], NULL, hw_surface, NULL);

    /// -------- Main loop ---------
    while (!stop_menu_loop)
    {
        /// -------- Handle Keyboard Events ---------
        while (SDL_PollEvent(&event))
        switch(event.type)
        {
            case SDL_QUIT:
                stop_menu_loop = 1;
        break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
                case SDLK_m:
                    stop_menu_loop = 1;
                    res = ERROR_MANUAL_FAIL;
                    break;

                case SDLK_q:
                case SDLK_n:
                case SDLK_ESCAPE:
                    stop_menu_loop = 1;
                    res = 0;
                    break;

                case SDLK_l:
                    cur_idx_blit = cur_idx_blit?cur_idx_blit-1:nb_images_to_blit-1;
                    SDL_BlitSurface(imgs_to_blit[cur_idx_blit], NULL, hw_surface, NULL);
                    printf("idx = %d\n", cur_idx_blit);
                    break;

                case SDLK_r:
                    cur_idx_blit = (cur_idx_blit+1)%nb_images_to_blit;
                    SDL_BlitSurface(imgs_to_blit[cur_idx_blit], NULL, hw_surface, NULL);
                    break;

                default:                
                    break;
            }
        }

        /* To investigate but with Buildroot, we need this: */
        SDL_Flip(hw_surface);

        /* Sleep for some time */
        SDL_Delay(1000/60);
    }

    /* free */ 
    SDL_FreeSurface(image_rgb_16b);
    SDL_FreeSurface(image_rgb_16b_rotated_90_neon);
#endif //0
/*************************************************************************************/















/******************************************     3 bis (non squared)    ********************************/
#if 0
    /* Vars */
    int w = image_rgb_16b->w/2, h = image_rgb_16b->h;
    SDL_Surface *image_rgb_16b_notsquare = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 16, 0,0,0,0);
    //SDL_Rect dst_rect={ (image_rgb_16b->w-w)/2, 0, w, h };
    SDL_BlitSurface(image_rgb_16b, NULL, image_rgb_16b_notsquare, NULL);
    uint16_t * p = (uint16_t *)image_rgb_16b_notsquare->pixels;
    int y, x;

    /* Rotate 90 CW */
    SDL_Surface *image_rgb_16b_rotated_90_neon = SDL_CreateRGBSurface(SDL_SWSURFACE, h, w, 16, 0,0,0,0);
    uint16_t * p2 = (uint16_t *)image_rgb_16b_rotated_90_neon->pixels;
    fbtft_rotate_90cw_neon(p, p2, w, h);

    /* Transpose inv */
    SDL_Surface *image_rgb_16b_transp_inv_neon = SDL_CreateRGBSurface(SDL_SWSURFACE, h, w, 16, 0,0,0,0);
    uint16_t * p3 = (uint16_t *)image_rgb_16b_transp_inv_neon->pixels;
    fbtft_transpose_inv_neon(p, p3, w, h);

    /* Rotate 270 CW */
    SDL_Surface *image_rgb_16b_rotated_270_neon = SDL_CreateRGBSurface(SDL_SWSURFACE, h, w, 16, 0,0,0,0);
    uint16_t * p4 = (uint16_t *)image_rgb_16b_rotated_270_neon->pixels;
    fbtft_rotate_270cw_neon(p, p4, w, h);

    /* Transpose */
    SDL_Surface *image_rgb_16b_transp_neon = SDL_CreateRGBSurface(SDL_SWSURFACE, h, w, 16, 0,0,0,0);
    uint16_t * p5 = (uint16_t *)image_rgb_16b_transp_neon->pixels;
    fbtft_transpose_neon(p, p5, w, h);
    
    /* Blit image */
    SDL_Surface * imgs_to_blit[] =  {
                                        image_rgb_16b_notsquare,
                                        image_rgb_16b_rotated_270_neon,
                                        image_rgb_16b_rotated_90_neon,
                                        image_rgb_16b_transp_neon,
                                        image_rgb_16b_transp_inv_neon
                                    };
    int nb_images_to_blit = sizeof(imgs_to_blit)/sizeof(imgs_to_blit[0]);
    int cur_idx_blit = 0;                                                


    SDL_BlitSurface(imgs_to_blit[cur_idx_blit], NULL, hw_surface, NULL);

    /// -------- Main loop ---------
    while (!stop_menu_loop)
    {
        /// -------- Handle Keyboard Events ---------
        while (SDL_PollEvent(&event))
        switch(event.type)
        {
            case SDL_QUIT:
                stop_menu_loop = 1;
        break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
                case SDLK_m:
                    stop_menu_loop = 1;
                    res = ERROR_MANUAL_FAIL;
                    break;

                case SDLK_q:
                case SDLK_n:
                case SDLK_ESCAPE:
                    stop_menu_loop = 1;
                    res = 0;
                    break;

                case SDLK_l:
                    cur_idx_blit = cur_idx_blit?cur_idx_blit-1:nb_images_to_blit-1;
                    SDL_BlitSurface(imgs_to_blit[cur_idx_blit], NULL, hw_surface, NULL);
                    printf("idx = %d\n", cur_idx_blit);
                    break;

                case SDLK_r:
                    cur_idx_blit = (cur_idx_blit+1)%nb_images_to_blit;
                    SDL_BlitSurface(imgs_to_blit[cur_idx_blit], NULL, hw_surface, NULL);
                    break;

                default:                
                    break;
            }
        }

        /* To investigate but with Buildroot, we need this: */
        SDL_Flip(hw_surface);

        /* Sleep for some time */
        SDL_Delay(1000/60);
    }

    /* free */ 
    SDL_FreeSurface(image_rgb_16b);
    SDL_FreeSurface(image_rgb_16b_rotated_90_neon);
#endif //0
/*************************************************************************************/














/******************************************     4  (perfs tests 240x240)    ********************************/
#if 0
//* Vars */
int h = image_rgb_16b->h, w = image_rgb_16b->w;
uint16_t * p = (uint16_t *)image_rgb_16b->pixels;
SDL_Surface *image_rgb_16b_transposed = SDL_CreateRGBSurface(SDL_SWSURFACE, h, w, 16, 0,0,0,0);
uint16_t * p2 = (uint16_t *)image_rgb_16b_transposed->pixels;

int i;
uint32_t now = SDL_GetTicks();


/* Saved perfs for 10000 iterations: */
/*  Rotate square optimized with memcpy: 2381ms
*   Rotate square optimized exported: 9847ms
*   Translate soft: 8645ms
*   Translate soft a la mano 4x4: 11110ms
*   Translate neon: 7800ms
*/



#define ITERATIONS  10000
printf("\n");

/* Rotate square optimized with memcpy*/
memcpy((uint8_t*) p2, (uint8_t*) p, h*w*sizeof(uint16_t));
for (i=0; i<ITERATIONS; i++) fbtft_rotate_soft_square(p2, h, 90);
printf("Rotate square optimized with memcpy: %dms\n", SDL_GetTicks()-now);
now = SDL_GetTicks();

/* Rotate square optimized exported*/
for (i=0; i<ITERATIONS; i++) fbtft_rotate_soft_square_export(p, p2, h, 90);
printf("Rotate square optimized exported: %dms\n", SDL_GetTicks()-now);
now = SDL_GetTicks();

/* Translate soft */
for (i=0; i<ITERATIONS; i++) fbtft_transpose(p, p2, w, h);
printf("Translate soft: %dms\n", SDL_GetTicks()-now);
now = SDL_GetTicks();

/* Translate soft a la mano 4x4 */
for (i=0; i<ITERATIONS; i++) fbtft_transpose_mano4x4(p, p2, w, h);
printf("Translate soft a la mano 4x4: %dms\n", SDL_GetTicks()-now);
now = SDL_GetTicks();

#ifdef __ARM_FP
/* Translate neon */
for (i=0; i<ITERATIONS; i++) fbtft_transpose_neon(p, p2, w, h);
printf("Translate neon: %dms\n", SDL_GetTicks()-now);
now = SDL_GetTicks();
#endif //__ARM_FP

#endif
/*************************************************************************************/













/******************************************     4 bis (optims neon)    ********************************/
#if 1
//* Vars */
int h = image_rgb_16b->h, w = image_rgb_16b->w;
uint16_t * p = (uint16_t *)image_rgb_16b->pixels;
SDL_Surface *image_rgb_16b_transposed = SDL_CreateRGBSurface(SDL_SWSURFACE, h, w, 16, 0,0,0,0);
uint16_t * p2 = (uint16_t *)image_rgb_16b_transposed->pixels;

int i;
uint32_t now = SDL_GetTicks();


/* Saved perfs for 10000 iterations: */
/*  Rotate square optimized with memcpy: 2381ms
*   Rotate square optimized exported: 9847ms
*   Translate soft: 8645ms
*   Translate soft a la mano 4x4: 11110ms
*   Translate neon: 7800ms
*/



#define ITERATIONS  3000
printf("\n");

#ifdef __ARM_FP

/* Rotate 270 with prefetch neon */
for (i=0; i<ITERATIONS; i++) fbtft_rotate_270cw_neon_prefetch2(p, p2, w, h);
printf("Rotate 270 cw neon with prefetch2: %dms\n", SDL_GetTicks()-now);
now = SDL_GetTicks();

/* Rotate 270 neon */
for (i=0; i<ITERATIONS; i++) fbtft_rotate_270cw_neon(p, p2, w, h);
printf("Rotate 270 cw neon: %dms\n", SDL_GetTicks()-now);
now = SDL_GetTicks();

/* Rotate 270 with prefetch neon */
for (i=0; i<ITERATIONS; i++) fbtft_rotate_270cw_neon_prefetch(p, p2, w, h);
printf("Rotate 270 cw neon with prefetch: %dms\n", SDL_GetTicks()-now);
now = SDL_GetTicks();

#endif //__ARM_FP

#endif
/*************************************************************************************/














/******************************************     4  (perfs tests 320x240)    ********************************/
#if 0
//* Vars */
int  w = 320, h = 240;
SDL_Surface *image_tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, h, w, 16, 0,0,0,0);
SDL_Rect dst_rect = {0,0,w,h};
SDL_BlitSurface(image_rgb_16b, NULL, image_tmp, &dst_rect);
uint16_t * p = (uint16_t *)image_tmp->pixels;
SDL_Surface *image_rgb_16b_transposed = SDL_CreateRGBSurface(SDL_SWSURFACE, h, w, 16, 0,0,0,0);
uint16_t * p2 = (uint16_t *)image_rgb_16b_transposed->pixels;
int i;
uint32_t now = SDL_GetTicks();



/* Saved perfs for 10000 iterations: */
/*  Rotate square optimized: 10127ms for 320x320 though !
/*  Rotate square optimized with memcpy: 11734ms for 320x320 though !
*   Rotate square optimized exported: 21971ms for 320x320 though !
*   Translate soft: 8634ms
*   Translate soft a la mano 4x4: 11097ms
*   Translate neon: 7736ms
*/


#define ITERATIONS  10000
printf("\n");

/* Rotate square optimized */
memcpy((uint8_t*) p2, (uint8_t*) p, h*w*sizeof(uint16_t));
for (i=0; i<ITERATIONS; i++) fbtft_rotate_soft_square(p2, h, 90);
printf("Rotate square optimized: %dms\n", SDL_GetTicks()-now);
now = SDL_GetTicks();

/* Rotate square optimized exported*/
for (i=0; i<ITERATIONS; i++) fbtft_rotate_soft_square_export(p, p2, h, 90);
printf("Rotate square optimized exported: %dms\n", SDL_GetTicks()-now);
now = SDL_GetTicks();

/* Translate soft */
for (i=0; i<ITERATIONS; i++) fbtft_transpose(p, p2, w, h);
printf("Translate soft: %dms\n", SDL_GetTicks()-now);
now = SDL_GetTicks();

/* Translate soft a la mano 4x4 */
for (i=0; i<ITERATIONS; i++) fbtft_transpose_mano4x4(p, p2, w, h);
printf("Translate soft a la mano 4x4: %dms\n", SDL_GetTicks()-now);
now = SDL_GetTicks();

#ifdef __ARM_FP
/* Translate neon */
for (i=0; i<ITERATIONS; i++) fbtft_transpose_neon(p, p2, w, h);
printf("Translate neon: %dms\n", SDL_GetTicks()-now);
now = SDL_GetTicks();
#endif //__ARM_FP

#endif//0
/*************************************************************************************/















    return res;
}
