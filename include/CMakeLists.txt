target_include_directories(harpoon PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> 
	$<INSTALL_INTERFACE:include/harpoon>)

set(headers )

install(FILES ${headers} DESTINATION include/harpoon)
