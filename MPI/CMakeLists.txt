cmake_minimum_required(VERSION 3.12)
project(MPI)

set(CMAKE_CXX_STANDARD 14)



find_package(MPI REQUIRED)

if (MPI_FOUND)
    include_directories(${MPI_INCLUDE_PATH})
    add_executable(MPI main.cpp)
    target_link_libraries(mpi ${MPI_LIBRARIES})

    if(MPI_COMPILE_FLAGS)
        set_target_properties(mpi PROPERTIES
                COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
    endif()

    if(MPI_LINK_FLAGS)
        set_target_properties(mpi PROPERTIES
                LINK_FLAGS "${MPI_LINK_FLAGS}")
    endif()
else (MPI_FOUND)
    message(SEND_ERROR "This application cannot compile without MPI")
endif (MPI_FOUND)