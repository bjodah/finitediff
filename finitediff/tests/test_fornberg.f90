module test_fornberg

  use types, only: dp
  use fornberg, only: populate_weights

  implicit none
  private
  public test_weights

contains

  subroutine test_weights()
    real(dp) :: z
    integer :: j, k, m, nd
    real(dp), allocatable :: c(:,:)
    real(dp), parameter :: x(0:2) = [-1.0_dp, 0.0_dp, 1.0_dp]
    nd = size(x)-1
    m = 2
    z = 0.0_dp
    allocate(c(0:nd, 0:m))
    do k=0,m
      do j=0,nd
        c(j,k) = 0.0_dp
      end do
    end do
    call populate_weights(z, x, nd, m, c)
    print *, c
  end subroutine

end module test_fornberg

program main
use test_fornberg, only: test_weights
call test_weights()
end program
