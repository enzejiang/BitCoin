cmake_minimum_required(VERSION 3.5)

PROJECT(BlockEngine)

SET(CUR_SRC_DIR "${SRC_ROOT}/BlockEngine")
INCLUDE_DIRECTORIES(
    "${CUR_SRC_DIR}/"
    "${SRC_ROOT}/"
    "${SRC_ROOT}/CommonBase"
    "${SRC_ROOT}/DAO"
    "${SRC_ROOT}/NetWorkService"
    "${SRC_ROOT}/WalletService"
    )

SET(SRC_LIST
    "${CUR_SRC_DIR}/BlockEngine.cpp"
    "${CUR_SRC_DIR}/CBlock.cpp"
)


ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_LIST} )


INSTALL (TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_PATH})
