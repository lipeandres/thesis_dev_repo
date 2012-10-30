#ifndef realfft_split_h
#define realfft_split_h


void init_realfft_split ();
void realfft_split (float *data);

inline float fftreal (int i);
inline float fftimag (int i);
inline float fftangle (int i);
inline float fftabs2 (int i);
inline float fftabs (int i);

#define pi_2 1.57079632679489661923F

#define abs2(v)  (v.r*v.r + v.i*v.i)

#define angle(v) atan2f(v.i,v.r)

#define cmult(c,a,b) c.r=a.r*b.r - a.i*b.i, \
                     c.i=a.r*b.i + a.i*b.r

#define csub(c,a,b)  c.r=a.r - b.r, \
                     c.i=a.i - b.i

#define cadd(c,a,b)  c.r=a.r + b.r, \
                     c.i=a.i + b.i

#define N  2048
#define N2 (N/2)

typedef struct
{
  float r;
  float i;
} complex;

#endif
