cmake_minimum_required(VERSION 3.5.1)
include (ExternalProject)
include(ExternalData)

set (VERSION_MAJOR 1)
set (VERSION_MINOR 0)
set (VERSION_REV 0)
set (VERSION_FULL "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REV}")

option(MALC "mini async log C enabled" off)
option(MAL "mini async log enabled" off)
option(NANOLOG "NanoLog enabled" off)
option(SPDLOG "spdlog enabled" off)
option(GLOG "Google log enabled" off)
option(G3LOG "G3 log enabled" off)
option(NANOLOG2 "NanoLog(2) enabled" off)
option(ENABLE_ALL "Enables all loggers, overrides other individual values" off)

project(logger_bench VERSION ${VERSION_FULL})
set (CMAKE_CXX_STANDARD 11)

set (HAS_LOGGERS false)

if (ENABLE_ALL)
    set (MALC on)
    set (MAL on)
    set (NANOLOG on)
    set (SPDLOG on)
    set (GLOG on)
    set (G3LOG on)
    set (NANOLOG2 on)
endif()

#-- Own Binary -----------------------------------------------------------------

set(SOURCES ${CMAKE_SOURCE_DIR}/src)
set(LOGGER_INSTALL ${CMAKE_BINARY_DIR}/install)

add_executable(
    logger-bench
    ${SOURCES}/main.cpp
    ${SOURCES}/cpuclock.cpp
    )

target_include_directories(
    logger-bench PRIVATE
    ${SOURCES}
    ${LOGGER_INSTALL}/include/
    )

if (NOT LOGGER_MEMORY_KB)
    set(LOGGER_MEMORY_KB 8192)
endif()

target_compile_definitions(logger-bench PRIVATE LOGGER_MEMORY_KB=${LOGGER_MEMORY_KB})

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
    set (CMAKE_BUILD_TYPE Release)
endif()

#-- Google benchmark -----------------------------------------------------------

set (GBENCHMARK_REPO ${CMAKE_BINARY_DIR}/gbenchmark-git)

ExternalProject_Add(
    gbenchmark_artifacts
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        v1.5.0
    SOURCE_DIR     ${GBENCHMARK_REPO}
    UPDATE_COMMAND ""
    INSTALL_DIR    ${LOGGER_INSTALL}
    CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=${LOGGER_INSTALL} -DBENCHMARK_ENABLE_TESTING=off -DBENCHMARK_ENABLE_GTEST_TESTS=off -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        )
add_library (gbenchmark STATIC IMPORTED)
set_property(
    TARGET                     gbenchmark
    PROPERTY IMPORTED_LOCATION ${LOGGER_INSTALL}/lib/libbenchmark.a
    )
add_dependencies (gbenchmark gbenchmark_artifacts)

add_library (gbenchmark_main STATIC IMPORTED)
set_property(
    TARGET                     gbenchmark_main
    PROPERTY IMPORTED_LOCATION ${LOGGER_INSTALL}/lib/libbenchmark_main.a
    )
add_dependencies (gbenchmark_main gbenchmark_artifacts)

target_include_directories (logger-bench PRIVATE ${GBENCHMARK_REPO}/src)
target_link_libraries (logger-bench gbenchmark gbenchmark_main)

#-- malc -----------------------------------------------------------------------

