cmake_minimum_required(VERSION 3.5)

PROJECT(NetWorkService)

SET(CUR_SRC_DIR "${SRC_ROOT}/NetWorkService")
INCLUDE_DIRECTORIES(
    "${CUR_SRC_DIR}/"
    "${SRC_ROOT}/"
    "${SRC_ROOT}/CommonBase"
    "${SRC_ROOT}/DAO"
    "${SRC_ROOT}/BlockEngine"
    "${SRC_ROOT}/WalletService"
    )

SET(SRC_LIST
    "${CUR_SRC_DIR}/irc.cpp"
    "${CUR_SRC_DIR}/ZMQNode.cpp"
    "${CUR_SRC_DIR}/NetWorkThread.cpp"
    "${CUR_SRC_DIR}/NetWorkServ.cpp"
)


ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_LIST} )


INSTALL (TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_PATH})
