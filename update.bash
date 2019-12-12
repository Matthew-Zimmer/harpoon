#!/bin/bash

echo -e \
"target_include_directories(harpoon PUBLIC 
	\$<BUILD_INTERFACE:\${CMAKE_CURRENT_LIST_DIR}> 
	\$<INSTALL_INTERFACE:include/harpoon>)

set(headers $(find include/ -name *.hpp -printf \\n\\t\"%P\"))

install(FILES \${headers} DESTINATION include/harpoon)" > include/CMakeLists.txt


echo -e \
"target_sources(harpoon PUBLIC $(find include/ -name *.cpp -printf \\n\\t\"%P\"))" > src/CMakeLists.txt

