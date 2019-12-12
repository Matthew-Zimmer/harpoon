cmake_minimum_required(VERSION 3.13)

project(harpoon)

add_library(harpoon PUBLIC)

add_subdirectory(include)
add_subdirectory(src)

target_compile_features(harpoon PUBLIC cxx_std_17)

#Set(CMAKE_CXX_FLAGS "-Wall" "-Wextra")

install(TARGETS harpoon EXPORT harpoon DESTINATION lib/harpoon)
install(EXPORT harpoon DESTINATION lib/harpoon)
install(FILES harpoon-config.cmake DESTINATION lib/harpoon)


#libs
find_Package(reflection REQUIRED)
target_link_libraries(harpoon PUBLIC reflection)