set(SOURCE_FILES
    src/main.cc
    src/free_list.cc
    src/allocator_utility.cc
)

set(HEADER_FILES
    include/allocator_utility.h
    include/header.h
    include/free_list_node.h
    include/free_list.h
    include/aliasing.h
)

add_executable(allocator_lib ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(allocator_lib PUBLIC include)
target_compile_features(allocator_lib PUBLIC cxx_std_23)
