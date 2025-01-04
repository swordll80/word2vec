cmake_minimum_required(VERSION 3.10) # 建议使用较新的 CMake 版本

project(word2vec C)

# 设置 C 标准为最新可用版本
set(CMAKE_C_STANDARD 17) # 先设置一个基准，例如C99
set(CMAKE_C_STANDARD_REQUIRED ON) # 强制使用指定的标准

# 设置 C 编译选项，添加必要的警告和优化
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra") # linux
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4") # Windows release 下使用 /W4 和 /O2
if(CMAKE_BUILD_TYPE EQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Od")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MDd")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /O2")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MD")
endif()

# 创建可执行目标 (必须在链接库之前)
add_executable(word2vec word2vec.c)
add_executable(word2phrase word2phrase.c)
add_executable(distance distance.c)
add_executable(word-analogy word-analogy.c)
add_executable(compute-accuracy compute-accuracy.c)

# 查找 pthreads 库
find_path(PTHREADS_INCLUDE_DIR NAMES pthread.h HINTS "./pthreads-win32/include")
find_library(PTHREADS_LIBRARY NAMES pthreadVC3 HINTS "./pthreads-win32/lib/x64")

if(PTHREADS_INCLUDE_DIR AND PTHREADS_LIBRARY)
    include_directories(${PTHREADS_INCLUDE_DIR})
    target_link_libraries(word2vec ${PTHREADS_LIBRARY})
    target_link_libraries(word2phrase ${PTHREADS_LIBRARY})
    target_link_libraries(distance ${PTHREADS_LIBRARY})
    target_link_libraries(word-analogy ${PTHREADS_LIBRARY})
    target_link_libraries(compute-accuracy ${PTHREADS_LIBRARY})
else()
    message(FATAL_ERROR "Could not find pthreads")
endif()

# (可选) 设置输出目录，将可执行文件输出到 bin 目录
set_target_properties(word2vec PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(word2phrase PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(distance PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(word-analogy PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(compute-accuracy PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# (可选) 安装目标，方便后续部署
# install(TARGETS word2vec word2phrase distance word-analogy compute-accuracy DESTINATION bin)

# 处理数据文件（questions-words.txt 和 questions-phrases.txt）
# 将这些文件复制到构建目录，以便程序运行时可以找到它们
configure_file(questions-words.txt ${CMAKE_BINARY_DIR}/questions-words.txt COPYONLY)
configure_file(questions-phrases.txt ${CMAKE_BINARY_DIR}/questions-phrases.txt COPYONLY)


# 处理 shell 脚本 (demo-*.sh)，这些脚本需要在构建后手动运行或通过其他方式集成
# CMake 本身不直接执行 shell 脚本，但你可以添加自定义命令
# 示例（仅供参考，需要根据实际需求调整）：
# add_custom_target(run_demos
#     COMMAND ${CMAKE_COMMAND} -E echo "运行 demo 脚本 (请手动执行)"
# )
# add_dependencies(run_demos word2vec word2phrase distance word-analogy compute-accuracy) # 依赖于可执行文件的构建


# 获取 CMakeLists.txt 所在的目录
get_filename_component(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)

message(STATUS "Project directory: ${PROJECT_DIR}")

# 循环处理每个可执行文件
foreach(target word2vec word2phrase distance word-analogy compute-accuracy)
    message(STATUS "Target file path: $<TARGET_FILE:${target}>")
    add_custom_command(
        TARGET ${target}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${target}>
            ${PROJECT_DIR}
        COMMENT "Copying $<CONFIG> executable ${target} to project directory"
    )
endforeach()