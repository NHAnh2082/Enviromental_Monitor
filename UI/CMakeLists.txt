cmake_minimum_required(VERSION 3.16)

project(UART VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2)
find_package(Qt6 REQUIRED COMPONENTS SerialPort)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appUART
    main.cpp
)

qt_add_qml_module(appUART
    URI UART
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES SerialPortManger.cpp
        SOURCES SerialPortManger.h
        QML_FILES SerialConfigTab.qml
        QML_FILES MonitorTab.qml
        QML_FILES CO2_icon.qml
        QML_FILES TimeSettingTab.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appUART PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appUART
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appUART
    PRIVATE Qt6::Quick
            Qt6::SerialPort
            Qt6::QuickControls2
            Qt6::Multimedia
)

include(GNUInstallDirs)
install(TARGETS appUART
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
