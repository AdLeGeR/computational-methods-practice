#include <iostream>
#include <cmath>

void progonka(int n, double* a, double* c, double* b, double* f, double* v) {
    double* alpha = new double[n + 1];
    double* beta  = new double[n + 1];

    alpha[1] = 0.0;
    beta[1]  = 10.0;

    for (int i = 1; i <= n - 1; i++) {
        double d = c[i] - a[i] * alpha[i];
        alpha[i + 1] = b[i] / d;
        beta[i + 1]  = (f[i] + a[i] * beta[i]) / d;
    }

    v[n] = 100.0;

    for (int i = n - 1; i >= 0; i--) {
        v[i] = alpha[i + 1] * v[i + 1] + beta[i + 1];
    }

    delete[] alpha;
    delete[] beta;
}

int main() {
    int n;
    std::cout << "еnter n: ";
    std::cin >> n;

    if (n < 2) {
        std::cout << "n < 2" << std::endl;
        return 1;
    }

    double h = 1.0 / n;

    double* a = new double[n + 1];
    double* c = new double[n + 1];
    double* b = new double[n + 1];
    double* f = new double[n + 1];
    double* v = new double[n + 1];

    for (int i = 1; i <= n - 1; i++) {
        a[i] = 12.0 / (h * h);
        c[i] = 24.0 / (h * h) + 5.0;
        b[i] = 12.0 / (h * h);

        double xi = i * h;
        f[i] = -2110.0 + 450.0 * xi * xi;
    }

    progonka(n, a, c, b, f, v);

    double max = 0.0;

    std::cout << "i\txi\tvti\tvi\tvti-vi" << std::endl;
    std::cout << "---------------------------------------------" << std::endl;

    for (int i = 0; i <= n; i++) {
        double xi = i * h;
        double vti = 10.0 + 90.0 * xi * xi;
        double maxr=std::abs(vti - v[i]);

        if (maxr > max) {
            max = maxr;
        }

        std::cout << i << "\t" << xi << "\t" << vti << "\t" << v[i]<< "\t" << vti-v[i] << std::endl;
    }
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << "max|vti-vi|: " << max << std::endl;

    delete[] a;
    delete[] c;
    delete[] b;
    delete[] f;
    delete[] v;

    return 0;
}