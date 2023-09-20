cmake_minimum_required(VERSION 3.24)

include(${CMAKE_CURRENT_BINARY_DIR}/local_conf.cmake OPTIONAL)

include(FetchContent)
FetchContent_Declare(
    ccommon
    GIT_REPOSITORY https://github.com/EddyTheCo/Common.git
    GIT_TAG main
)
FetchContent_MakeAvailable(ccommon)
version_from_git(
    LOG       OFF
    TIMESTAMP "%Y%m%d%H%M%S"
)
project(NFTMinter VERSION ${VERSION} LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
include(GNUInstallDirs)

find_package(Qt6 COMPONENTS Core Gui Quick Qml REQUIRED)

qt_standard_project_setup()

qt_add_executable(nftminter main.cpp)
FetchContent_Declare(
    qrCode
    GIT_REPOSITORY https://github.com/EddyTheCo/qrCode.git
    GIT_TAG v0.1.2
    FIND_PACKAGE_ARGS 0.1 CONFIG
)
FetchContent_MakeAvailable(qrCode)

FetchContent_Declare(
    midlePay
    GIT_REPOSITORY https://github.com/EddyTheCo/MidlePay.git
    GIT_TAG v0.0.4
    FIND_PACKAGE_ARGS 0.0 CONFIG
)
FetchContent_MakeAvailable(midlePay)
FetchContent_Declare(
    MyDesigns
    GIT_REPOSITORY https://github.com/EddyTheCo/MyDesigns.git
    GIT_TAG v0.2.3
    FIND_PACKAGE_ARGS 0.2 CONFIG
)
FetchContent_MakeAvailable(MyDesigns)
FetchContent_Declare(
    account
    GIT_REPOSITORY https://github.com/EddyTheCo/account.git
    GIT_TAG v0.2.2
    FIND_PACKAGE_ARGS 0.2 CONFIG
)
FetchContent_MakeAvailable(account)
FetchContent_Declare(
    nodeConection
    GIT_REPOSITORY https://github.com/EddyTheCo/ConectionSettings.git
    GIT_TAG v0.2.3
    FIND_PACKAGE_ARGS 0.2 CONFIG
)
FetchContent_MakeAvailable(nodeConection)

FetchContent_Declare(
    outMonitor
    GIT_REPOSITORY https://github.com/EddyTheCo/outMonitor.git
    GIT_TAG v0.1.5
    FIND_PACKAGE_ARGS 0.1 CONFIG
)
FetchContent_MakeAvailable(outMonitor)



set_source_files_properties(qml/LocalConf.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

qt6_add_qml_module(nftminter
    URI  NftMinter
    VERSION 1.0
    SOURCES
    src/CreateNFT.cpp include/CreateNFT.hpp
    QML_FILES
    qml/BoxNFT.qml
    qml/BoxMenu.qml
    qml/ConfDrawer.qml
    qml/window.qml
    qml/LocalConf.qml
    RESOURCE_PREFIX
    "/esterVtech.com/imports"
    IMPORT_PATH ${QML_IMPORT_PATH}
)
qt_add_resources(nftminter "nftminter.fonts"
    PREFIX "/esterVtech.com/imports/NftMinter"
    FILES
    "fonts/Anton/Anton-Regular.ttf"
    "fonts/Permanent_Marker/PermanentMarker-Regular.ttf"
)

target_link_libraries(nftminter PRIVATE Qt::Core Qt::Gui Qt::Qml Qt::Quick)


if(ANDROID)
    set_property(TARGET nftminter APPEND PROPERTY QT_ANDROID_MIN_SDK_VERSION 30)
    set_property(TARGET nftminter APPEND PROPERTY QT_ANDROID_TARGET_SDK_VERSION 34)
    set_property(TARGET nftminter APPEND PROPERTY QT_ANDROID_SDK_BUILD_TOOLS_REVISION 34.0.0)

    FetchContent_Declare(
        android_openssl
        DOWNLOAD_EXTRACT_TIMESTAMP true
        URL      https://github.com/KDAB/android_openssl/archive/refs/heads/master.zip
        #URL_HASH MD5=c97d6ad774fab16be63b0ab40f78d945 #optional
    )
if(NOT android_openssl_POPULATED)
    FetchContent_Populate(android_openssl)
    include(${android_openssl_SOURCE_DIR}/android_openssl.cmake)
    add_android_openssl_libraries(nftminter)
endif()


endif(ANDROID)

target_link_libraries(nftminter PRIVATE MyDesigns account nodeConection QtQrGen outMonitor midlePay)
set_target_properties(nftminter PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)


target_include_directories(nftminter PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

install(TARGETS nftminter
    BUNDLE  DESTINATION .
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(QTDEPLOY)
    qt_generate_deploy_qml_app_script(
        TARGET nftminter
        OUTPUT_SCRIPT deploy_script
    )
install(SCRIPT ${deploy_script})
endif(QTDEPLOY)
if(EMSCRIPTEN)
    add_custom_command(
        TARGET nftminter
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy "${CMAKE_CURRENT_BINARY_DIR}/nftminter.js" "${CMAKE_CURRENT_BINARY_DIR}/nftminter.wasm" "${CMAKE_CURRENT_BINARY_DIR}/qtloader.js" "${CMAKE_CURRENT_SOURCE_DIR}/wasm"
    )
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CPACK_PACKAGE_CONTACT "estervtech")
    set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-v${SEMVER}-${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_VERSION}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_CXX_COMPILER_ID}")
    if(USE_THREADS)
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-MThread")
    endif(USE_THREADS)
    include(CPack)
endif()
