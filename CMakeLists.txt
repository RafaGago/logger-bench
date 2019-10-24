cmake_minimum_required(VERSION 3.5.1)
include (ExternalProject)
include(ExternalData)

set (VERSION_MAJOR 1)
set (VERSION_MINOR 0)
set (VERSION_REV 0)
set (VERSION_FULL "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REV}")

option(MALC "mini async log C enabled" on)
option(MAL "mini async log enabled" on)
option(NANOLOG "NanoLog enabled" off)
option(SPDLOG "spdlog enabled" off)
option(GLOG "Google log enabled" off)
option(G3LOG "G3 log enabled" off)
option(LLCPP "LLCP enabled" off)
option(NANOLOG2 "NanoLog(2) enabled" off)

project(logger_bench VERSION ${VERSION_FULL})
set (CMAKE_CXX_STANDARD 11)

#-- Own Binary -----------------------------------------------------------------

set(SOURCES ${CMAKE_SOURCE_DIR}/src)
set(LOGGER_INSTALL ${CMAKE_BINARY_DIR}/install)

add_executable(
    logger-bench
    ${SOURCES}/main.cpp
    )

target_include_directories(
    logger-bench PRIVATE
    ${SOURCES}
    ${LOGGER_INSTALL}/include/
    )

#-- Build system specific things -----------------------------------------------

execute_process(
    COMMAND         ${CMAKE_CXX_COMPILER} -dumpmachine
    OUTPUT_VARIABLE ARCH_TRIPLET
    )
