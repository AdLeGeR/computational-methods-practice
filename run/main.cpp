#include <iostream>
#include <sweep.h>

using namespace std;
int main(){

    int n;
    std::cout << "еnter n: ";
    std::cin >> n;

    if (n < 2) {
        std::cout << "n < 2" << std::endl;
        return 1;
    }

    TridiagonalSystem s(n);
    double h = 1.0 / static_cast<double>(n);

    for (int i = 1; i <= n - 1; i++) {
        s.A[i] = 12.0 / (h * h);
        s.C[i] = 24.0 / (h * h) + 5.0;
        s.B[i] = 12.0 / (h * h);

        double xi = static_cast<double>(i) * h;
        s.phi[i] = -2110.0 + 450.0 * xi * xi;
    }
    s.kp1 = 0.0; s.mu1 = 10;
    s.kp2 = 0.0; s.mu2 = 100;

    auto v = GenetalSweepStrategy().sweep(s);

    double max = 0.0;

    std::cout << "i\txi\tvti\tvi\tvti-vi" << std::endl;
    std::cout << "---------------------------------------------" << std::endl;

    for (int i = 0; i <= n; i++) {
        double xi = i * h;
        double vti = 10.0 + 90.0 * xi * xi;
        double maxr = std::abs(vti - v[i]);

        if (maxr > max) {
            max = maxr;
        }

        std::cout << i << "\t" << xi << "\t" << vti << "\t" << v[i] << "\t" << vti - v[i] << std::endl;
    }
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << "max|vti-vi|: " << max << std::endl;

    return 0;
}