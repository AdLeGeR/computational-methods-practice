#pragma once
#include <vector>

// Кубический сплайн на равномерной сетке x_0 = a, ..., x_n = b, шаг h.
// На участке [x_{i-1}, x_i], i = 1..n:
//   S_i(x) = a_i + b_i (x - x_i) + c_i/2 (x - x_i)^2 + d_i/6 (x - x_i)^3
struct Spline {
    int n = 0;
    double h = 0;
    std::vector<double> x, a, b, c, d;  // размер n+1; у a, b, d индекс 0 не используется, c[0] = S''(x_0)
};

// f - функция, [left, right] - отрезок, n - число участков,
// M1 = S''(left), M2 = S''(right) - граничные условия
Spline buildSpline(double (*f)(double), double left, double right, int n, double M1, double M2);

// k-я производная сплайна в точке x (k = 0, 1, 2)
double S(const Spline& s, double x, int k = 0);
