#include "sweep.h"
#include <cmath>
#include <stdexcept>

namespace {

    // условия на краевые коэффициенты (достаточное условие корректности и устойчивости прогонки)
    void checkBoundary(const TridiagonalSystem& s) {
        if (s.n < 1) throw std::invalid_argument("sweep: нужно n >= 1");
        if (std::abs(s.kp1) > 1.0 || std::abs(s.kp2) > 1.0 ||
            std::abs(s.kp1) + std::abs(s.kp2) >= 2.0) {
            throw std::invalid_argument("sweep: нужно |kp1| <= 1, |kp2| <= 1 и |kp1| + |kp2| < 2");
        }
    }

    // диагональное преобладание в строке i: |C| >= |A| + |B|, A и B не равны 0.
    // вместе с checkBoundary гарантирует, что |alpha| <= 1 и знаменатели не обращаются в 0
    void checkRow(double a, double b, double c, std::size_t i) {
        if (a == 0.0 || b == 0.0 || std::abs(c) < std::abs(a) + std::abs(b)) {
            throw std::invalid_argument("sweep: нет диагонального преобладания в строке i = " + std::to_string(i));
        }
    }

    // resize не выделяет память заново, если размер уже подходящий.
    // обнулять не нужно: все используемые элементы перезаписываются
    void prepare(std::size_t n, SweepWorkspace& ws, std::vector<double>& y) {
        ws.alpha.resize(n + 1);
        ws.beta.resize(n + 1);
        y.resize(n + 1);
    }

    // обратный ход - одинаковый для обеих версий
    void backward(const TridiagonalSystem& s, const SweepWorkspace& ws, std::vector<double>& y) {
        const std::size_t n = s.n;
        const double dn = 1.0 - s.kp2 * ws.alpha[n];
        // при выполненных условиях dn != 0, это страховка
        if (dn == 0.0) throw std::runtime_error("sweep: нулевой знаменатель в формуле для y[n]");

        y[n] = (s.mu2 + s.kp2 * ws.beta[n]) / dn;
        for (std::size_t i = n; i-- > 0;) {
            y[i] = ws.alpha[i + 1] * y[i + 1] + ws.beta[i + 1];
        }
    }

} // namespace

void GeneralSweepStrategy::solve(const TridiagonalSystem& s, SweepWorkspace& ws, std::vector<double>& y) const {
    //проверка входных данных
    checkBoundary(s);
    if (s.constantCoeffs) {
        throw std::invalid_argument("general: нужна система с полными массивами A/B/C");
    }
    const std::size_t n = s.n;
    if (s.A.size() <= n || s.C.size() <= n || s.B.size() <= n || s.phi.size() <= n) {
        throw std::invalid_argument("sweep: размеры A/C/B/phi должны быть >= n+1");
    }

    prepare(n, ws, y);
    std::vector<double>& alpha = ws.alpha;
    std::vector<double>& beta = ws.beta;

    //---- прямой ход ----------------------------------------------------------
    alpha[1] = s.kp1;
    beta[1] = s.mu1;

    for (std::size_t i = 1; i < n; ++i) {
        // коэффициенты в каждой строке свои, поэтому и проверка в каждой строке
        checkRow(s.A[i], s.B[i], s.C[i], i);
        const double d = s.C[i] - s.A[i] * alpha[i];
        alpha[i + 1] = s.B[i] / d;
        beta[i + 1] = (s.phi[i] + s.A[i] * beta[i]) / d;
    }

    // ---- обратный ход -------------------------------------------------------
    backward(s, ws, y);
}

void OptimizedSweepStrategy::solve(const TridiagonalSystem& s, SweepWorkspace& ws, std::vector<double>& y) const {
    //проверка входных данных - та же, что в general, но строка всего одна, поэтому O(1)
    checkBoundary(s);
    if (!s.constantCoeffs) {
        throw std::invalid_argument("optimized: нужна система с постоянными коэффициентами "
            "(TridiagonalSystem::withConstantCoeffs)");
    }
    const std::size_t n = s.n;
    if (s.A.size() < 2 || s.B.size() < 2 || s.C.size() < 2 || s.phi.size() <= n) {
        throw std::invalid_argument("optimized: размеры A/B/C должны быть >= 2, phi >= n+1");
    }

    //оптимизация как конст/скаляр
    const double A = s.A[1];
    const double B = s.B[1];
    const double C = s.C[1];
    if (n >= 2) checkRow(A, B, C, 1);

    prepare(n, ws, y);
    std::vector<double>& alpha = ws.alpha;
    std::vector<double>& beta = ws.beta;

    // ---- прямой ход -----------------------------------------
    alpha[1] = s.kp1;
    beta[1] = s.mu1;

    for (std::size_t i = 1; i < n; ++i) {
        const double d = C - A * alpha[i];
        alpha[i + 1] = B / d;
        beta[i + 1] = (s.phi[i] + A * beta[i]) / d;
    }

    // ---- обратный ход -------------------------------------------
    backward(s, ws, y);
}
