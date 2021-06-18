include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../)

add_executable(test1 test1.cpp)
target_compile_options(test1 PRIVATE -std=c++17 -DDEBUG -ggdb)
target_link_libraries(test1 pthread)

add_executable(start_stop_test start_stop_test.cpp)
target_compile_options(start_stop_test PRIVATE -std=c++17 -DDEBUG -ggdb)
target_link_libraries(start_stop_test pthread)

add_executable(start_stop_with_jobs_test start_stop_with_jobs.cpp)
target_compile_options(start_stop_with_jobs_test PRIVATE -std=c++17 -DDEBUG -ggdb)
target_link_libraries(start_stop_with_jobs_test pthread)

add_executable(sorting_test sorting_test.cpp)
target_compile_options(sorting_test PRIVATE -std=c++17 -DDEBUG -ggdb)
target_link_libraries(sorting_test pthread)

add_executable(sorting_test_withsleep sorting_test_withsleep.cpp)
target_compile_options(sorting_test_withsleep PRIVATE -std=c++17 -DDEBUG -ggdb)
target_link_libraries(sorting_test_withsleep pthread)
