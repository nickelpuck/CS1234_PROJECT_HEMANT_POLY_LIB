#include "crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

//Modular Arithmetic Helpers
long mod_pow(long base,long exp,long mod) {
    long result = 1;
    base %= mod;
    if(base<0) base+=mod;
    while(exp>0){
        if(exp&1) result=mod_mul(result,base,mod);
        base=mod_mul(base,base,mod);
        exp>>=1;
    }
    return result;
}

long mod_mul(long a,long b,long mod) {
    return (long)((__int128)a * b % mod);
}

long mod_inv(long a, long mod) {
    //Fermat's little thm: a^(p-1)=1 mod p=>a^(p-2)=a^-1 mod p
    return mod_pow(a,mod - 2,mod);
}

//Shamir's Secret Sharing
Share *shamir_split(long secret,int n,int k,long prime) {
    if(n<k || k<1) return NULL;
    srand((unsigned)time(NULL));

    //Build random polyn of deg k-1 with const term=secret
    //f(x)=secret+a1*x+a2*x^2+...+a(k-1)*x^(k-1) mod prime
    long *coeffs = (long *)malloc(k * sizeof(long));
    if(!coeffs) return NULL;
    coeffs[0]=secret%prime;
    for(int i=1;i < k;i++)
        coeffs[i]=(long)(rand()%(prime - 1))+1;

    Share *shares=(Share *)malloc(n * sizeof(Share));
    if(!shares) {free(coeffs);return NULL; }

    for(int i=0;i<n;i++) {
        long x=i+1;
        long y=0;
        long xpow=1;
        for (int j=0;j<k;j++) {
            y=(y+mod_mul(coeffs[j],xpow,prime))%prime;
            xpow =mod_mul(xpow,x,prime);
        }
        shares[i].x=(int)x;
        shares[i].y=y;
    }
    free(coeffs);
    return shares;
}

long shamir_reconstruct(Share *shares,int k,long prime) {
    if(!shares || k<1) return -1;
    // Lagrange interpolation at x=0 over Z_prime
    long secret=0;
    for(int i=0;i<k;i++) {
        long xi=shares[i].x;
        long yi=shares[i].y;
        long num=1,den=1;
        for(int j=0;j<k;j++) {
            if(i==j) continue;
            long xj=shares[j].x;
            num=mod_mul(num,(prime-xj)%prime,prime);
            den=mod_mul(den,(xi-xj+prime)%prime,prime);
        }
        long term=mod_mul(yi,mod_mul(num,mod_inv(den,prime),prime),prime);
        secret=(secret+term)%prime;
    }
    return secret;
}
int shamir_verify(Share *shares,int k,Share test,long prime) {
    // Reconstruct poly value at test.x using k shares
    long computed=0;
    for(int i=0;i<k;i++) {
        long xi = shares[i].x;
        long yi = shares[i].y;
        long num = 1, den = 1;
        for (int j = 0; j < k; j++) {
            if (i == j) continue;
            long xj = shares[j].x;
            // Lagrange basis at test.x
            num = mod_mul(num, ((long)test.x - xj + prime) % prime, prime);
            den = mod_mul(den, (xi - xj + prime) % prime, prime);
        }
        long term = mod_mul(yi, mod_mul(num, mod_inv(den, prime), prime), prime);
        computed = (computed + term) % prime;
    }
    return computed == test.y % prime;
}

//Lagrange Interpolation(real-valued)

Poly *lagrange_interpolate(double *xs, double *ys, int k) {
    if (!xs || !ys || k <= 0) return NULL;

    Poly *result = poly_create(0);
    result->coeffs[0] = 0.0;

    for (int i = 0; i < k; i++) {
        //Build basis polyn L_i(x)=product_{j!=i} (x - xj)/(xi - xj)
        Poly *basis = poly_create(0);
        basis->coeffs[0] = 1.0;

        double denom = 1.0;
        for (int j = 0; j < k; j++) {
            if (i == j) continue;
            //multiply basis by(x - xs[j])
            double c[] = {-xs[j], 1.0};
            Poly *factor = poly_from_array(c, 1);
            Poly *tmp = poly_mul(basis, factor);
            poly_destroy(basis);
            poly_destroy(factor);
            basis = tmp;
            denom *= (xs[i] - xs[j]);
        }
        //Scale by ys[i]/denom
        Poly *term = poly_scalar_mul(basis, ys[i] / denom);
        poly_destroy(basis);
        Poly *tmp = poly_add(result, term);
        poly_destroy(result);
        poly_destroy(term);
        result = tmp;
    }
    return poly_normalize(result);
}

