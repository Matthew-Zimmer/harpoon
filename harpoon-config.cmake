get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(${SELF_DIR}/harpoon.cmake)

find_package(reflection REQUIRED)