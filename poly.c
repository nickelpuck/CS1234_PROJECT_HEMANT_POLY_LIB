#include "poly.h"

//Cr/Dest
Poly *poly_create(int degree){
    if(degree<0) return NULL;
    Poly *p=(Poly *)malloc(sizeof(Poly));
    if(!p) return NULL;
    p->degree=degree;
    p->coeffs=(double *)calloc(degree+1,sizeof(double));
    if(!p->coeffs){free(p);return NULL;}
    return p;
}

Poly *poly_from_array(double *coeffs,int degree){
    if(!coeffs || degree<0) return NULL;
    Poly *p=poly_create(degree);
    if(!p) return NULL;
    for(int i=0;i<=degree;i++)
        p->coeffs[i]=coeffs[i];
    return poly_normalize(p);
}
Poly *poly_copy(const Poly *p){
    if(!p) return NULL;
    return poly_from_array(p->coeffs,p->degree);
}
void poly_destroy(Poly *p){
    if(!p) return;
    free(p->coeffs);
    free(p);
}

//we need to normalize it
Poly *poly_normalize(Poly *p){
    if(!p) return NULL;
    while(p->degree>0 && fabs(p->coeffs[p->degree])<1e-12)
        p->degree--;
    return p;
}

//Display the 

static void _print_poly(const Poly *p,FILE *f){
    if(!p) {fprintf(f,"(null)\n"); return;}
    if(poly_is_zero(p)) {fprintf(f, "0\n"); return;}
    int printed=0;
    for(int i=p->degree;i>=0;i--){
        double c=p->coeffs[i];
        if(fabs(c)<1e-12) continue;

        if(printed){
            fprintf(f,c<0 ? " - " : " + ");
            c=fabs(c);
        }else if(c<0){
            fprintf(f, "-");
            c=fabs(c);
        }
        if (i==0)
            fprintf(f, "%.4g", c);
        else if (fabs(c - 1.0)<1e-12)
            i==1 ? fprintf(f, "x") : fprintf(f, "x^%d", i);
        else
            i==1 ? fprintf(f, "%.4gx", c) : fprintf(f, "%.4gx^%d", c, i);
        printed=1;
    }
    fprintf(f, "\n");
}

void poly_print(const Poly *p){
    _print_poly(p, stdout);
}
void poly_print_to_file(const Poly *p,FILE *f){
    _print_poly(p, f);
}

//Properties of the poly
int poly_degree(const Poly *p){
    return p ? p->degree : -1;
}

double poly_leading_coeff(const Poly *p){
    return p ? p->coeffs[p->degree] : 0.0;
}

int poly_is_zero(const Poly *p) {
    if(!p) return 1;
    for(int i=0;i<=p->degree;i++)
        if(fabs(p->coeffs[i])>=1e-12) return 0;
    return 1;
}

int poly_equals(const Poly *a,const Poly *b){
    if(!a || !b) return 0;
    if(a->degree!= b->degree) return 0;
    for(int i=0;i<= a->degree;i++)
        if(fabs(a->coeffs[i] - b->coeffs[i]) >= 1e-9) return 0;
    return 1;
}

//Arith operatns
Poly *poly_add(const Poly *a,const Poly *b){
    if(!a || !b) return NULL;
    int deg= a->degree > b->degree ? a->degree : b->degree;
    Poly *res=poly_create(deg);
    if(!res) return NULL;
    for(int i=0;i <= a->degree;i++) res->coeffs[i]+=a->coeffs[i];
    for(int i=0;i<=b->degree;i++) res->coeffs[i]+=b->coeffs[i];
    return poly_normalize(res);
}

Poly *poly_sub(const Poly *a,const Poly *b){
    if(!a || !b) return NULL;
    int deg=a->degree > b->degree ? a->degree : b->degree;
    Poly *res=poly_create(deg);
    if(!res) return NULL;
    for(int i=0;i<=a->degree;i++) res->coeffs[i]+=a->coeffs[i];
    for (int i=0;i<=b->degree; i++) res->coeffs[i]-=b->coeffs[i];
    return poly_normalize(res);
}

Poly *poly_mul(const Poly *a,const Poly *b){
    if(!a || !b) return NULL;
    int deg=a->degree+b->degree;
    Poly *res=poly_create(deg);
    if(!res) return NULL;
    for(int i=0;i<=a->degree;i++)
        for(int j=0;j<=b->degree;j++)
            res->coeffs[i + j] += a->coeffs[i] * b->coeffs[j];
    return poly_normalize(res);
}

