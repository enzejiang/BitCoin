cmake_minimum_required(VERSION 3.5)

PROJECT(BitCoin)

SET(CUR_SRC_DIR "${SRC_ROOT}")


INCLUDE_DIRECTORIES(
    "${PROTO_SRC_PATH}"
    "${CUR_SRC_DIR}/"
    "${CUR_SRC_DIR}/Block"
    "${CUR_SRC_DIR}/TX"
    "${CUR_SRC_DIR}/Network"
    "${CUR_SRC_DIR}/Db"
    "${CUR_SRC_DIR}/CommonBase"
    )


SET(SRC_LIST
    #"${CUR_SRC_DIR}/CommonBase/main.cpp"
    "${CUR_SRC_DIR}/CommonBase/market.cpp"
    "${CUR_SRC_DIR}/CommonBase/script.cpp"
    "${CUR_SRC_DIR}/CommonBase/sha.cpp"
    "${CUR_SRC_DIR}/CommonBase/CommFunctionDef.cpp"
    "${CUR_SRC_DIR}/CommonBase/util.cpp"
    "${CUR_SRC_DIR}/CommonBase/BlockEngine.cpp"
    "${CUR_SRC_DIR}/Network/irc.cpp"
    "${CUR_SRC_DIR}/Network/net.cpp"
    "${CUR_SRC_DIR}/Block/CBlock.cpp"
    "${CUR_SRC_DIR}/TX/CTransaction.cpp"
    "${CUR_SRC_DIR}/TX/CTxIn.cpp"
    "${CUR_SRC_DIR}/TX/CMerkleTx.cpp"
    "${CUR_SRC_DIR}/TX/CWalletTx.cpp"
    "${CUR_SRC_DIR}/Db/CDB.cpp"
    "${CUR_SRC_DIR}/Db/CAddrDB.cpp"
    "${CUR_SRC_DIR}/Db/CReviewDB.cpp"
    "${CUR_SRC_DIR}/Db/CTxDB.cpp"
    "${CUR_SRC_DIR}/Db/CWalletDB.cpp"
    "${CUR_SRC_DIR}/Db/db.cpp"
    "${CUR_SRC_DIR}/test.cpp"
    "${CUR_SRC_DIR}/TestTxSend.cpp"
)


#ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_LIST})
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST})

TARGET_LINK_LIBRARIES ( ${PROJECT_NAME}
        "pthread"
        "db_cxx"
        "crypto"
        "dl"
        "ssl"
        #        "HelloProto"
        #${Protobuf_LIBRARIES}
    )

