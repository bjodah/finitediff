module fornberg

  use iso_c_binding, only: c_double, c_int

  implicit none

  integer, parameter :: dp=kind(0.d0)  ! double precision

  private
  public apply_fd, populate_weights

contains

  subroutine apply_fd(nin, maxorder, xdata, ydata, xtgt, out) bind(c, name="apply_fd")
    integer(c_int), intent(in)    :: nin, maxorder
    real(c_double), intent(in)    :: xdata(0:nin-1), ydata(0:nin-1), xtgt
    real(c_double), intent(inout) :: out(0:maxorder)

    integer :: j,k
    real(dp), allocatable :: c(:,:)

    allocate(c(0:nin-1, 0:maxorder))
    do k=0,maxorder
      do j=0,nin-1
        c(j,k) = 0.0_dp
      end do
    end do
    call populate_weights(xtgt, xdata, nin-1, maxorder, c)
    do j=0,maxorder
      out(j) = sum(c(:, j)*ydata(:))
    end do
  end subroutine


  subroutine populate_weights (z, x, nd, m, c) bind(c, name="populate_weights")
    ! 
    !  Input Parameters
    !    z            -  location where approximations are to be
    !                    accurate,
    !    x(0:nd)      -  grid point locations, found in x(0:n)
    !    nd           -  dimension of x- and c-arrays in calling
    !                    program x(0:nd) and c(0:nd,0:m), respectively,
    !    m            -  highest derivative for which weights are
    !                    sought,
    !
    !  Output Parameter
    !    c(0:nd,0:m)  -  weights at grid locations x(0:n) for
    !                    derivatives of order 0:m, found in c(0:n,0:m)
    !
    !  Reference:
    !      Generation of Finite Difference Formulas on Arbitrarily
    !          Spaced Grids, Bengt Fornberg,
    !          Mathematics of compuation, 51, 184, 1988, 699-706

    real(c_double), intent(in)    :: z
    integer(c_int), intent(in)    :: nd, m
    real(c_double), intent(in)    :: x(0:nd)
    real(c_double), intent(inout) :: c(0:nd, 0:m)
    
    real(dp) :: c1, c2, c3, c4, c5
    integer :: i, j, k, mn, n

    n = nd
    c1 = 1.0_dp
    c4 = x(0)-z
    c(0,0) = 1.0_dp
    do i=1,n
      mn = min(i,m)
      c2 = 1.0_dp
      c5 = c4
      c4 = x(i)-z
      do j=0,i-1
        c3 = x(i)-x(j)
        c2 = c2*c3
        if (j == i-1) then
          do k=mn,1,-1
            c(i,k) = c1*(k*c(i-1, k-1)-c5*c(i-1, k))/c2
          end do
          c(i,0) = -c1*c5*c(i-1,0)/c2
        endif
        do k=mn,1,-1
          c(j,k) = (c4*c(j,k)-k*c(j,k-1))/c3
        end do
        c(j,0) = c4*c(j,0)/c3
      end do
      c1 = c2
    end do
    return
  end subroutine
end module
