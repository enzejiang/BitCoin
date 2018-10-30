cmake_minimum_required(VERSION 3.5)

PROJECT(BitCoin)

SET(CUR_SRC_DIR "${SRC_ROOT}")


INCLUDE_DIRECTORIES(
    "${PROTO_SRC_PATH}"
    "${CUR_SRC_DIR}/"
    "${SRC_ROOT}/CommonBase"
    "${SRC_ROOT}/DAO"
    "${SRC_ROOT}/BlockEngine"
    "${SRC_ROOT}/WalletService"
    "${CUR_SRC_DIR}/NetWorkService"
    "${CUR_SRC_DIR}/CommonBase"
    )


SET(SRC_LIST
    "${CUR_SRC_DIR}/test.cpp"
    "${CUR_SRC_DIR}/TestTxSend.cpp"
)


#ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_LIST})
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST})

TARGET_LINK_LIBRARIES ( ${PROJECT_NAME}
        "BlockEngine"
        "DAO"
        "NetWorkService"
        "WalletService"
        "CommonBase"
        "EnzeProto"
        "zmq"
        "db_cxx"
        "crypto"
        "dl"
        "ssl"
        "${Protobuf_LIBRARIES}"
    )

INSTALL (TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_PATH})
