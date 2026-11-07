if(NOT TARGET box3d::box3d)
    message(STATUS "Must have defined the box3d target by now T_T")
    return()
endif()

message(STATUS "Configuring COURAGE codebase ...")

set(COURAGE_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/mjcf.c"
)

add_library(box3d_courage STATIC ${COURAGE_SRCS})
target_link_libraries(box3d_courage PRIVATE box3d::box3d)
target_link_libraries(box3d_courage PUBLIC mujoco::mujoco)

message(STATUS "Done configuring COURAGE :D")
