//
//                       FFT library
//
//  (one-dimensional complex and real FFTs for array 
//  lengths of 2^n)
//
//      Author: Toth Laszlo (tothl@inf.u-szeged.hu)
//  
//      Research Group on Artificial Intelligence
//  H-6720 Szeged, Aradi vertanuk tere 1, Hungary
//      
//      Last modified: 97.05.29
/////////////////////////////////////////////////////////
#include <math.h>
#include <realfft_split.h>

const float pi = M_PI;

/////////////////////////////////////////////////////////
// Sorensen in-place split-radix FFT for real values
// data: array of floats:
// re(0),re(1),re(2),...,re(size-1)
// 
// output:
// re(0),re(1),re(2),...,re(size/2),im(size/2-1),...,im(1)
// normalized by array length
//
// Source: 
// Sorensen et al: Real-Valued Fast Fourier Transform Algorithms,
// IEEE Trans. ASSP, ASSP-35, No. 6, June 1987

float isqrt2;
float *fft_data;

#if 1

#include "realfft_tables.c"
void
init_realfft_split ()
{
}

#else
float cos_cc1[N / 4];
float sin_ss1[N / 4];
float cos_cc3[N / 4];
float sin_ss3[N / 4];

void
init_realfft_split ()
{

  long k, j, n2, n8, ndx;
  float a3, a, e;

  isqrt2 = 1. / sqrt (2.0);

  ndx = 0;
   /*----------------------*/
  //L shaped butterflies
  n2 = 2;
  for (k = N; k > 2; k >>= 1)
    {
      n2 <<= 1;
      n8 = n2 >> 3;
      e = 2.F * pi / (float) (n2);

      a = e;
      for (j = 2; j <= n8; j++)
        {
          a3 = 3.F * a;
          //printf("%d %d cosf(%f)=%f\n",ndx,j,a,cosf(a));
          cos_cc1[ndx] = cosf (a);
          sin_ss1[ndx] = sinf (a);
          cos_cc3[ndx] = cosf (a3);
          sin_ss3[ndx] = sinf (a3);
          a = j * e;
          ndx++;
        }
    }
}
#endif

