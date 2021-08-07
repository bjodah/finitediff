#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division, print_function  # Python 3 behaviour in Py2

import numpy as np

from finitediff import derivatives_at_point_by_finite_diff, interpolate_by_finite_diff


def demo_usage(n_data=50, n_fit=537, nhead=5, ntail=5, plot=False, alt=0):
    """
    Plots a noisy sine curve and the fitting to it.
    Also presents the error and the error in the
    approximation of its first derivative (cosine curve)

    Usage example for benchmarking:

    $ time python sine.py --nhead 3 --ntail 3 --n-fit 500000 --n-data 50000

    Usage example for plotting:

    $ python sine.py --nhead 1 --ntail 1 --plot

    """

    x0, xend = 0, 5
    # shaky linspace -5% to +5% noise
    x_data = (
        np.linspace(x0, xend, n_data)
        + np.random.rand(n_data) * (xend - x0) / n_data / 1.5
    )
    y_data = np.sin(x_data) * (1.0 + 0.1 * (np.random.rand(n_data) - 0.5))

    x_fit = np.linspace(x0, xend, n_fit)

    # Edges behave badly, work around:
    x_fit[0] = x_fit[0] + (x_fit[1] - x_fit[0]) / 2
    x_fit[-1] = x_fit[-2] + (x_fit[-1] - x_fit[-2]) / 2

    if alt:
        y_fit = np.empty(n_fit)
        dydx_fit = np.empty(n_fit)
        for i, xf in enumerate(x_fit):
            # get index j of first data point beyond xf
            j = np.where(x_data > xf)[0][0]
            lower_bound = max(0, j - alt)
            upper_bound = min(n_data - 1, j + alt)
            y_fit[i] = derivatives_at_point_by_finite_diff(
                x_data[lower_bound:upper_bound], y_data[lower_bound:upper_bound], xf, 0
            )
            dydx_fit[i] = derivatives_at_point_by_finite_diff(
                x_data[lower_bound:upper_bound], y_data[lower_bound:upper_bound], xf, 1
            )[1]
    else:
        interp = interpolate_by_finite_diff(x_data, y_data, x_fit, 1, nhead, ntail)
        y_fit = interp[:, 0]
        dydx_fit = interp[:, 1]

    if plot:
        import matplotlib.pyplot as plt

        plt.subplot(221)
        plt.plot(x_data, y_data, "x", label="Data points (sin)")
        plt.plot(x_fit, y_fit, "-", label="Fitted curve (order=0)")
        plt.plot(x_data, np.sin(x_data), "-", label="Analytic sin(x)")
        plt.legend()

        plt.subplot(222)
        plt.plot(x_fit, y_fit - np.sin(x_fit), label="Error in order=0")
        plt.legend()

        plt.subplot(223)
        plt.plot(x_fit, dydx_fit, "-", label="Fitted derivative (order=1)")
        plt.plot(x_data, np.cos(x_data), "-", label="Analytic cos(x)")
        plt.legend()

        plt.subplot(224)
        plt.plot(x_fit, dydx_fit - np.cos(x_fit), label="Error in order=1")
        plt.legend()

        plt.show()


if __name__ == "__main__":
    try:
        from argh import dispatch_command
    except ImportError:

        def dispatch_command(cb):
            return cb()

    dispatch_command(demo_usage)
