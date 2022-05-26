//
// Created by qiaojinxia on 2022/5/25.
//
int ret3(void) {
    return 3;
    return 5;
}

int add2(int x, int y) {
    return x + y;
}

int sub2(int x, int y) {
    return x - y;
}

int add6(int a, int b, int c, int d, int e, int f) {
    return a + b + c + d + e + f;
}

int addx(int *x, int y) {
    return *x + y;
}

int sub_char(char a, char b, char c) {
    return a - b - c;
}

int fib(int x) {
    if (x<=1)
        return 1;
    return fib(x-1) + fib(x-2);
}

int sub_long(long a, long b, long c) {
    return a - b - c;
}

int sub_short(short a, short b, short c) {
    return a - b - c;
}

int g1;

int *g1_ptr(void) { return &g1; }
char int_to_char(int x) { return x; }

int div_long(long a, long b) {
    return a / b;
}

_Bool bool_fn_add(_Bool x) { return x + 1; }
_Bool bool_fn_sub(_Bool x) { return x - 1; }

static int static_fn(void) { return 3; }

int param_decay(int x[]) { return x[0]; }

int counter() {
    static int i;
    static int j = 1+1;
    return i++ + j++;
}

void ret_none() {
    return;
}

_Bool true_fn();
_Bool false_fn();
char char_fn();
short short_fn();

unsigned char uchar_fn();
unsigned short ushort_fn();

char schar_fn();
short sshort_fn();

int add_all(int n, ...);

typedef struct {
    int gp_offset;
    int fp_offset;
    void *overflow_arg_area;
    void *reg_save_area;
} __va_elem;

typedef __va_elem va_list[1];

int add_all(int n, ...);
int sprintf(char *buf, char *fmt, ...);
int vsprintf(char *buf, char *fmt, va_list ap);

char *fmt(char *buf, char *fmt, ...) {
    va_list ap;
    *ap = *(__va_elem *)__va_area__;
    vsprintf(buf, fmt, ap);
}

double add_double(double x, double y);
float add_float(float x, float y);

float add_float3(float x, float y, float z) {
    return x + y + z;
}

double add_double3(double x, double y, double z) {
    return x + y + z;
}

int (*fnptr(int (*fn)(int n, ...)))(int, ...) {
    return fn;
}

int param_decay2(int x()) { return x(); }

int param_decay2(int x()) { return x(); }
int testfunc() {

}