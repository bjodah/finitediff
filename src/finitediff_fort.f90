module fornberg

  implicit none

  integer, parameter :: dp=kind(0.d0)  ! double precision

  private
  public apply_fd, populate_weights

contains

  subroutine apply_fd(nin, maxorder, xdata, ydata, xtgt, out)
    integer, intent(in)    :: nin, maxorder
    real(dp), intent(in)    :: xdata(0:), ydata(0:), xtgt
    real(dp), intent(out) :: out(0:)

    integer :: j
    real(dp) :: c(0:nin-1, 0:maxorder)

    call populate_weights(xtgt, xdata, nin-1, maxorder, c)
    forall(j=0:maxorder) out(j) = sum(c(:, j)*ydata)
  end subroutine


  subroutine populate_weights(z, x, nd, m, c)
    !
    !  Input Parameters
    !    z            -  location where approximations are to be
    !                    accurate
    !    x(0:nd)      -  grid point locations, found in x(0:n)
    !    nd           -  dimension of x- and c-arrays in calling
    !                    program x(0:nd) and c(0:nd, 0:m), respectively
    !    m            -  highest derivative for which weights are
    !                    sought
    !
    !  Output Parameter
    !    c(0:nd,0:m)  -  weights at grid locations x(0:n) for
    !                    derivatives of order 0:m, found in c(0:nd, 0:m)
    !
    !  References:
    !      Generation of Finite Difference Formulas on Arbitrarily
    !          Spaced Grids, Bengt Fornberg,
    !          Mathematics of compuation, 51, 184, 1988, 699--706,
    !          doi: 10.1090/S0025-5718-1988-0935077-0
    !
    !      Classroom note: Calculation of weights in finite
    !          difference formulas, Bengt Fornberg,
    !          SIAM review, 40, 3, 1998, 685--691,
    !          doi: 10.1137/S0036144596322507

    real(dp), intent(in)    :: z
    integer,  intent(in)    :: nd, m
    real(dp), intent(in)    :: x(0:nd)
    real(dp), intent(out)   :: c(0:nd, 0:m)

    real(dp) :: c1, c2, c3, c4, c5
    integer  :: i, j, k, mn

    c1 = 1
    c4 = x(0) - z
    c = 0
    c(0, 0) = 1
    do i=1, nd
      mn = min(i, m)
      c2 = 1
      c5 = c4
      c4 = x(i) - z
      do j=0, i-1
        c3 = x(i) - x(j)
        c2 = c2*c3
        if (j == i-1) then
          do k = mn, 1, -1
            c(i, k) = c1*(k*c(i-1, k-1) - c5*c(i-1, k))/c2
          end do
          c(i, 0) = -c1*c5*c(i-1, 0)/c2
        endif
        do k=mn, 1, -1
          c(j, k) = (c4*c(j, k) - k*c(j, k-1))/c3
        end do
        c(j, 0) = c4*c(j, 0)/c3
      end do
      c1 = c2
    end do
  end subroutine

end module