string (STRIP ${ARCH_TRIPLET} ARCH_TRIPLET)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set (MESON_BUILDTYPE debug)
elseif(CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
    set (MESON_BUILDTYPE debugoptimized)
else()
    set (MESON_BUILDTYPE release)
endif()
#-- malc -----------------------------------------------------------------------

if (MALC)
    ExternalProject_Add(
        malc_artifacts
        GIT_REPOSITORY    https://github.com/RafaGago/mini-async-log-c.git
        GIT_TAG           a77d49632f7c9614b9271ba3b9014905f65bd5da
        CONFIGURE_COMMAND
            meson malc_build --prefix=/ --buildtype=${MESON_BUILDTYPE} -Dcompressed_builtins=false -Dcompressed_ptrs=false -Dbare=true
        UPDATE_COMMAND    ""
        BUILD_IN_SOURCE   1
        BUILD_COMMAND     ninja -C malc_build
        INSTALL_COMMAND   DESTDIR=${LOGGER_INSTALL} ninja -C malc_build install
        )
    add_library (malc STATIC IMPORTED)
    set_property(
        TARGET                     malc
        PROPERTY IMPORTED_LOCATION ${LOGGER_INSTALL}/lib/${ARCH_TRIPLET}/libmalc.a
        )
    add_dependencies(malc malc_artifacts)

    add_library (bl_base STATIC IMPORTED)
    set_property(
        TARGET                     bl_base
        PROPERTY IMPORTED_LOCATION ${LOGGER_INSTALL}/lib/${ARCH_TRIPLET}/libbl-base.a
        )
    add_dependencies (bl_base malc_artifacts)

    add_library (bl_nonblock STATIC IMPORTED)
    set_property(
        TARGET                     bl_nonblock
        PROPERTY IMPORTED_LOCATION ${LOGGER_INSTALL}/lib/${ARCH_TRIPLET}/libbl-nonblock.a
        )
    add_dependencies (bl_nonblock malc_artifacts)

    target_link_libraries (logger-bench malc bl_nonblock bl_base)
    target_compile_definitions (logger-bench PRIVATE HAS_MALC=1)
    target_sources (logger-bench PRIVATE ${SOURCES}/malc.cpp)
endif (MALC)

#-- mal ------------------------------------------------------------------------

if (MAL)
    ExternalProject_Add(
        mal_artifacts
        GIT_REPOSITORY https://github.com/RafaGago/mini-async-log.git
        GIT_TAG        dec11bca53482f44a9d52388dad1c5afafd4c146
        UPDATE_COMMAND ""
        INSTALL_DIR    ${LOGGER_INSTALL}
        CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=${LOGGER_INSTALL} -DBUILD_SHARED_LIBS=off
        )
    add_library (mal STATIC IMPORTED)
    set_property(
        TARGET                     mal
        PROPERTY IMPORTED_LOCATION ${LOGGER_INSTALL}/lib/libmini_async_log.a
        )
    add_dependencies (mal mal_artifacts)

    target_link_libraries (logger-bench mal)
    target_compile_definitions (logger-bench PRIVATE HAS_MAL=1)
    target_sources (logger-bench PRIVATE ${SOURCES}/mal.cpp)
endif (MAL)

#-- nanolog --------------------------------------------------------------------

if (NANOLOG)
    set (NANOLOG_REPO ${CMAKE_BINARY_DIR}/nanolog-git)

    ExternalProject_Add(
        nanolog_clone
        GIT_REPOSITORY    https://github.com/Iyengar111/NanoLog.git
        GIT_TAG           40a53c36e0336af45f7664abeb939f220f78273e
        SOURCE_DIR        ${NANOLOG_REPO}
        UPDATE_COMMAND    ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND     ""
        INSTALL_COMMAND   ""
        )
    add_dependencies (logger-bench nanolog_clone)
    target_compile_definitions (logger-bench PRIVATE HAS_NANOLOG=1)
    target_include_directories (logger-bench PRIVATE ${NANOLOG_REPO})
    target_sources (logger-bench PRIVATE ${SOURCES}/nanolog_bench.cpp)
endif (NANOLOG)

#-- spdlog ---------------------------------------------------------------------

if (SPDLOG)
    set (SPDLOG_REPO ${CMAKE_BINARY_DIR}/spdlog-git)

    ExternalProject_Add(
        spdlog_clone
        GIT_REPOSITORY    https://github.com/gabime/spdlog.git
        GIT_TAG           4858d7e454e66d347024a074749535b91ccbeed9
        SOURCE_DIR        ${SPDLOG_REPO}
        UPDATE_COMMAND    ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND     ""
        INSTALL_COMMAND   ""
        )
    add_dependencies (logger-bench spdlog_clone)
    target_compile_definitions (logger-bench PRIVATE HAS_SPDLOG=1)
    target_include_directories (logger-bench PRIVATE ${SPDLOG_REPO}/include)
    target_sources (logger-bench PRIVATE ${SOURCES}/spdlog.cpp)
endif (SPDLOG)

#-- glog -----------------------------------------------------------------------

if (GLOG)
    ExternalProject_Add(
        glog_artifacts
        GIT_REPOSITORY https://github.com/google/glog.git
        GIT_TAG        4cc89c9e2b452db579397887c37f302fb28f6ca1
        UPDATE_COMMAND ""
        INSTALL_DIR    ${LOGGER_INSTALL}
        CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=${LOGGER_INSTALL} -DBUILD_SHARED_LIBS=off
        )
    add_library (glog STATIC IMPORTED)
    set_property(
        TARGET                     glog
        PROPERTY IMPORTED_LOCATION ${LOGGER_INSTALL}/lib/libglog.a
        )
    add_dependencies (glog glog_artifacts)

    target_link_libraries (logger-bench glog)
    target_compile_definitions (logger-bench PRIVATE HAS_GLOG=1)
    target_sources (logger-bench PRIVATE ${SOURCES}/glog.cpp)
endif (GLOG)

#-- g3log ----------------------------------------------------------------------

if (G3LOG)
    ExternalProject_Add(
        g3log_artifacts
        GIT_REPOSITORY https://github.com/KjellKod/g3log.git
        GIT_TAG        f1491791785101d4ae948f8ecee7e9cc3e6b0be8
        UPDATE_COMMAND ""
        INSTALL_DIR    ${LOGGER_INSTALL}
        CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=${LOGGER_INSTALL} -DCPACK_INSTALL_PREFIX=${LOGGER_INSTALL} -DG3_SHARED_LIB=off
        )
    add_library (g3log STATIC IMPORTED)
    set_property(
        TARGET                     g3log
        PROPERTY IMPORTED_LOCATION ${LOGGER_INSTALL}/lib/libg3logger.a
        )
    add_dependencies (g3log g3log_artifacts)

    target_link_libraries (logger-bench g3log)
    target_compile_definitions (logger-bench PRIVATE HAS_G3LOG=1)
    target_sources (logger-bench PRIVATE ${SOURCES}/g3log.cpp)
    set_property(
        SOURCE ${SOURCES}/g3log.cpp APPEND
        PROPERTY COMPILE_FLAGS -std=gnu++17
        )
endif (G3LOG)

#-- llcpp ----------------------------------------------------------------------

if (LLCPP)
    set (LLCPP_REPO ${CMAKE_BINARY_DIR}/llcpp-git)

    ExternalProject_Add(
        llcpp_clone
        GIT_REPOSITORY    https://github.com/blapid/llcpp.git
        GIT_TAG           b6b079bb1f646188964d96fa81e5a9bf472f8137
        SOURCE_DIR        ${LLCPP_REPO}
        UPDATE_COMMAND    ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND     ""
        INSTALL_COMMAND   ""
        )
    add_dependencies (logger-bench llcpp_clone)
    target_compile_definitions (logger-bench PRIVATE HAS_LLCPP=1)
    target_include_directories (logger-bench PRIVATE ${LLCPP_REPO}/include)
    target_sources (logger-bench PRIVATE ${SOURCES}/llcpp.cpp)
    set_property(
        SOURCE ${SOURCES}/llcpp.cpp APPEND
        PROPERTY COMPILE_FLAGS -std=gnu++17
        )
endif (LLCPP)

#-- nanolog2--------------------------------------------------------------------

if (NANOLOG2)
    set (NANOLOG2_REPO ${CMAKE_BINARY_DIR}/nanolog2-git)

    ExternalProject_Add(
        nanolog2_artifacts
        GIT_REPOSITORY    https://github.com/PlatformLab/NanoLog.git
        GIT_TAG           dfdd4639b5731d69207dfd72a020e39972a8c720
        SOURCE_DIR        ${NANOLOG2_REPO}
        UPDATE_COMMAND    ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND     $(MAKE) -j4 -C ${NANOLOG2_REPO}/runtime
        INSTALL_COMMAND   ""
        )
    add_library (nanolog2 STATIC IMPORTED)
    set_property(
        TARGET                     nanolog2
        PROPERTY IMPORTED_LOCATION ${NANOLOG2_REPO}/runtime/libNanoLog.a
        )
    add_dependencies (nanolog2 nanolog2_artifacts)

    target_compile_definitions (logger-bench PRIVATE HAS_NANOLOG2=1)
    target_include_directories (logger-bench PRIVATE ${NANOLOG2_REPO}/runtime)
    target_sources (logger-bench PRIVATE ${SOURCES}/nanolog2.cpp)
    set_property(
        SOURCE ${SOURCES}/nanolog2.cpp APPEND
        PROPERTY COMPILE_FLAGS -std=gnu++17
        )
    target_link_libraries (logger-bench nanolog2 rt)
endif (NANOLOG2)

#-- System libraries -----------------------------------------------------------

find_package (Threads)
target_link_libraries (logger-bench  ${CMAKE_THREAD_LIBS_INIT})
