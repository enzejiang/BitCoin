cmake_minimum_required(VERSION 3.5)

PROJECT(WalletService)

SET(CUR_SRC_DIR "${SRC_ROOT}/WalletService")
INCLUDE_DIRECTORIES(
    "${CUR_SRC_DIR}/"
    "${SRC_ROOT}/"
    "${SRC_ROOT}/CommonBase"
    "${SRC_ROOT}/DAO"
    "${SRC_ROOT}/NetWorkService"
    "${SRC_ROOT}/BlockEngine"
    )

SET(SRC_LIST
    "${CUR_SRC_DIR}/CTransaction.cpp"
    "${CUR_SRC_DIR}/CTxIn.cpp"
    "${CUR_SRC_DIR}/CMerkleTx.cpp"
    "${CUR_SRC_DIR}/CWalletTx.cpp"
    "${CUR_SRC_DIR}/WalletServ.cpp"
)


ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_LIST} )


INSTALL (TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_PATH})
