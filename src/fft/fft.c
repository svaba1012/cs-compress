#include "fft.h"
#include "math.h"

// fft skinut sa github-a od druga
// --------------------------------------------------
void dft(double complex *l, double complex *yf, uint64_t N) {
	for(uint64_t k = 0; k<N; k++) {
		double complex t = 0;
		for(uint64_t n = 0; n<N; n++)
			t+=l[n]*cexp(-I*2*PI*k*n/N);
		yf[k] = t;
	}
}

void idft(double complex *l, double complex *yf, uint64_t N) {
	for(uint64_t k = 0; k<N; k++) {
		double complex t = 0;
		for(uint64_t n = 0; n<N; n++)
			t+=l[n]*cexp(I*2*PI*k*n/N);
		yf[k] = t/N;
	}
}

void fft(double complex *l, double complex *yf, uint64_t N) {
	//printf("%d\n", N);
	if(N%2>0) {
		printf("N must be even.\n"), yf = NULL;
		return;
	} else if(N<=NREK) {
		dft(l, yf, N);
		return;
	}

	double complex *lparno = malloc(sizeof(*lparno)*(N/2));
	double complex *yfparno = malloc(sizeof(*yfparno)*(N/2));
	double complex *lneparno = malloc(sizeof(*lneparno)*(N/2));
	double complex *yfneparno = malloc(sizeof(*yfneparno)*(N/2));

	for(uint64_t i = 0; i<N/2; i++)
		lparno[i] = l[2*i];
	fft(lparno, yfparno, N/2);

	for(uint64_t i = 0; i<N/2; i++)
		lneparno[i] = l[2*i+1];
	fft(lneparno, yfneparno, N/2);

	for(uint64_t i = 0; i<N; i++) {
		double complex t = cexp(-2*I*PI*i/N);
		yf[i] = yfparno[i%(N/2)] + t * yfneparno[i%(N/2)];
	}

	free(lparno);
	free(yfparno);
	free(lneparno);
	free(yfneparno);

}

void ifft(double complex *l, double complex *yf, uint64_t N) {
	//printf("%d\n", N);
	if(N%2>0) {
		printf("N must be even.\n"), yf = NULL;
		return;
	} else if(N<=NREK) {
		idft(l, yf, N);
		return;
	}

	double complex *lparno = malloc(sizeof(*lparno)*(N/2));
	double complex *yfparno = malloc(sizeof(*yfparno)*(N/2));
	double complex *lneparno = malloc(sizeof(*lneparno)*(N/2));
	double complex *yfneparno = malloc(sizeof(*yfneparno)*(N/2));

	for(uint64_t i = 0; i<N/2; i++)
		lparno[i] = l[2*i];
	ifft(lparno, yfparno, N/2);

	for(uint64_t i = 0; i<N/2; i++)
		lneparno[i] = l[2*i+1];
	ifft(lneparno, yfneparno, N/2);

	for(uint64_t i = 0; i<N; i++) {
		double complex t = cexp(2*I*PI*i/N);
		yf[i] = (yfparno[i%(N/2)] + t * yfneparno[i%(N/2)])/2;
	}

	free(lparno);
	free(yfparno);
	free(lneparno);
	free(yfneparno);

}


void dft_rec(double complex *l, double complex *yf, uint64_t N, uint64_t poc, uint64_t korak) {
	for(uint64_t k = 0; k<N; k++) {
		double complex t = 0;
		for(uint64_t n = 0; n<N; n++)
			t+=l[n*korak+poc]*cexp(-I*2*PI*k*n/N);
		yf[k] = t;
	}
}

void idft_rec(double complex *l, double complex *yf, uint64_t N, uint64_t poc, uint64_t korak) {
	for(uint64_t k = 0; k<N; k++) {
		double complex t = 0;
		for(uint64_t n = 0; n<N; n++)
			t+=l[n*korak+poc]*cexp(I*2*PI*k*n/N);
		yf[k] = t/N;
	}
}

void fft_rec(double complex *l, double complex *yf, uint64_t N, uint64_t poc, uint64_t korak) {
	//printf("%d\n", N);
	if(N%2>0) {
		printf("N must be even.\n"), yf = NULL;
		return;
	} else if(N<=NREK) {
		dft_rec(l, yf, N, poc, korak);
		return;
	}

	double complex *yfparno = malloc(sizeof(*yfparno)*(N/2));
	double complex *yfneparno = malloc(sizeof(*yfneparno)*(N/2));

	fft_rec(l, yfparno, N/2, poc, korak*2);

	fft_rec(l, yfneparno, N/2, poc+korak, korak*2);

	for(uint64_t i = 0; i<N; i++) {
		double complex t = cexp(-2*I*PI*i/N);
		yf[i] = yfparno[i%(N/2)] + t * yfneparno[i%(N/2)];
	}

	free(yfparno);
	free(yfneparno);

}

