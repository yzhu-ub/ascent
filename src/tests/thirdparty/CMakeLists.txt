###############################################################################
# Copyright (c) Lawrence Livermore National Security, LLC and other Ascent
# Project developers. See top-level LICENSE AND COPYRIGHT files for dates and
# other details. No copyright assignment is required to contribute to Ascent.
###############################################################################

################################
# TPL Smoke Tests
################################

set(smoke_device_deps)

if(CUDA_FOUND)
    list(APPEND smoke_device_deps cuda)
endif()

if(HIP_FOUND)
    list(APPEND smoke_device_deps blt_hip)
endif()

set(conduit_smoke_deps conduit::conduit)

if(ZLIB_FOUND)
    message(STATUS "Adding zlib smoke test")
    list(APPEND conduit_smoke_deps ZLIB::ZLIB)
    add_cpp_test(TEST t_zlib_smoke DEPENDS_ON ZLIB::ZLIB)
endif()

message(STATUS "Adding conduit smoke test")
add_cpp_test(TEST t_conduit_smoke DEPENDS_ON ${conduit_smoke_deps})

if(CALIPER_FOUND)
    message(STATUS "Adding caliper smoke test")
    add_cpp_test(TEST t_caliper_smoke DEPENDS_ON caliper)
endif()

if(RAJA_FOUND)
    message(STATUS "Adding raja smoke test")
    set(raja_smoke_deps RAJA ${smoke_device_deps})
    add_cpp_test(TEST t_raja_smoke DEPENDS_ON ${raja_smoke_deps})
endif()

if(UMPIRE_FOUND)
    message(STATUS "Adding umpire smoke test")
    set(umpire_smoke_deps umpire ${smoke_device_deps})
    add_cpp_test(TEST t_umpire_smoke DEPENDS_ON ${umpire_smoke_deps})
endif()

IF(VTKM_FOUND AND ENABLE_SERIAL)
    message(STATUS "Adding VTK-m smoke test")
    set(vtkm_smoke_deps vtkm)
    
    if(CUDA_FOUND)
        list(APPEND vtkm_smoke_deps cuda)
    endif()

    # Note: We may not need this if everything propgates
    # from VTK-m in the future
    if(HIP_FOUND)
        list(APPEND vtkm_smoke_deps blt_hip)
    endif()

    add_cpp_test(TEST t_vtkm_smoke DEPENDS_ON ${vtkm_smoke_deps})
    vtkm_add_target_information(t_vtkm_smoke
                                DEVICE_SOURCES t_vtkm_smoke.cpp)
    set_target_properties(t_vtkm_smoke PROPERTIES CXX_VISIBILITY_PRESET hidden)

    target_compile_definitions(t_vtkm_smoke PRIVATE "VTKH_FORCE_SERIAL")

    if (ENABLE_OPENMP)
        target_compile_definitions(t_vtkm_smoke PRIVATE "VTKH_FORCE_OPENMP")
    endif()

    if (CUDA_FOUND)
        target_compile_definitions(t_vtkm_smoke PRIVATE "VTKH_FORCE_CUDA")
    endif()

endif()

IF(MFEM_FOUND)
    message(STATUS "Adding mfem smoke test")
    set(mfem_smoke_deps mfem conduit::conduit)
    #check if mfem has device deps
    if(MFEM_CUDA_ENABLED OR MFEM_HIP_ENABLED)
         list(APPEND mfem_smoke_deps ${smoke_device_deps})
    endif()
    # mfem does not have both a serial version and an mpi
    # version. So if we compiled with MPI on, the we need
    # to set the correct dependency
    if(MPI_FOUND AND ENABLE_MPI)
        list(APPEND mfem_smoke_deps mpi)
    endif()
    add_cpp_test(TEST t_mfem_smoke DEPENDS_ON ${mfem_smoke_deps})
endif()

