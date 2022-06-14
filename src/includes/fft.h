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
double complex* fft3(double complex *x,double complex* a, double complex *b,int* c, uint64_t N);
double complex* ifft3(double complex *x,double complex* a, double complex *b,int* c, uint64_t N);
void fft3_spec_calc_coefs(int* c, uint64_t N);
void ifft3_spec(double complex *x, double complex *res, double complex* help, int* c,  uint32_t stepen2);
void fft3_spec(double complex *x, double complex *res, double complex* help, int* c,  uint32_t stepen2);


#endif