#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include "cubic_spline.h"
#include "spline_output.h"

// а) тестовая функция phi(x) на [-1, 1]
double phi(double x) { return x <= 0 ? x * x * x + 3 * x * x : -x * x * x + 3 * x * x; }
double dphi(double x) { return x <= 0 ? 3 * x * x + 6 * x : -3 * x * x + 6 * x; }
double d2phi(double x) { return x <= 0 ? 6 * x + 6 : -6 * x + 6; }

// b) вариант 17: f(x) = e^(x-3) на [0, 5], f' = f'' = f
double f(double x) { return std::exp(x - 3); }

// c) осциллирующая: f(x) + cos(10x)
double g(double x) { return f(x) + std::cos(10 * x); }
double dg(double x) { return f(x) - 10 * std::sin(10 * x); }
double d2g(double x) { return f(x) - 100 * std::cos(10 * x); }

const TargetFunction functions[] = {
    { "phi(x)",             -1, 1, { phi, dphi, d2phi } },
    { "e^(x-3)",             0, 5, { f, f, f } },
    { "e^(x-3) + cos(10x)",  0, 5, { g, dg, d2g } },
};

const double EPS = 1e-6;
const int N_MAX = 1 << 20;  // таблица 5: n удваивается, пока n <= N_MAX

int main() {
    std::cout << std::setprecision(8);

    int fn, bc = 2, n;
    std::cout << "function: 0 - phi(x) (test a), 1 - e^(x-3) (b), 2 - e^(x-3) + cos(10x) (c): ";
    std::cin >> fn;
    if (fn != 0) {
        std::cout << "boundary: 1 - EGU S''(a) = S''(b) = 0, 2 - RGU S''(a) = F''(a), S''(b) = F''(b): ";
        std::cin >> bc;
    }
    std::cout << "n: ";
    std::cin >> n;
    if (!std::cin || fn < 0 || fn > 2 || bc < 1 || bc > 2 || n < 1) {
        std::cerr << "wrong input\n";
        return 1;
    }

    // для phi условия (2) совпадают с РГУ: S''(-1) = phi''(-1) = 0, S''(1) = phi''(1) = 0
    const TargetFunction& F = functions[fn];
    const double M1 = bc == 1 ? 0 : F.d[2](F.a);
    const double M2 = bc == 1 ? 0 : F.d[2](F.b);
    std::cout << "\nF(x) = " << F.name << ", [a, b] = [" << F.a << ", " << F.b << "]"
        << ", S''(a) = " << M1 << ", S''(b) = " << M2 << '\n';

    const Spline s = buildSpline(F.d[0], F.a, F.b, n, M1, M2);
    printCoefficients(s);
    printComparison(s, F, 0, 1);
    printComparison(s, F, 2, 2);
    printReport(computeErrors(s, F));

    const std::string csv = std::string("spline_") + "abc"[fn] + (bc == 1 ? "_EGU" : "_RGU") + "_n" + std::to_string(n) + ".csv";
    writeCsv(csv, s, F);

    // графики: tools/plot_spline.py в отдельном процессе, программа не ждет закрытия окна
#ifdef _WIN32
    const std::string plot = "start \"\" /b py \"" PLOT_SCRIPT "\" " + csv;
#else
    const std::string plot = "python3 \"" PLOT_SCRIPT "\" " + csv + " &";
#endif
    std::cout << "\nplots: " << csv << " -> " << PLOT_SCRIPT << '\n';
    std::system(plot.c_str());

    if (fn == 0) return 0;  // для теста хватает таблиц 1-3 и справки

    // таблица 4: n, при котором достигается точность EPS
    const int nEps = findN(F, M1, M2, EPS);
    std::cout << "\nmax|F-S| <= " << EPS << " is first reached at n = " << nEps << '\n';
    if (nEps > 0) printReport(computeErrors(buildSpline(F.d[0], F.a, F.b, nEps, M1, M2), F));

    printConvergence(F, M1, M2, n, N_MAX);
    return 0;
}
