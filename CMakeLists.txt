cmake_minimum_required(VERSION 3.20.0)
project(WebSocetClient VERSION 0.1.0)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-O3") # Компиляция больших файлов
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

IF (WIN32)
    set(wxWidgets_ROOT_DIR "c:/wxwidgets/3.1.6tdm")
    set(wxWidgets_LIB_DIR "c:/wxwidgets/3.1.6tdm/lib/gcc1030TDM_x64_dll")
    set(BOOST_ROOT "c:/devel/boost")
    set(BOOST_INCLUDEDIR "c:/devel/boost/include/boost")
ENDIF()

if(MINGW)
    link_libraries(ws2_32 wsock32)
endif()

set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_NO_BOOST_CMAKE  ON) # additional set this one
set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_STATIC_RUNTIME   ON)

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)

find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})

file(GLOB project
    src/*
    src/websock/*
)
add_executable(${PROJECT_NAME} ${project})

if(wxWidgets_FOUND)
    target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PRIVATE ${wxWidgets_INCLUDE_DIRS})
    message(STATUS "Found wxWidgets ${wxWidgets_VERSION_STRING}")
else()
    message(STATUS "wxWidgets Not Found")
endif()

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
    message(STATUS "Found BOOST ${Boost_VERSION_STRING}")
else()
    message(STATUS "BOOST Not Found")
endif()

if (OPENSSL_FOUND)
    # Add the include directories for compiling
    target_include_directories(${PROJECT_NAME} PUBLIC ${OPENSSL_INCLUDE_DIR})
    # Add the static lib for linking
    target_link_libraries(${PROJECT_NAME} OpenSSL::SSL OpenSSL::Crypto)
    message(STATUS "Found OpenSSL ${OPENSSL_VERSION}")
else()
    message(STATUS "OpenSSL Not Found")
endif()