#!/bin/bash

echo -e \
"target_include_directories(harpoon PUBLIC 
	\$<BUILD_INTERFACE:\${CMAKE_CURRENT_LIST_DIR}> 
	\$<INSTALL_INTERFACE:include/harpoon>)

set(headers $(find include/ -name *.hpp -printf \\n\\t\"%P\"))

install(FILES \${headers} DESTINATION include/harpoon)" > include/CMakeLists.txt


echo -e \
"target_sources(harpoon PUBLIC $(find src/ -name *.cpp -printf \\n\\t\"\$\<BUILD_INTERFACE:\${CMAKE_CURRENT_LIST_DIR}/%f\>\"))" > src/CMakeLists.txt

echo -e \
"target_sources(harpoon_test PUBLIC $(find test/ -name *.cpp -printf \\n\\t\"\$\<BUILD_INTERFACE:\${CMAKE_CURRENT_LIST_DIR}/%f\>\"))" > test/CMakeLists.txt

