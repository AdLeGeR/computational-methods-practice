#include "spline_output.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace {

    const int W = 16;  // ширина столбца

    // true - строку i из first..last печатать. если строк больше ROWS,
    // печатаем только первые и последние ROWS/2, а вместо пропущенных - "..."
    bool showRow(int i, int first, int last) {
        if (last - first < ROWS || i < first + ROWS / 2 || i > last - ROWS / 2) return true;
        if (i == first + ROWS / 2) std::cout << "...\n";
        return false;
    }

} // namespace

SplineErrors computeErrors(const Spline& s, const TargetFunction& F) {
    SplineErrors e{ s.n, K * s.n, { 0, 0, 0 }, { s.x[0], s.x[0], s.x[0] } };
    const double H = s.h / K;
    for (int j = 0; j <= e.N; j++) {
        const double x = s.x[0] + j * H;
        for (int k = 0; k < 3; k++) {
            const double d = std::abs(F.d[k](x) - S(s, x, k));
            if (d > e.err[k]) { e.err[k] = d; e.x[k] = x; }
        }
    }
    return e;
}

void printCoefficients(const Spline& s) {
    std::cout << "\nTable 1. Spline coefficients, n = " << s.n << '\n'
        << std::setw(6) << "i" << std::setw(W) << "x[i-1]" << std::setw(W) << "x[i]"
        << std::setw(W) << "a[i]" << std::setw(W) << "b[i]" << std::setw(W) << "c[i]" << std::setw(W) << "d[i]" << '\n';
    for (int i = 1; i <= s.n; i++) {
        if (!showRow(i, 1, s.n)) continue;
        std::cout << std::setw(6) << i << std::setw(W) << s.x[i - 1] << std::setw(W) << s.x[i]
            << std::setw(W) << s.a[i] << std::setw(W) << s.b[i] << std::setw(W) << s.c[i] << std::setw(W) << s.d[i] << '\n';
    }
}

void printComparison(const Spline& s, const TargetFunction& F, int kFrom, int kTo) {
    static const char* names[3][3] = { { "F", "S", "F-S" }, { "F'", "S'", "F'-S'" }, { "F''", "S''", "F''-S''" } };
    const int N = K * s.n;
    const double H = s.h / K;

    std::cout << "\nTable " << (kFrom == 0 ? 2 : 3) << ". F(x) = " << F.name << ", n = " << s.n << ", N = " << N << '\n'
        << std::setw(6) << "j" << std::setw(W) << "x";
    for (int k = kFrom; k <= kTo; k++) {
        for (const char* name : names[k]) std::cout << std::setw(W) << name;
    }
    std::cout << '\n';

    for (int j = 0; j <= N; j++) {
        if (!showRow(j, 0, N)) continue;
        const double x = s.x[0] + j * H;
        std::cout << std::setw(6) << j << std::setw(W) << x;
        for (int k = kFrom; k <= kTo; k++) {
            const double f = F.d[k](x), sp = S(s, x, k);
            std::cout << std::setw(W) << f << std::setw(W) << sp << std::setw(W) << f - sp;
        }
        std::cout << '\n';
    }
}

void printReport(const SplineErrors& e) {
    static const char* names[3] = { "max|F-S|    ", "max|F'-S'|  ", "max|F''-S''|" };
    std::cout << "\nReport\n"
        << "spline grid:  n = " << e.n << '\n'
        << "control grid: N = " << e.N << '\n';
    for (int k = 0; k < 3; k++) {
        std::cout << names[k] << " = " << e.err[k] << "  at x = " << e.x[k] << '\n';
    }
}

void writeCsv(const std::string& path, const Spline& s, const TargetFunction& F) {
    std::ofstream out(path);
    if (!out) { std::cerr << "can't write " << path << '\n'; return; }

    // 17 знаков - чтобы погрешности ~1e-10 не потерялись на фоне F ~ 1
    out << std::setprecision(17) << "x,F,S,dF,dS,d2F,d2S\n";
    const double H = s.h / K;
    for (int j = 0; j <= K * s.n; j++) {
        const double x = s.x[0] + j * H;
        out << x;
        for (int k = 0; k < 3; k++) out << ',' << F.d[k](x) << ',' << S(s, x, k);
        out << '\n';
    }
}

int findN(const TargetFunction& F, double M1, double M2, double eps) {
    auto ok = [&](int n) {
        return computeErrors(buildSpline(F.d[0], F.a, F.b, n, M1, M2), F).err[0] <= eps;
    };

    // удваиваем n, пока погрешность больше eps, затем делим [n/2, n] пополам
    int hi = 1;
    while (!ok(hi)) {
        if (hi >= (1 << 22)) return 0;  // eps не достигается (мешает погрешность округления)
        hi *= 2;
    }
    int lo = hi / 2;  // при lo погрешность еще больше eps
    while (hi - lo > 1) {
        const int mid = (lo + hi) / 2;
        (ok(mid) ? hi : lo) = mid;
    }
    return hi;
}

void printConvergence(const TargetFunction& F, double M1, double M2, int n0, int nMax) {
    std::cout << "\nTable 5. F(x) = " << F.name << ", p = log2(e(n/2) / e(n))\n"
        << std::setw(8) << "n"
        << std::setw(W) << "max|F-S|" << std::setw(8) << "p"
        << std::setw(W) << "max|F'-S'|" << std::setw(8) << "p"
        << std::setw(W) << "max|F''-S''|" << std::setw(8) << "p" << '\n';

    double prev[3] = {};
    for (int n = n0; n <= nMax; n *= 2) {
        const SplineErrors e = computeErrors(buildSpline(F.d[0], F.a, F.b, n, M1, M2), F);
        std::cout << std::setw(8) << n;
        for (int k = 0; k < 3; k++) {
            std::cout << std::setw(W) << e.err[k] << std::setw(8);
            if (n == n0) std::cout << "-";
            else std::cout << std::round(100 * std::log2(prev[k] / e.err[k])) / 100;  // 2 знака после запятой
            prev[k] = e.err[k];
        }
        std::cout << '\n';
    }
}
