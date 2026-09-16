#include <sweep.h>

std::vector<double> GeneralSweepStrategy::sweep(const TridiagonalSystem& s) const {
    //проверка входных данных
    if (s.n < 1) throw std::invalid_argument("sweep: нужно n >= 1");
    if (s.A.size() <= s.n || s.C.size() <= s.n ||
        s.B.size() <= s.n || s.phi.size() <= s.n) {
        throw std::invalid_argument("sweep: размеры A/C/B/phi должны быть >= n+1");
    }
    const std::size_t n = s.n;
    //---- прямой ход ----------------------------------------------------------
    std::vector<double> alpha(n + 1, 0.0);
    std::vector<double> beta(n + 1, 0.0);
    alpha[1] = s.kp1;
    beta[1]  = s.mu1;
    for (std::size_t i = 1; i + 1 <= n; ++i) {
        const double d = s.C[i] - s.A[i] * alpha[i];
        //проверка знаменателя
        if (std::abs(d) < eps) {
            std::cout << "sweep: нулевой знаменатель на шаге i = " << i
                      << " (C - A*alpha = " << d << ")";
            throw std::runtime_error("0");
        }
        alpha[i + 1] = s.B[i] / d;
        beta[i + 1]  = (s.phi[i] + s.A[i] * beta[i]) / d;
    }
// ---- обратный ход -------------------------------------------------------
    std::vector<double> y(n + 1, 0.0);
    const double dn = 1.0 - s.kp2 * alpha[n];
//проверка знаменателя
    if (std::abs(dn) < eps) {
        throw std::runtime_error("sweep: нулевой знаменатель в формуле для y[n]");
    }
    y[n] = (s.mu2 + s.kp2 * beta[n]) / dn;
    for (std::size_t i = n; i-- > 0;) {
        y[i] = alpha[i + 1] * y[i + 1] + beta[i + 1];
    }
    return y;
}