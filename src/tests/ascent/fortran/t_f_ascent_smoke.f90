!*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*!
!* Copyright (c) Lawrence Livermore National Security, LLC and other Ascent
!* Project developers. See top-level LICENSE AND COPYRIGHT files for dates and
!* other details. No copyright assignment is required to contribute to Ascent.
!*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*!


!------------------------------------------------------------------------------
!
! t_f_ascent_smoke.f
!
!------------------------------------------------------------------------------

!------------------------------------------------------------------------------
module t_f_ascent_smoke
!------------------------------------------------------------------------------

  use iso_c_binding
  use fruit
  use conduit
  use ascent
  implicit none

!------------------------------------------------------------------------------
contains
!------------------------------------------------------------------------------

!------------------------------------------------------------------------------
! About test
!------------------------------------------------------------------------------

    !--------------------------------------------------------------------------
    subroutine t_ascent_about
        type(C_PTR) cnode
        integer res

        !----------------------------------------------------------------------
        call set_case_name("t_ascent_about")
        !----------------------------------------------------------------------

        cnode = conduit_node_create()
        call ascent_about(cnode)
        call conduit_node_print(cnode)
        call conduit_node_destroy(cnode)

    end subroutine t_ascent_about

!------------------------------------------------------------------------------
end module t_f_ascent_smoke
!------------------------------------------------------------------------------

!------------------------------------------------------------------------------
program fortran_test
!------------------------------------------------------------------------------
  use fruit
  use t_f_ascent_smoke
  implicit none
  logical ok

  call init_fruit

  !----------------------------------------------------------------------------
  ! call our test routines
  !----------------------------------------------------------------------------
  call t_ascent_about

  call fruit_summary
  call fruit_finalize
  call is_all_successful(ok)

  if (.not. ok) then
     call exit(1)
  endif

!------------------------------------------------------------------------------
end program fortran_test
!------------------------------------------------------------------------------