Poly *poly_scalar_mul(const Poly *a,double scalar){
    if(!a) return NULL;
    Poly *res=poly_copy(a);
    if(!res) return NULL;
    for(int i=0;i<=res->degree; i++)
        res->coeffs[i]*=scalar;
    return poly_normalize(res);
}

Poly *poly_negate(const Poly *a){
    return poly_scalar_mul(a, -1.0);
}

//Evaluating the poly
double poly_eval(const Poly *p,double x){
    if(!p) return 0.0;
    double result=0.0;
    double xpow=1.0;
    for(int i=0;i<=p->degree;i++) {
        result+=p->coeffs[i]*xpow;
        xpow*=x;
    }
    return result;
}

double poly_eval_horner(const Poly *p,double x){
    if(!p) return 0.0;
    double result=p->coeffs[p->degree];
    for(int i=p->degree-1;i>=0;i--)
        result=result * x + p->coeffs[i];
    return result;
}

//Handle the calculus
Poly *poly_differentiate(const Poly *p){
    if(!p) return NULL;
    if(p->degree==0){
        Poly *zero=poly_create(0);
        zero->coeffs[0]=0.0;
        return zero;
    }
    Poly *res=poly_create(p->degree - 1);
    if(!res) return NULL;
    for (int i=1;i<=p->degree;i++)
        res->coeffs[i-1] = i*p->coeffs[i];
    return poly_normalize(res);
}

Poly *poly_integrate(const Poly *p,double constant) {
    if(!p) return NULL;
    Poly *res=poly_create(p->degree + 1);
    if(!res) return NULL;
    res->coeffs[0]=constant;
    for(int i=0;i<=p->degree; i++)
        res->coeffs[i + 1] = p->coeffs[i] / (i + 1);
    return poly_normalize(res);
}

// Adv opr
Poly *poly_compose(const Poly *f,const Poly *g){
    if(!f || !g) return NULL;
    Poly *result=poly_create(0);
    result->coeffs[0] = 0.0;

    Poly *gpow=poly_create(0);
    gpow->coeffs[0]=1.0;

    for(int i=0;i<=f->degree;i++){
        Poly *term=poly_scalar_mul(gpow, f->coeffs[i]);
        Poly *tmp=poly_add(result, term);
        poly_destroy(result);
        poly_destroy(term);
        result = tmp;

        if(i<f->degree){
            Poly *newgpow = poly_mul(gpow, g);
            poly_destroy(gpow);
            gpow = newgpow;
        }
    }
    poly_destroy(gpow);
    return poly_normalize(result);
}

Poly **poly_divmod(const Poly *a,const Poly *b){
    if(!a || !b || poly_is_zero(b)) return NULL;

    Poly **res =(Poly **)malloc(2 * sizeof(Poly *));
    if(!res) return NULL;

    Poly *rem =poly_copy(a);
    Poly *quo =poly_create(0);
    quo->coeffs[0] = 0.0;

    while(!poly_is_zero(rem) && rem->degree >= b->degree){
        int deg=rem->degree - b->degree;
        double coeff = rem->coeffs[rem->degree] / b->coeffs[b->degree];

        Poly *mono=poly_create(deg);
        mono->coeffs[deg]=coeff;

        Poly *tmp_quo=poly_add(quo, mono);
        poly_destroy(quo);
        quo=tmp_quo;

        Poly *sub_term=poly_mul(mono, b);
        Poly *new_rem=poly_sub(rem, sub_term);
        poly_destroy(mono);
        poly_destroy(sub_term);
        poly_destroy(rem);
        rem = poly_normalize(new_rem);
    }

    res[0]=poly_normalize(quo);
    res[1]=rem;
    return res;
}

Poly *poly_mod(const Poly *a,const Poly *b){
    Poly **dm=poly_divmod(a, b);
    if(!dm) return NULL;
    Poly *rem=dm[1];
    poly_destroy(dm[0]);
    free(dm);
    return rem;
}

Poly *poly_gcd(const Poly *a,const Poly *b){
    if (!a || !b) return NULL;
    Poly *x=poly_copy(a);
    Poly *y=poly_copy(b);
    while (!poly_is_zero(y)) {
        Poly *r=poly_mod(x, y);
        poly_destroy(x);
        x=y;
        y=r;
    }
    double lc=poly_leading_coeff(x);
    if (fabs(lc) > 1e-12) {
        Poly *norm = poly_scalar_mul(x, 1.0 / lc);
        poly_destroy(x);
        x = norm;
    }
    poly_destroy(y);
    return x;
}

