#ifndef CODES_H
#define CODES_H
#include "poly.h"

//Reed-Solomon Err Correcting Codes
//Encode a mesg of k symb as a polyn of deg=k-1
//Evaluate at n pts => n codeword symb
//Can recover org msg even if up to (n-k)/2 symb are corrupted
//
//We work over int with floating-pt arithmetic(simplified RS)
typedef struct {
    int n;  // total codeword len(no. of evaluatn pts)
    int k;      // message length (number of data symbols)
    double *points;    // evaluatn pts x0,x1,...,x{n-1}
} RSCode;

//Create RS code with n codeword symb, k mesg symb
//Evaluatm pts are 1, 2, ..., n
RSCode *rs_create(int n,int k);
void rs_destroy(RSCode *rs);

//Encode: msg[0..k-1]->codeword[0..n-1]
//Treats msg as coeff of polyn,evaluates at n pts
double *rs_encode(const RSCode *rs,double *message);

//Decode:recover msg from codeword(with possible erasures marked as NaN)
//Uses Lagrange interpolation on non-erased positn
//Returns NULL if too many erasures(>n-k)
double *rs_decode(const RSCode *rs,double *codeword);

//Simulate corruption:randomly corrupt 'num_errors' positns
void rs_corrupt(double *codeword,int n,int num_errors);

//Check if codeword is valid(all evaluatns consistent with a deg k-1 poly)
int rs_is_valid(const RSCode *rs,double *codeword);

//Print codeword
void rs_print_codeword(double *codeword,int n);

//Polyn-based Checksum/CRC-like
//Treat data as polyn,divide by generator poly
//Rem is the checksum

//Generator polyn for checksum (e.g.,x^4+x+1)
Poly *checksum_generator(int degree);
Poly *checksum_compute(const char *data,int len,Poly *generator);
int checksum_verify(const char *data,int len,Poly *generator,Poly *checksum);

//Berlekamp-Welch style error locator (simplified) ────────────────────────
// Given n points where up to e are wrong, find the correct polynomial
// Works when n >= k + 2e
Poly *rs_error_correct(double *xs,double *ys,int n,int k,int max_errors);
#endif
