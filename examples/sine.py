#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division, print_function # Python 3 behaviour in Python 2

import numpy as np

import argh

from finitediff import derivatives_at_point_by_finite_diff


def demo_usage(n_data=50, n_fit=537, m=20):
    """
    Plots a noisy sine curve and the fitting to it.
    Also presents the error and the error in the
    approximation of its first derivative (cosine curve)
    """
    import matplotlib.pyplot as plt

    x0, xend = 0, 5
    x_data = np.linspace(x0,xend,n_data) + \
             np.random.rand(n_data)*(xend-x0)/n_data/1.5 # shaky linspace
    y_data = np.sin(x_data) * (1.0+0.1*(np.random.rand(n_data)-0.5)) # -5% to +5% noise

    if n_data < n_fit:
        m = 5 # points used behind and in front of interpolation

    x_fit = np.linspace(x0, xend, n_fit)

    # Edges behave badly, work around:
    x_fit[0] = x_fit[0] + (x_fit[1]-x_fit[0])/2
    x_fit[-1] = x_fit[-2]+(x_fit[-1]-x_fit[-2])/2

    y_fit = np.empty(n_fit)
    dydx_fit = np.empty(n_fit)
    for i, xf in enumerate(x_fit):
        # get index j of first data point beyond xf
        j = np.where(x_data > xf)[0][0]
        lower_bound = max(0,   j-m)
        upper_bound = min(n_data-1, j+m)
        y_fit[i] = derivatives_at_point_by_finite_diff(
            x_data[lower_bound:upper_bound],
            y_data[lower_bound:upper_bound], xf, 0)
        dydx_fit[i] = derivatives_at_point_by_finite_diff(
            x_data[lower_bound:upper_bound],
            y_data[lower_bound:upper_bound], xf, 1)[1]

    plt.subplot(221)
    plt.plot(x_data,y_data,'x',label='Data points (sin)')
    plt.plot(x_fit,y_fit,'-',label='Fitted curve (order=0)')
    plt.plot(x_data,np.sin(x_data),'-',label='Analytic sin(x)')
    plt.legend()

    plt.subplot(222)
    plt.plot(x_fit,y_fit-np.sin(x_fit), label='Error in order=0')
    plt.legend()

    plt.subplot(223)
    plt.plot(x_fit,dydx_fit,'-',label='Fitted derivative (order=1)')
    plt.plot(x_data,np.cos(x_data),'-',label='Analytic cos(x)')
    plt.legend()

    plt.subplot(224)
    plt.plot(x_fit,dydx_fit-np.cos(x_fit), label='Error in order=1')
    plt.legend()

    plt.show()


argh.dispatch_command(demo_usage)
