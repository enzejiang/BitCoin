cmake_minimum_required(VERSION 3.5)

PROJECT(CommonBase)

SET(CUR_SRC_DIR "${SRC_ROOT}/CommonBase")
INCLUDE_DIRECTORIES(
    "${CUR_SRC_DIR}/"
    "${SRC_ROOT}/"
    "${SRC_ROOT}/BlockEngine"
    "${SRC_ROOT}/DAO"
    "${SRC_ROOT}/NetWorkService"
    "${SRC_ROOT}/WalletService"
    )

SET(SRC_LIST
    "${CUR_SRC_DIR}/CommFunctionDef.cpp"
    "${CUR_SRC_DIR}/market.cpp"
    "${CUR_SRC_DIR}/script.cpp"
    "${CUR_SRC_DIR}/sha.cpp"
    "${CUR_SRC_DIR}/util.cpp"
    "${CUR_SRC_DIR}/ProtocSerialize.cpp"
)


ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_LIST} )


INSTALL (TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_PATH})
