cmake_minimum_required(VERSION 2.8)

PROJECT(BitCoin)

SET(CUR_SRC_DIR "${SRC_ROOT}/")

SET(SRC_LIST
    "${CUR_SRC_DIR}/db.cpp"
    "${CUR_SRC_DIR}/irc.cpp"
    "${CUR_SRC_DIR}/main.cpp"
    "${CUR_SRC_DIR}/market.cpp"
    "${CUR_SRC_DIR}/net.cpp"
    "${CUR_SRC_DIR}/script.cpp"
    "${CUR_SRC_DIR}/sha.cpp"
    "${CUR_SRC_DIR}/ui.cpp"
    "${CUR_SRC_DIR}/util.cpp"
    "${CUR_SRC_DIR}/test.cpp"
)


#ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_LIST})
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST})

TARGET_LINK_LIBRARIES ( ${PROJECT_NAME}
        "pthread"
        "db_cxx"
        "crypto"
        "dl"
        "ssl"
    )

