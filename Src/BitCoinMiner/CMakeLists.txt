cmake_minimum_required(VERSION 2.8)

PROJECT(DBServer)

SET(CUR_SRC_DIR "${SRC_ROOT}/DBServer")
INCLUDE_DIRECTORIES(
    "${SRC_ROOT}/"
    "${CUR_SRC_DIR}/"
    "${INCLUDE_PATH}/"
    "${INCLUDE_PATH}/ace"
    "${INCLUDE_PATH}/COS"
    "${INCLUDE_PATH}/omniORB4"
    "${INCLUDE_PATH}/omnithread"
    "${INCLUDE_PATH}/omniORB4/internal"
    )

SET(SRC_LIST
    "${CUR_SRC_DIR}/DBServer_Impl.cpp"
    "${CUR_SRC_DIR}/main.cpp"
)

LINK_DIRECTORIES( 
    "${LIB_PATH}"
    )

ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST})

TARGET_LINK_LIBRARIES( ${PROJECT_NAME}
    "Utilities"
    "CommonBase"
    "ACE"
    "omnithread"
    "omniORB4"
    "omniDynamic4"
    )

INSTALL (TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_PATH})
