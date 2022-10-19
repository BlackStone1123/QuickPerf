cmake_minimum_required(VERSION 3.5)

project(QuickTest LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Quick AxContainer REQUIRED)

file(GLOB_RECURSE dataModelSource DataModel/*.cpp)
file(GLOB_RECURSE controllerSource Controller/*.cpp)
file(GLOB_RECURSE json Json/*.cpp)

add_executable(QuickTest
   main.cpp
   qml.qrc
   ${controllerSource}
   ${dataModelSource}
   ${json}
)

target_compile_definitions(QuickTest
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(QuickTest PRIVATE Qt5::Core Qt5::Quick Qt5::AxContainer)