void
realfft_split (float *data)
{

  long i, j, k, i5, i6, i7, i8, i0, id, i1, i2, i3, i4, n2, n4, n8, ndx;
  float t1, t2, t3, t4, t5, t6, ss1, ss3, cc1, cc3;

  n4 = N - 1;
  ndx = 0;
  fft_data = data;

  //data shuffling
  for (i = 0, j = 0, n2 = N / 2; i < n4; i++)
    {
      if (i < j)
        {
          t1 = data[j];
          data[j] = data[i];
          data[i] = t1;
        }
      k = n2;
      while (k <= j)
        {
          j -= k;
          k >>= 1;
        }
      j += k;
    }

/*----------------------*/

  //length two butterflies  
  i0 = 0;
  id = 4;
  do
    {
      for (; i0 < n4; i0 += id)
        {
          i1 = i0 + 1;
          t1 = data[i0];
          data[i0] = t1 + data[i1];
          data[i1] = t1 - data[i1];
        }
      id <<= 1;
      i0 = id - 2;
      id <<= 1;
    }
  while (i0 < n4);

   /*----------------------*/
  //L shaped butterflies
  n2 = 2;
  for (k = N; k > 2; k >>= 1)
    {
      n2 <<= 1;
      n4 = n2 >> 2;
      n8 = n2 >> 3;
      i1 = 0;
      id = n2 << 1;
      do
        {
          for (; i1 < N; i1 += id)
            {
              i2 = i1 + n4;
              i3 = i2 + n4;
              i4 = i3 + n4;
              t1 = data[i4] + data[i3];
              data[i4] -= data[i3];
              data[i3] = data[i1] - t1;
              data[i1] += t1;
              if (n4 != 1)
                {
                  i0 = i1 + n8;
                  i2 += n8;
                  i3 += n8;
                  i4 += n8;
                  t1 = (data[i3] + data[i4]) * isqrt2;
                  t2 = (data[i3] - data[i4]) * isqrt2;
                  data[i4] = data[i2] - t1;
                  data[i3] = -data[i2] - t1;
                  data[i2] = data[i0] - t2;
                  data[i0] += t2;
                }
            }
          id <<= 1;
          i1 = id - n2;
          id <<= 1;
        }
      while (i1 < N);

      for (j = 2; j <= n8; j++)
        {
          cc1 = cos_cc1[ndx];
          ss1 = sin_ss1[ndx];
          cc3 = cos_cc3[ndx];
          ss3 = sin_ss3[ndx];

          ndx++;

          i = 0;
          id = n2 << 1;

          do
            {
              for (; i < N; i += id)
                {
                  i1 = i + j - 1;
                  i2 = i1 + n4;
                  i3 = i2 + n4;
                  i4 = i3 + n4;
                  i5 = i + n4 - j + 1;
                  i6 = i5 + n4;
                  i7 = i6 + n4;
                  i8 = i7 + n4;
                  t1 = data[i3] * cc1 + data[i7] * ss1;
                  t2 = data[i7] * cc1 - data[i3] * ss1;
                  t3 = data[i4] * cc3 + data[i8] * ss3;
                  t4 = data[i8] * cc3 - data[i4] * ss3;
                  t5 = t1 + t3;
                  t6 = t2 + t4;
                  t3 = t1 - t3;
                  t4 = t2 - t4;
                  t2 = data[i6] + t6;
                  data[i3] = t6 - data[i6];
                  data[i8] = t2;
                  t2 = data[i2] - t3;
                  data[i7] = -data[i2] - t3;
                  data[i4] = t2;
                  t1 = data[i1] + t5;
                  data[i6] = data[i1] - t5;
                  data[i1] = t1;
                  t1 = data[i5] + t4;
                  data[i5] -= t4;
                  data[i2] = t1;
                }
              id <<= 1;
              i = id - n2;
              id <<= 1;
            }
          while (i < N);
        }
    }
}

inline float
fftreal (int i)
{
  return fft_data[i];
}

inline float
fftimag (int i)
{
  return fft_data[N - i];
}

inline float
fftangle (int i)
{
  return atan2f (fft_data[N - i], fft_data[i]);
}

inline float
fftabs2 (int n)
{
  float r = fft_data[n];
  float i = fft_data[N - n];
  return (r * r + i * i);
}

inline float
fftabs (int n)
{
  return sqrt (fftabs2 (n));
}

#if 0

#include <stdio.h>

int print_array(float* v, int sz)
{
  int i;
  printf("{\n");
  
  for(i=0; i<sz; i++ )
  {
    printf("%2.26f,\n",v[i]);
  }
  printf("};\n");
  
  return 0;
}

int
main ()
{
  init_realfft_split ();

  printf("\nfloat cos_cc1[N / 4] =\n");
  print_array(cos_cc1, N/4);
  
  printf("\nfloat sin_ss1[N / 4] =\n");
  print_array(sin_ss1, N/4);
  
  printf("\nfloat cos_cc3[N / 4] =\n");
  print_array(cos_cc3, N/4);
  
  printf("\nfloat sin_ss3[N / 4] =\n");
  print_array(sin_ss3, N/4);
  
  
  return 0;
}  
#endif

#if 0

float cos_cc1[N / 4];
float sin_ss1[N / 4];
float cos_cc3[N / 4];
float sin_ss3[N / 4];


#include <stdio.h>

int
main ()
{
  int i;
  float data[N * 2];
  float *real = data;
  float *imag = &data[N / 2];

  for (i = 0; i < N; i++)
    data[i] = i;

  init_realfft_split ();
  realfft_split (data);

  printf ("%d %f %f\n ", 0, real[0], 0);

  for (i = 1; i < N / 2; i++)
    printf ("%d %f %f\n ", i, real[i], imag[N / 2 - i]);

  return 0;
}
#endif
