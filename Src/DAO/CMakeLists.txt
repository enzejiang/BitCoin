cmake_minimum_required(VERSION 3.5)

PROJECT(DAO)

SET(CUR_SRC_DIR "${SRC_ROOT}/DAO")
INCLUDE_DIRECTORIES(
    "${CUR_SRC_DIR}/"
    "${SRC_ROOT}/"
    "${SRC_ROOT}/CommonBase"
    "${SRC_ROOT}/BlockEngine"
    "${SRC_ROOT}/NetWorkService"
    "${SRC_ROOT}/WalletService"
    )

SET(SRC_LIST
    "${CUR_SRC_DIR}/CAddrDB.cpp"
    "${CUR_SRC_DIR}/CReviewDB.cpp"
    "${CUR_SRC_DIR}/CTxDB.cpp"
    "${CUR_SRC_DIR}/CWalletDB.cpp"
    "${CUR_SRC_DIR}/CDB.cpp"
    "${CUR_SRC_DIR}/DaoServ.cpp"
    "${CUR_SRC_DIR}/db.cpp"
)


ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_LIST} )


INSTALL (TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_PATH})
