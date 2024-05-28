
cmake_minimum_required(VERSION 2.8.10)
project(test)

# brpc output path
execute_process(
    COMMAND bash -c "find ${PROJECT_SOURCE_DIR}/../.. -type d -regex \".*output/include$\" | head -n1 | xargs dirname | tr -d '\n'"
    OUTPUT_VARIABLE OUTPUT_PATH
)
set(CMAKE_PREFIX_PATH ${OUTPUT_PATH}) 

# protobuf
include(FindProtobuf)
protobuf_generate_cpp(PROTO_SRC PROTO_HEADER test.proto)

# brpc include
find_path(BRPC_INCLUDE_PATH NAMES brpc/server.h)
include_directories(${BRPC_INCLUDE_PATH})
find_path(BRPC_INCLUDE_PATH NAMES butil/logging.h)
include_directories(${BRPC_INCLUDE_PATH})

# brpc lib
find_library(BRPC_LIB NAMES libbrpc.a brpc)

# gflags
find_path(GFLAGS_INCLUDE_PATH gflags/gflags.h)
find_library(GFLAGS_LIBRARY NAMES gflags libgflags)
include_directories(${GFLAGS_INCLUDE_PATH})

# OpenSSL
find_package(OpenSSL)
include_directories(${OPENSSL_INCLUDE_DIR})

# LevelDB
find_path(LEVELDB_INCLUDE_PATH NAMES leveldb/db.h)
find_library(LEVELDB_LIB NAMES leveldb)
include_directories(${LEVELDB_INCLUDE_PATH})

# add libs
set(DYNAMIC_LIB
    pthread
    ${GFLAGS_LIBRARY}
    ${PROTOBUF_LIBRARIES}
    ${OPENSSL_CRYPTO_LIBRARY}
    ${OPENSSL_SSL_LIBRARY}
    ${LEVELDB_LIB}
    dl
    )

add_executable(test_client client.cpp ${PROTO_SRC} ${PROTO_HEADER})
add_executable(test_server server.cpp ${PROTO_SRC} ${PROTO_HEADER})

target_link_libraries(test_client ${BRPC_LIB} ${DYNAMIC_LIB})
target_link_libraries(test_server ${BRPC_LIB} ${DYNAMIC_LIB})
