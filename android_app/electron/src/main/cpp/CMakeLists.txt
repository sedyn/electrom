# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("electron")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

include_directories("libnode/include/node")

add_library( # Sets the name of the library.
        electron

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        log.cpp
        helper.cpp
        android_context.cpp

        node_bindings.cpp
        javascript_environment.cpp

        # Simple copy of gin in Chromium or gin_helper in Electron
        object_template_builder.cpp
        wrappable.cpp
        event_emitter_caller.cpp
        event_emitter.cpp
        trackable_object.cpp

        electron_main_parts.cpp

        electron_api_app.cpp
        electron_api_browser_window.cpp
        electron_api_ipc_main.cpp

        electron.cpp)


target_compile_options(electron PRIVATE -std=c++17)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

add_library(
        libnode
        SHARED
        IMPORTED)

set_target_properties(
        libnode
        PROPERTIES IMPORTED_LOCATION

        # Provides the path to the library you want to import.
        ${CMAKE_SOURCE_DIR}/libnode/bin/${ANDROID_ABI}/libnode.so)

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        electron
        libnode

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})
