#ifndef FFT_H
#define FFT_H

#include <complex.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define PI 3.14159265358979
#define NREK 4

void fft2(double complex *l, double complex *yf, uint64_t N);
void ifft2(double complex *l, double complex *yf, uint64_t N);
void ifft(double complex *l, double complex *yf, uint64_t N);
void fft(double complex *l, double complex *yf, uint64_t N);
void idft(double complex *l, double complex *yf, uint64_t N);
void dft(double complex *l, double complex *yf, uint64_t N);
double complex* fft3(double complex *x, double complex *y, uint64_t N);
double complex* ifft3(double complex *x, double complex *y, uint64_t N);


#endif