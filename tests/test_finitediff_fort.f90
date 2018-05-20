module test_fornberg

  use fornberg, only: populate_weights

  implicit none

  integer, parameter :: dp=kind(0.d0) ! double precision

  private
  public test_weights

contains

  subroutine test_weights()
    real(dp) :: z
    integer :: m, nd
    real(dp), allocatable :: c(:,:)
    real(dp), parameter :: x(0:2) = [-1.0_dp, 0.0_dp, 1.0_dp]
    real(dp), parameter :: c_ref(3,2) = reshape([0.0_dp, 1.0_dp, 0.0_dp, -0.5_dp, 0.0_dp, 0.5_dp, 1.0_dp, -2.0_dp, 1.0_dp], [3,2])
    nd = size(x)-1
    m = 2
    z = 0.0_dp
    allocate(c(0:nd, 0:m))
    call populate_weights(z, x, nd, m, c)
    if (sum(abs(c - c_ref)) > 1e-10) then
       stop "c not equal to c_ref"
    end if
  end subroutine

end module test_fornberg

program main
use test_fornberg, only: test_weights
call test_weights()
end program
