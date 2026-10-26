#pragma once
#include <string>
#include "cubic_spline.h"

using Fn = double (*)(double);

// исследуемая функция F на [a, b]: d[0] = F, d[1] = F', d[2] = F''
struct TargetFunction {
    std::string name;
    double a, b;
    Fn d[3];
};

const int K = 10;      // контрольная сетка: N = K * n участков
const int ROWS = 60;   // если строк в таблице больше - печатаем только начало и конец

// нормы погрешностей (7), (8) на контрольной сетке:
// err[k] = max|F^(k) - S^(k)|, x[k] - точка, где максимум достигается
struct SplineErrors {
    int n, N;
    double err[3], x[3];
};

SplineErrors computeErrors(const Spline& s, const TargetFunction& F);

// таблица 1: коэффициенты сплайна
void printCoefficients(const Spline& s);

// таблица 2 (kFrom = 0, kTo = 1) и таблица 3 (kFrom = kTo = 2): F^(k), S^(k), F^(k) - S^(k)
void printComparison(const Spline& s, const TargetFunction& F, int kFrom, int kTo);

// справка
void printReport(const SplineErrors& e);

// x, F, S, F', S', F'', S'' на контрольной сетке - для tools/plot_spline.py
void writeCsv(const std::string& path, const Spline& s, const TargetFunction& F);

// минимальное n, при котором max|F - S| <= eps на контрольной сетке (0 - не найдено)
int findN(const TargetFunction& F, double M1, double M2, double eps);

// таблица 5: n = n0, 2 n0, 4 n0, ... <= nMax, порядок p = log2(e(n/2) / e(n))
void printConvergence(const TargetFunction& F, double M1, double M2, int n0, int nMax);