void ifft_rec(double complex *l, double complex *yf, uint64_t N, uint64_t poc, uint64_t korak) {
	//printf("%d\n", N);
	if(N%2>0) {
		printf("N must be even.\n"), yf = NULL;
		return;
	} else if(N<=NREK) {
		idft_rec(l, yf, N, poc, korak);
		return;
	}

	double complex *yfparno = malloc(sizeof(*yfparno)*(N/2));
	double complex *yfneparno = malloc(sizeof(*yfneparno)*(N/2));

	ifft_rec(l, yfparno, N/2, poc, korak*2);

	ifft_rec(l, yfneparno, N/2, poc+korak, korak*2);

	for(uint64_t i = 0; i<N; i++) {
		double complex t = cexp(2*I*PI*i/N);
		yf[i] = (yfparno[i%(N/2)] + t * yfneparno[i%(N/2)])/2;
	}

	free(yfparno);
	free(yfneparno);

}

void fft2(double complex *l, double complex *yf, uint64_t N) {
	fft_rec(l, yf, N, 0, 1);
}
void ifft2(double complex *l, double complex *yf, uint64_t N) {
	ifft_rec(l, yf, N, 0, 1);
}
// ----------------------------------------------------------------------------

// moj iterativni fft3
double complex* fft3(double complex *x, double complex *y, uint64_t N) {
	
	// provera da li je broj stepen dvojke
	if(N & (N - 1)){
		printf("Broj nije stepen dvojke\n");
		return NULL;
	}
	double complex* a = malloc(N * sizeof(double complex));
	double complex* b = y;
	double complex* temp;

	// obrce bitove
	//fast bit reversal algorithm
	//1989, Anne Cathrine Elster School of Electrical Engineering Come11 University Ilhaca, New York 14853 
	// download link
	// https://folk.idi.ntnu.no/elster/pubs/elster-bit-rev-1989.pdf
	int* c = malloc(sizeof(int) * N);
	c[0] = 0;
	c[1] = 1;
	a[1] = x[N/2];
	a[0] = x[0];
	for(int L = 2; L < N; L = L << 1){
		int r = N/(2 * L);
		int L0 = L >> 1;
		for(int j = 0; j <= L0; j++){
			c[L + 2*j] = c[L0 + j];
			c[L + 2*j + 1] = c[L0 + j] + L;
			a[L + 2*j] = x[c[L + 2*j]*r];
			a[L + 2*j + 1] = x[(c[L + 2*j + 1])*r];
		}
	}

	for(int i = 1; i < N; i *= 2){
		int k = 2 * i;
		int N2 = N / k;	
		for(int j = 0; j < N2; j++){
			int base1 = j*k;
			int base2 = j*k +i;
			for(int s = 0; s < i; s++){
				b[base1 + s] = a[base1 + s] + cexp(-2*I*PI*s/k) * a[base2 + s];
				b[base2 + s] = a[base1 + s] + cexp(-2*I*PI*(s+i)/k) * a[base2 + s];
			}
		}
		temp = b;
		b = a;
		a = temp;
	}
	return a;
}

double complex* ifft3(double complex *x, double complex *y, uint64_t N) {
	if(N & (N - 1)){
		printf("Broj nije stepen dvojke\n");
		return NULL;
	}
	double complex* a = malloc(N * sizeof(double complex));
	double complex* b = y;
	double complex* temp;
	
	//version 3 fast bit reversal algorithm
	int* c = malloc(sizeof(int) * N);
	c[0] = 0;
	c[1] = 1;
	a[1] = x[N/2];
	a[0] = x[0];
	for(int L = 2; L < N; L = L << 1){
		int r = N/(2 * L);
		int L0 = L >> 1;
		for(int j = 0; j <= L0; j++){
			c[L + 2*j] = c[L0 + j];
			c[L + 2*j + 1] = c[L0 + j] + L;
			a[L + 2*j] = x[c[L + 2*j]*r];
			a[L + 2*j + 1] = x[(c[L + 2*j + 1])*r];
		}
	}
	// free(c);
	int count = 0;
	for(int i = 1; i < N; i *= 2){
		int k = 2 * i;
		int N2 = N / k;	
		for(int j = 0; j < N2; j++){
			int base1 = j*k;
			int base2 = j*k +i;
			for(int s = 0; s < i; s++){
				b[base1 + s] = a[base1 + s] + cexp(2*I*PI*s/k) * a[base2 + s];
				b[base2 + s] = a[base1 + s] + cexp(2*I*PI*(s+i)/k) * a[base2 + s];
			}
		}
		temp = b;
		b = a;
		a = temp;
	}
	for(int i = 0; i < N; i++){
		a[i] /= N;
	}
	return a;
}
