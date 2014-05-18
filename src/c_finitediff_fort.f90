module c_fornberg

  use iso_c_binding, only: c_double, c_int
  use fornberg, only: apply_fd, populate_weights

  implicit none

contains

  subroutine fornberg_apply_fd(nin, maxorder, xdata, ydata, xtgt, out) bind(c, name="fornberg_apply_fd")
    integer(c_int), value, intent(in) :: nin, maxorder
    real(c_double), intent(in) :: xdata(0:nin-1), ydata(0:nin-1)
    real(c_double), value, intent(in) :: xtgt
    real(c_double), intent(out) :: out(0:maxorder)
    call apply_fd(nin, maxorder, xdata, ydata, xtgt, out)
  end subroutine fornberg_apply_fd

  subroutine fornberg_populate_weights(z, x, nd, m, c) bind(c, name="fornberg_populate_weights")
    integer(c_int), value, intent(in) :: nd, m
    real(c_double), intent(in) :: x(0:nd)
    real(c_double), value, intent(in) :: z
    real(c_double), intent(out) :: c(0:nd, 0:m)
    call populate_weights(z, x, nd, m, c)
  end subroutine fornberg_populate_weights

end module
