set(target_name suitepp)

add_library(${target_name} INTERFACE)

target_include_directories(${target_name}
    INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}
)

set (target_for_ide "${target_name}_ide")
if (NOT TARGET ${target_for_ide})
    file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
    add_custom_target(${target_for_ide} SOURCES ${SOURCES})
endif()

