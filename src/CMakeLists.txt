target_sources(harpoon PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/main.cpp>"
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/process.cpp>")
