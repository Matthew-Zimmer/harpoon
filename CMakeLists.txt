cmake_minimum_required(VERSION 3.13)

project(harpoon)

add_library(harpoon "")
add_executable(harpoon_test "")

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)

target_compile_features(harpoon PUBLIC cxx_std_17)
target_compile_features(harpoon_test PUBLIC cxx_std_17)

#Set(CMAKE_CXX_FLAGS "-Wall" "-Wextra")

install(TARGETS harpoon EXPORT harpoon DESTINATION lib/harpoon)
install(EXPORT harpoon DESTINATION lib/harpoon)
install(FILES harpoon-config.cmake DESTINATION lib/harpoon)


#libs
#find_Package(reflection REQUIRED)
#target_link_libraries(harpoon PUBLIC reflection)
target_link_libraries(harpoon PUBLIC pthread)

#test libs
target_link_libraries(harpoon_test PUBLIC harpoon)