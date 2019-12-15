# Install script for directory: C:/Urho3D/Urho3D

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Urho3D")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM FILES
    "C:/Urho3D/Urho3D/bin/ConvertModels.bat"
    "C:/Urho3D/Urho3D/bin/Editor.bat"
    "C:/Urho3D/Urho3D/bin/NinjaSnowWar.bat"
    "C:/Urho3D/Urho3D/bin/PBRDemo.bat"
    "C:/Urho3D/Urho3D/bin/PBRDemoDeferred.bat"
    "C:/Urho3D/Urho3D/bin/PBRDemoDeferredHWDepth.bat"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/./CMake" TYPE DIRECTORY FILES "C:/Urho3D/Urho3D/CMake/")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/./Scripts" TYPE PROGRAM FILES
    "C:/Urho3D/Urho3D/cmake_android.bat"
    "C:/Urho3D/Urho3D/cmake_clean.bat"
    "C:/Urho3D/Urho3D/cmake_codeblocks.bat"
    "C:/Urho3D/Urho3D/cmake_codelite.bat"
    "C:/Urho3D/Urho3D/cmake_emscripten.bat"
    "C:/Urho3D/Urho3D/cmake_generic.bat"
    "C:/Urho3D/Urho3D/cmake_mingw.bat"
    "C:/Urho3D/Urho3D/cmake_ninja.bat"
    "C:/Urho3D/Urho3D/cmake_vs2008.bat"
    "C:/Urho3D/Urho3D/cmake_vs2010.bat"
    "C:/Urho3D/Urho3D/cmake_vs2012.bat"
    "C:/Urho3D/Urho3D/cmake_vs2013.bat"
    "C:/Urho3D/Urho3D/cmake_vs2015.bat"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Urho3D/Urho3D-VS2019/Source/cmake_install.cmake")
  include("C:/Urho3D/Urho3D-VS2019/Docs/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Urho3D/Urho3D-VS2019/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
