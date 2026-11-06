message(STATUS "Configuring COURAGE codebase ...")

set(COURAGE_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/mjcf.c"
)

add_library(box3d_courage STATIC ${COURAGE_SRCS})
target_link_libraries(box3d_courage PRIVATE mujoco::mujoco)

message(STATUS "Done configuring COURAGE :D")
