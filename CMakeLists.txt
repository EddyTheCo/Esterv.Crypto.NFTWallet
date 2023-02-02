cmake_minimum_required(VERSION 3.16)

project(IOTA VERSION 0.1 LANGUAGES CXX)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick)

include(local_conf.cmake OPTIONAL)

qt_add_executable(nftMinter main.cpp)

qt6_add_qml_module(nftMinter
    URI  NFT_Minter
    VERSION 1.0
    QML_FILES
    qml/window.qml
    SOURCES
    src/CreateNFT.cpp include/CreateNFT.hpp
    RESOURCE_PREFIX
    "/esthertech.com/imports"
    OUTPUT_TARGETS out_targets_var
    IMPORTS MyDesigns nodeConection account
    IMPORT_PATH
    ${CMAKE_BINARY_DIR}/_deps/mydesigns-build
    ${CMAKE_BINARY_DIR}/_deps/nodeconection-build
    ${CMAKE_BINARY_DIR}/_deps/account-build
)

target_link_libraries(nftMinter PUBLIC Qt6::Gui Qt6::Quick)
target_include_directories(nftMinter PUBLIC include)

include(FetchContent)
FetchContent_Declare(
    qrCode
    GIT_REPOSITORY git@github.com:EddyTheCo/qrCode.git
    GIT_TAG main
)
FetchContent_MakeAvailable(qrCode)

FetchContent_Declare(
    MyDesigns
    GIT_REPOSITORY git@github.com:EddyTheCo/MyDesigns.git
    GIT_TAG main
)
FetchContent_MakeAvailable(MyDesigns)
FetchContent_Declare(
    account
    GIT_REPOSITORY git@github.com:EddyTheCo/account.git
    GIT_TAG main 
)
FetchContent_MakeAvailable(account)
FetchContent_Declare(
    nodeConection
    GIT_REPOSITORY git@github.com:EddyTheCo/ConectionSettings.git
    GIT_TAG main
)
FetchContent_MakeAvailable(nodeConection)

target_link_libraries(nftMinter PUBLIC MyDesigns account nodeConection QtQrGen)

target_compile_definitions(nftMinter
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

if(EMSCRIPTEN)
    add_custom_command(
        TARGET nftMinter
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy "${CMAKE_CURRENT_BINARY_DIR}/nftMinter.js" "${CMAKE_CURRENT_BINARY_DIR}/nftMinter.wasm" "${CMAKE_CURRENT_BINARY_DIR}/qtloader.js" "${CMAKE_CURRENT_SOURCE_DIR}/wasm"
    )
endif()
