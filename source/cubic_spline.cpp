#include "cubic_spline.h"
#include <cmath>
#include "sweep.h"

Spline buildSpline(double (*f)(double), double left, double right, int n, double M1, double M2) {
    Spline s;
    s.n = n;
    s.h = (right - left) / n;
    const double h = s.h;

    // узлы и значения функции в них
    s.x.resize(n + 1);
    std::vector<double> y(n + 1);
    for (int i = 0; i <= n; i++) {
        s.x[i] = left + i * h;
        y[i] = f(s.x[i]);
    }

    // 1) c_i находим прогонкой. Уравнения:
    //      c[i-1] + 4 c[i] + c[i+1] = 6 (y[i+1] - 2 y[i] + y[i-1]) / h^2,   i = 1..n-1
    //      c[0] = M1,  c[n] = M2
    //    В прогонке вид A c[i-1] - C c[i] + B c[i+1] = -phi[i], поэтому умножаем на -1:
    //      A = -1, B = -1, C = 4, phi = 6 (y[i+1] - 2 y[i] + y[i-1]) / h^2
    TridiagonalSystem sys(n);
    for (int i = 1; i < n; i++) {
        sys.A[i] = -1.0;
        sys.B[i] = -1.0;
        sys.C[i] = 4.0;
        sys.phi[i] = 6.0 * (y[i + 1] - 2.0 * y[i] + y[i - 1]) / (h * h);
    }
    sys.kp1 = 0.0;  sys.mu1 = M1;  // c[0] = M1
    sys.kp2 = 0.0;  sys.mu2 = M2;  // c[n] = M2

    GeneralSweepStrategy sweep;
    s.c = sweep.sweep(sys);

    // 2) остальные коэффициенты
    s.a.resize(n + 1);
    s.b.resize(n + 1);
    s.d.resize(n + 1);
    for (int i = 1; i <= n; i++) {
        s.a[i] = y[i];
        s.d[i] = (s.c[i] - s.c[i - 1]) / h;
        s.b[i] = (y[i] - y[i - 1]) / h + h * (2.0 * s.c[i] + s.c[i - 1]) / 6.0;
    }
    return s;
}

double S(const Spline& s, double x, int k) {
    // номер участка i (от 1 до n), в который попадает x
    int i = (int)std::ceil((x - s.x[0]) / s.h);
    if (i < 1) i = 1;
    if (i > s.n) i = s.n;

    const double t = x - s.x[i];
    switch (k) {
    case 0:  return s.a[i] + s.b[i] * t + s.c[i] / 2 * t * t + s.d[i] / 6 * t * t * t;
    case 1:  return s.b[i] + s.c[i] * t + s.d[i] / 2 * t * t;
    default: return s.c[i] + s.d[i] * t;
    }
}
