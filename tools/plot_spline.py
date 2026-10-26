"""Графики для лабы 2 (пункт 6 требований): F и S, производные, погрешности.

Использование:  py tools/plot_spline.py spline_b_EGU_n10.csv
CSV пишет writeCsv из spline_output.cpp: x,F,S,dF,dS,d2F,d2S (контрольная сетка)
Рядом с CSV сохраняется картинка .png - для отчета.
Нужен matplotlib:  py -m pip install matplotlib
"""
import csv
import sys

import matplotlib.pyplot as plt


def main() -> None:
    if len(sys.argv) != 2:
        print(__doc__)
        sys.exit(1)

    with open(sys.argv[1], newline="") as f:
        rows = list(csv.DictReader(f))
    col = {k: [float(r[k]) for r in rows] for k in rows[0]}
    x = col["x"]

    fig, ax = plt.subplots(2, 3, figsize=(15, 8))
    fig.suptitle(sys.argv[1])

    def pair(a, exact, approx, title):
        a.plot(x, col[exact], label=exact)
        a.plot(x, col[approx], "--", label=approx)
        a.set_title(title)
        a.legend()
        a.grid(True)

    def error(a, exact, approx, title):
        a.plot(x, [e - s for e, s in zip(col[exact], col[approx])])
        a.set_title(title)
        a.grid(True)

    pair(ax[0][0], "F", "S", "F(x) и S(x)")
    pair(ax[0][1], "dF", "dS", "F'(x) и S'(x)")
    pair(ax[0][2], "d2F", "d2S", "F''(x) и S''(x)")
    error(ax[1][0], "F", "S", "F - S")
    error(ax[1][1], "dF", "dS", "F' - S'")
    error(ax[1][2], "d2F", "d2S", "F'' - S''")

    plt.tight_layout()
    fig.savefig(sys.argv[1].removesuffix(".csv") + ".png", dpi=120)
    plt.show()


if __name__ == "__main__":
    main()
