cmake_minimum_required(VERSION 3.5)

PROJECT(EnzeProto)

SET(CUR_SRC_DIR "${PROTO_SRC_PATH}")


INCLUDE_DIRECTORIES(
    "${CUR_SRC_DIR}/"
    "${SRC_ROOT}/"
    "${SRC_ROOT}/CommonBase"
    )

SET(PROTOBUF_PROTOC_EXECUTABLE protoc)

SET(SRC_LIST
    "${CUR_SRC_DIR}/Hello.proto"
    "${CUR_SRC_DIR}/Address.proto"
    "${CUR_SRC_DIR}/Inventory.proto"
    "${CUR_SRC_DIR}/GetBlocks.proto"
    "${CUR_SRC_DIR}/TxIn.proto"
    "${CUR_SRC_DIR}/TxOut.proto"
    "${CUR_SRC_DIR}/Transaction.proto"
    "${CUR_SRC_DIR}/Review.proto"
    "${CUR_SRC_DIR}/Block.proto"
    "${CUR_SRC_DIR}/Order.proto"
    "${CUR_SRC_DIR}/Version.proto"
    "${CUR_SRC_DIR}/Message.proto"
    "${CUR_SRC_DIR}/DiskTxPos.proto"
    "${CUR_SRC_DIR}/TxIndex.proto"
    "${CUR_SRC_DIR}/DiskBlockIndex.proto"
    "${CUR_SRC_DIR}/WalletTx.proto"
    "${CUR_SRC_DIR}/ServerMessage.proto"
    
)

EXEC_PROGRAM(rm ARGS -rf ${PROTO_GEN_PATH}/*.cc ${PROTO_GEN_PATH}/*.pb.h)

foreach(FIL ${SRC_LIST})
    get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
    get_filename_component(FIL_WE ${FIL} NAME_WE)

    EXEC_PROGRAM(${PROTOBUF_PROTOC_EXECUTABLE} ARGS 
        --cpp_out=${PROTO_GEN_PATH} 
        --proto_path=${CUR_SRC_DIR} ${ABS_FIL} )
    
    SET_SOURCE_FILES_PROPERTIES(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)
    LIST(APPEND {PROTO_SRCS} "${PROTO_GEN_PATH}/${FIL_WE}.pb.cc")
    LIST(APPEND {PROTO_HDRS} "${PROTO_GEN_PATH}/${FIL_WE}.pb.h")
    
endforeach()

#PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${SRC_LIST})

SET(PROTO_SRCS
    "${PROTO_GEN_PATH}/Hello.pb.cc"
    "${PROTO_GEN_PATH}/Address.pb.cc"
    "${PROTO_GEN_PATH}/Inventory.pb.cc"
    "${PROTO_GEN_PATH}/GetBlocks.pb.cc"
    "${PROTO_GEN_PATH}/TxOut.pb.cc"
    "${PROTO_GEN_PATH}/TxIn.pb.cc"
    "${PROTO_GEN_PATH}/Transaction.pb.cc"
    "${PROTO_GEN_PATH}/Review.pb.cc"
    "${PROTO_GEN_PATH}/Block.pb.cc"
    "${PROTO_GEN_PATH}/Order.pb.cc"
    "${PROTO_GEN_PATH}/Version.pb.cc"
    "${PROTO_GEN_PATH}/Message.pb.cc"
    "${PROTO_GEN_PATH}/DiskTxPos.pb.cc"
    "${PROTO_GEN_PATH}/TxIndex.pb.cc"
    "${PROTO_GEN_PATH}/DiskBlockIndex.pb.cc"
    "${PROTO_GEN_PATH}/WalletTx.pb.cc"
    "${PROTO_GEN_PATH}/ServerMessage.pb.cc"
    #    "${PROTO_GEN_PATH}/ProtocSerialize.cpp"
    )

ADD_LIBRARY(${PROJECT_NAME} SHARED ${PROTO_SRCS} )

INSTALL (TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_PATH})
