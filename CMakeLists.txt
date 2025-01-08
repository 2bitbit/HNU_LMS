cmake_minimum_required(VERSION 3.22)
project(H_LMS LANGUAGES CXX VERSION 1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dist)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
set(MY_PATH "C:/vcpkg/installed/x64-mingw-dynamic/share")

# --- SQLite3 ---
find_package(unofficial-sqlite3 CONFIG REQUIRED PATHS ${MY_PATH})
if(NOT unofficial-sqlite3_FOUND)
    include(FetchContent)
    set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
    FetchContent_Declare(sqlite3
        GIT_REPOSITORY https://github.com/sqlite/sqlite
        GIT_TAG version-3.47.2
    )
    FetchContent_GetProperties(sqlite3)
    if(NOT sqlite3_POPULATED)
        FetchContent_Populate(sqlite3)
        add_subdirectory(${sqlite3_SOURCE_DIR} ${sqlite3_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
endif()

# --- SQLiteCpp ---
find_package(SQLiteCpp CONFIG REQUIRED PATHS ${MY_PATH})
if(NOT SQLiteCpp_FOUND)
    include(FetchContent)
    set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
    FetchContent_Declare(SQLiteCpp
        GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp
        GIT_TAG 3.3.2
    )
    FetchContent_GetProperties(SQLiteCpp)
    if(NOT SQLiteCpp_POPULATED)
        FetchContent_Populate(SQLiteCpp)
        add_subdirectory(${SQLiteCpp_SOURCE_DIR} ${SQLiteCpp_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
endif()

# --- FTXUI ---
find_package(ftxui CONFIG REQUIRED PATHS ${MY_PATH})
if(NOT ftxui_FOUND)
    include(FetchContent)
    set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
    FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG v5.0.0
    )
    FetchContent_GetProperties(ftxui)
    if(NOT ftxui_POPULATED)
        FetchContent_Populate(ftxui)
        add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
endif()

# --- OpenSSL ---
find_package(OpenSSL CONFIG REQUIRED PATHS ${MY_PATH})
if(NOT OpenSSL_FOUND)
    include(FetchContent)
    set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
    FetchContent_Declare(OpenSSL
        GIT_REPOSITORY https://github.com/openssl/openssl
        GIT_TAG openssl-3.4.0
    )
    FetchContent_GetProperties(OpenSSL)
    if(NOT OpenSSL_POPULATED)
        FetchContent_Populate(OpenSSL)
        add_subdirectory(${OpenSSL_SOURCE_DIR} ${OpenSSL_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
endif()

file(GLOB_RECURSE YOUR_ALL_FILES
    "main.cpp"
)

# 给exe添加图标：写在add_executable()之前
if(WIN32)
    set(APP_ICON "${CMAKE_SOURCE_DIR}/resources/H_LMS_icon.ico")
    set(RC_FILE "${CMAKE_CURRENT_BINARY_DIR}/app.rc")
    file(WRITE ${RC_FILE} "IDI_ICON1 ICON \"${APP_ICON}\"\n")
    set(CMAKE_RC_COMPILER_INIT windres)
    enable_language(RC)
    set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
    list(APPEND YOUR_ALL_FILES ${RC_FILE})
endif()

add_executable(${PROJECT_NAME} ${YOUR_ALL_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC
    sqlite3
    SQLiteCpp
    ftxui::component
    ftxui::dom
    ftxui::screen
    OpenSSL::Crypto
    crypt32
    ws2_32
    shell32
)

# 复制部分依赖的 DLL 文件到输出目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND_EXPAND_LISTS
)