long *lagrange_interpolate_zp(long *xs, long *ys, int k, long prime) {
    if (!xs || !ys || k <= 0) return NULL;
    long *coeffs = (long *)calloc(k, sizeof(long));
    if (!coeffs) return NULL;

    //Compute poly coeff via Lagrange over Z_p
    for (int i = 0; i < k; i++) {
        long denom = 1;
        for (int j = 0; j < k; j++) {
            if (i == j) continue;
            denom = mod_mul(denom, (xs[i] - xs[j] + prime) % prime, prime);
        }
        long li_const = mod_mul(ys[i], mod_inv(denom, prime), prime);

        //Build product polyn(x-xs[j]) for j!=i
        long *basis = (long *)calloc(k, sizeof(long));
        basis[0] = 1;
        int deg = 0;
        for (int j = 0; j < k; j++) {
            if (i == j) continue;
            //multiply by(x-xs[j])
            for (int d = deg; d >= 0; d--) {
                basis[d + 1] = (basis[d + 1] + basis[d]) % prime;
                basis[d] = mod_mul(basis[d], (prime - xs[j]) % prime, prime);
            }
            deg++;
        }
        for (int d = 0; d < k; d++)
            coeffs[d] = (coeffs[d] + mod_mul(li_const, basis[d], prime)) % prime;
        free(basis);
    }
    return coeffs;
}

//Polynomial Fingerprinting & Rabin-Karp

Fingerprint fp_create(long prime, long base) {
    Fingerprint fp;
    fp.prime = prime;
    fp.base  = base;
    return fp;
}

long fp_of_string(const char *s, int len, Fingerprint fp) {
    long h = 0;
    long pw = 1;
    for (int i = 0; i < len; i++) {
        h = (h + mod_mul((unsigned char)s[i], pw, fp.prime)) % fp.prime;
        pw = mod_mul(pw, fp.base, fp.prime);
    }
    return h;
}

// Rolling hash: slide window by removing old_char,adding new_char
long fp_roll(long prev_fp, char old_char, char new_char,
             int window, Fingerprint fp, long base_pow) {
    long h=prev_fp;
    // Remove contri of old_char(at pos 0)
    h=(h-(unsigned char)old_char%fp.prime+ fp.prime)%fp.prime;
    // Divide by base(multiply by base^-1)
    h=mod_mul(h,mod_inv(fp.base,fp.prime),fp.prime);
    // Add new_char at pos window-1
    h=(h+mod_mul((unsigned char)new_char,base_pow,fp.prime))%fp.prime;
    return h;
}

int *rabin_karp_search(const char *text,const char *pattern,int *count) {
    *count=0;
    if (!text || !pattern) return NULL;

    int n=(int)strlen(text);
    int m=(int)strlen(pattern);
    if (m>n || m==0) return NULL;


    long prime = 1000000007L;
    long base  = 31L;

    Fingerprint fp=fp_create(prime,base);

    // Precompute base^(m-1) mod prime
    long base_pow=mod_pow(base,m-1,prime);

    long pat_fp=fp_of_string(pattern,m,fp);
    long text_fp=fp_of_string(text,m,fp);

    int *results=(int *)malloc(n * sizeof(int));
    if(!results) return NULL;

    for (int i=0; i<=n-m;i++) {
        if (text_fp==pat_fp) {
            // Verify (avoid hash collision)
            if(strncmp(text+i,pattern,m)==0)
                results[(*count)++]=i;
        }
        if (i< n-m) {
            text_fp=fp_roll(text_fp,text[i],text[i + m],m,fp,base_pow);
        }
    }
    return results;
}

long fp_of_file(const char *filename,Fingerprint fp) {
    FILE *f=fopen(filename, "rb");
    if(!f) return -1;
    long h=0, pw=1;
    int c;
    while ((c=fgetc(f)) != EOF) {
        h=(h+mod_mul((unsigned char)c,pw,fp.prime))%fp.prime;
        pw=mod_mul(pw,fp.base,fp.prime);
    }
    fclose(f);
    return h;
}