double poly_newton_root(const Poly *p, double x0, int max_iter, double tol) {
    if (!p) return 0.0;
    Poly *dp = poly_differentiate(p);
    double x = x0;
    for (int i = 0; i < max_iter; i++) {
        double fx  = poly_eval_horner(p, x);
        double dfx = poly_eval_horner(dp, x);
        if (fabs(dfx) < 1e-15) break;
        double xnew = x - fx / dfx;
        if (fabs(xnew - x) < tol) { x = xnew; break; }
        x = xnew;
    }
    poly_destroy(dp);
    return x;
}

double *poly_roots_real(const Poly *p, int *count) {
    *count = 0;
    if (!p) return NULL;

    if (p->degree == 1) {
        double *roots = (double *)malloc(sizeof(double));
        roots[0] = -p->coeffs[0] / p->coeffs[1];
        *count = 1;
        return roots;
    }
    if (p->degree == 2) {
        double a = p->coeffs[2], b = p->coeffs[1], c = p->coeffs[0];
        double disc = b * b - 4 * a * c;
        if (disc < 0) return NULL;
        double *roots = (double *)malloc(2 * sizeof(double));
        if (disc == 0) {
            roots[0] = -b / (2 * a);
            *count = 1;
        } else {
            roots[0] = (-b + sqrt(disc)) / (2 * a);
            roots[1] = (-b - sqrt(disc)) / (2 * a);
            *count = 2;
        }
        return roots;
    }
    return NULL;
}

// --- File I/O ---

int poly_save(const Poly *p, const char *filename) {
    if (!p || !filename) return -1;
    FILE *f = fopen(filename, "w");
    if (!f) return -1;
    fprintf(f, "%d\n", p->degree);
    for (int i = 0; i <= p->degree; i++)
        fprintf(f, "%.15g\n", p->coeffs[i]);
    fprintf(f, "# Polynomial: ");
    poly_print_to_file(p, f);
    fclose(f);
    return 0;
}

Poly *poly_load(const char *filename) {
    if(!filename) return NULL;
    FILE *f=fopen(filename, "r");
    if(!f) return NULL;
    int degree;
    if(fscanf(f, "%d", &degree)!= 1 || degree<0){
        fclose(f); return NULL;
    }
    Poly *p=poly_create(degree);
    if(!p){
        fclose(f);
        return NULL;
    }
    for(int i=0;i<=degree;i++) {
        if(fscanf(f, "%lf", &p->coeffs[i]) != 1){
            
            poly_destroy(p);
            fclose(f);
            return NULL;
        }
    }
    fclose(f);
    return poly_normalize(p);
}
static void _get_divisors(int n,int *divs,int *ndivs) {
    *ndivs=0;
    for (int d=1;d<n;d++)
        if (n%d==0) divs[(*ndivs)++]=d;
}

Poly *poly_cyclotomic(int n){
    if(n<=0) return NULL;
    Poly *num=poly_create(n);
    num->coeffs[0] = -1.0;
    num->coeffs[n] =  1.0;
    int divs[256], ndivs;
    _get_divisors(n, divs, &ndivs);
    for(int i=0;i<ndivs;i++) {
        Poly *phi_d=poly_cyclotomic(divs[i]);
        Poly **dm=poly_divmod(num, phi_d);
        poly_destroy(num); poly_destroy(phi_d);
        num = dm[0]; poly_destroy(dm[1]); free(dm);
    }
    return poly_normalize(num);
}

Poly *poly_mod_p(const Poly *p,int prime){
    if(!p || prime <= 0) return NULL;
    Poly *result=poly_copy(p);
    for (int i=0; i<=result->degree;i++) {
        long c=(long)round(result->coeffs[i]);
        result->coeffs[i]=(double)(((c % prime)+prime)%prime);
    }
    return poly_normalize(result);
}

int poly_is_irreducible_zp(const Poly *p,int prime){
    if (!p || p->degree <= 0) return 0;
    if (p->degree == 1) return 1;
    for(int x=0;x<prime;x++){
        double val=0.0,xpow=1.0;
        for (int i=0; i<=p->degree;i++) {
            val+=round(p->coeffs[i])*xpow;
            xpow*=x;
        }
        if(((long)round(val)%prime+prime)%prime==0) return 0;
    }
    return 1;
}

long poly_hash(const Poly *p,long base,long mod){
    if(!p || mod<=0) return 0;
    long result=0;
    for(int i=p->degree;i>=0;i--) {
        long c=((long)round(p->coeffs[i])%mod+mod)%mod;
        result=(long)((__int128)result*base%mod);
        result=(result+c)%mod;
    }
    return result;
}
