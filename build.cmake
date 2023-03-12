cmake_minimum_required(VERSION 2.8)

set(build_dir ${CMAKE_CURRENT_LIST_DIR}/build)
cmake_host_system_information(RESULT core_count QUERY NUMBER_OF_LOGICAL_CORES)

if(NOT EXISTS ${build_dir})
    file(MAKE_DIRECTORY ${build_dir})
endif()

execute_process(
        COMMAND ${CMAKE_COMMAND} -DBENCODE_BUILD_EXAMPLES=ON -DBENCODE_BUILD_TESTS=OFF ..
        WORKING_DIRECTORY ${build_dir}
)

execute_process(
        COMMAND ${CMAKE_COMMAND} --build . --parallel ${core_count}
        WORKING_DIRECTORY ${build_dir}
)

execute_process(
        COMMAND ctest -VV
        WORKING_DIRECTORY ${build_dir}
        RESULT_VARIABLE test_result
)

if(${test_result})
    message(FATAL_ERROR "test failed")
endif()