if (MALC)
    set (HAS_LOGGERS true)

    ExternalProject_Add(
        malc_artifacts
        GIT_REPOSITORY    https://github.com/RafaGago/mini-async-log-c.git
        GIT_TAG           9844be93aa2e34a406e6c7de091c19dde9078096
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

    add_library (malcpp STATIC IMPORTED)
    set_property(
        TARGET                     malcpp
        PROPERTY IMPORTED_LOCATION ${LOGGER_INSTALL}/lib/${ARCH_TRIPLET}/libmalcpp.a
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

    add_library (bl_t_extras STATIC IMPORTED)
    set_property(
        TARGET                     bl_t_extras
        PROPERTY IMPORTED_LOCATION ${LOGGER_INSTALL}/lib/${ARCH_TRIPLET}/libbl-time-extras.a
        )
    add_dependencies (bl_t_extras malc_artifacts)

    target_link_libraries (logger-bench malcpp malc bl_nonblock bl_base bl_t_extras)
    target_compile_definitions (logger-bench PRIVATE HAS_MALC=1)
    target_sources (logger-bench PRIVATE ${SOURCES}/malc.cpp)
endif (MALC)

#-- mal ------------------------------------------------------------------------

if (MAL)
    set (HAS_LOGGERS true)

    ExternalProject_Add(
        mal_artifacts
        GIT_REPOSITORY https://github.com/RafaGago/mini-async-log.git
        GIT_TAG        dec11bca53482f44a9d52388dad1c5afafd4c146
        UPDATE_COMMAND ""
        INSTALL_DIR    ${LOGGER_INSTALL}
        CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=${LOGGER_INSTALL} -DBUILD_SHARED_LIBS=off -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
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

if (NANOLOG_IYENGAR)
    set (HAS_LOGGERS true)

    set (NANOLOG_IYENGAR_REPO ${CMAKE_BINARY_DIR}/nanolog-iyengar-git)

    ExternalProject_Add(
        nanolog_iyengar_clone
        GIT_REPOSITORY    https://github.com/Iyengar111/NanoLog.git
        GIT_TAG           40a53c36e0336af45f7664abeb939f220f78273e
        SOURCE_DIR        ${NANOLOG_IYENGAR_REPO}
        UPDATE_COMMAND    ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND     ""
        INSTALL_COMMAND   ""
        )
    add_dependencies (logger-bench nanolog_iyengar_clone)
    target_compile_definitions (logger-bench PRIVATE HAS_NANOLOG_IYENGAR=1)
    target_include_directories (logger-bench PRIVATE ${NANOLOG_IYENGAR_REPO})
    target_sources (logger-bench PRIVATE ${SOURCES}/nanolog_iyengar.cpp)
endif (NANOLOG_IYENGAR)

#-- spdlog ---------------------------------------------------------------------

if (SPDLOG)
    set (HAS_LOGGERS true)

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
    set (HAS_LOGGERS true)

    ExternalProject_Add(
        glog_artifacts
        GIT_REPOSITORY https://github.com/google/glog.git
        GIT_TAG        4cc89c9e2b452db579397887c37f302fb28f6ca1
        UPDATE_COMMAND ""
        INSTALL_DIR    ${LOGGER_INSTALL}
        CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=${LOGGER_INSTALL} -DBUILD_SHARED_LIBS=off -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
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
    set (HAS_LOGGERS true)

    ExternalProject_Add(
        g3log_artifacts
        GIT_REPOSITORY https://github.com/KjellKod/g3log.git
        GIT_TAG        f1491791785101d4ae948f8ecee7e9cc3e6b0be8
        UPDATE_COMMAND ""
        INSTALL_DIR    ${LOGGER_INSTALL}
        CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=${LOGGER_INSTALL} -DCPACK_INSTALL_PREFIX=${LOGGER_INSTALL} -DG3_SHARED_LIB=off -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
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

#-- nanolog---------------------------------------------------------------------

if (NANOLOG)
    set (HAS_LOGGERS true)

    set (NANOLOG_REPO ${CMAKE_BINARY_DIR}/nanolog-standford-git)

    ExternalProject_Add(
        nanolog_artifacts
        GIT_REPOSITORY    https://github.com/PlatformLab/NanoLog.git
        GIT_TAG           dfdd4639b5731d69207dfd72a020e39972a8c720
        SOURCE_DIR        ${NANOLOG_REPO}
        UPDATE_COMMAND    ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND     $(MAKE) -j4 -C ${NANOLOG_REPO}/runtime libNanoLog.a
        INSTALL_COMMAND   ""
        )
    add_library (nanolog STATIC IMPORTED)
    set_property(
        TARGET                     nanolog
        PROPERTY IMPORTED_LOCATION ${NANOLOG_REPO}/runtime/libNanoLog.a
        )
    add_dependencies (nanolog nanolog_artifacts)

    target_compile_definitions (logger-bench PRIVATE HAS_NANOLOG=1)
    target_include_directories (logger-bench PRIVATE ${NANOLOG_REPO}/runtime)
    target_sources (logger-bench PRIVATE ${SOURCES}/nanolog.cpp)
    set_property(
        SOURCE ${SOURCES}/nanolog.cpp APPEND
        PROPERTY COMPILE_FLAGS -std=gnu++17
        )
    target_link_libraries (logger-bench nanolog rt)
endif (NANOLOG)

#-- System libraries -----------------------------------------------------------

if (NOT HAS_LOGGERS)
    message(
        FATAL_ERROR
        "No loggers specified. Check out the options on top of CMakeLists.txt"
        )
endif()

find_package (Threads)
target_link_libraries (logger-bench  ${CMAKE_THREAD_LIBS_INIT})
