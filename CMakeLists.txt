cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)



project(Template)

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
#if(NOT CMAKE_VS_PLATFORM_NAME)
set(CMAKE_VS_PLATFORM_NAME_DEFAULT "Win32")
set(CMAKE_VS_PLATFORM_NAME "Win32")
#endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()




###BEGIN XR

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")



find_package(OpenXR REQUIRED)
find_package(Threads REQUIRED)

# Tell that we are using OpenGL.
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
if(WIN32)
  find_package(JSONCPP REQUIRED)
endif()
add_definitions(-DXR_USE_GRAPHICS_API_OPENGL)

# @todo We would like to use the OpenXR::openxr_loader target and have it import its
# include directories and libraries, but it has conflicts with the JSONCPP installer
# on Windows.  So we do this horrible hack to bring it in.
# Add include directories for standard Windows install
if(WIN32)
  include_directories("C:/Program Files (x86)/OPENXR/include")
  link_directories("C:/Program Files (x86)/OPENXR/lib")
endif()

# This is a helper graphics wrapping library that keeps track of what system include and library
# files are needed.  We have to make it a target before we include presentation so that it
# will have the system dependencies added to it.
add_library(openxr-gfxwrapper STATIC gfxwrapper/gfxwrapper_opengl.c gfxwrapper/gfxwrapper_opengl.h)
target_include_directories(openxr-gfxwrapper PUBLIC ${PROJECT_SOURCE_DIR}/external/include)
if(OPENGL_FOUND)
    if(TARGET OpenGL::OpenGL)
        target_link_libraries(openxr-gfxwrapper PUBLIC OpenGL::OpenGL)
    elseif(TARGET OpenGL::GL)
        target_link_libraries(openxr-gfxwrapper PUBLIC OpenGL::GL)
    else()
        target_link_libraries(openxr-gfxwrapper PUBLIC ${OPENGL_LIBRARIES})
    endif()
endif()

# Determine the presentation backend for Linux systems.
# Use an include because the code is pretty big.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    include(presentation)
endif()

# Several files use these compile-time OS switches
if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DXR_OS_WINDOWS)
    add_definitions(-DNOMINMAX)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DXR_OS_LINUX)
endif()

# Several files use these compile-time platform switches
if(WIN32)
    add_definitions(-DXR_USE_PLATFORM_WIN32)
elseif(PRESENTATION_BACKEND MATCHES "xlib")
    add_definitions(-DXR_USE_PLATFORM_XLIB)
elseif(PRESENTATION_BACKEND MATCHES "xcb")
    add_definitions(-DXR_USE_PLATFORM_XCB)
elseif(PRESENTATION_BACKEND MATCHES "wayland")
    add_definitions(-DXR_USE_PLATFORM_WAYLAND)
endif()

###END XR





################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    "Release_Small"
    "Run_Only"
    "Run_Only_Small"
    "Vitalize"
    "Vitalize_Small"
    CACHE STRING "" FORCE
)

################################################################################
# Global compiler options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_RELEASE "")
    set(CMAKE_CXX_FLAGS_RELEASE_SMALL "")
    set(CMAKE_CXX_FLAGS_RUN_ONLY "")
    set(CMAKE_CXX_FLAGS_RUN_ONLY_SMALL "")
    set(CMAKE_CXX_FLAGS_VITALIZE "")
    set(CMAKE_CXX_FLAGS_VITALIZE_SMALL "")
endif()

################################################################################
# Global linker options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_MODULE_LINKER_FLAGS "")
    set(CMAKE_SHARED_LINKER_FLAGS "")
    set(CMAKE_STATIC_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE_SMALL "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE_SMALL "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE_SMALL "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE_SMALL "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RUN_ONLY "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RUN_ONLY "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RUN_ONLY "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RUN_ONLY "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RUN_ONLY_SMALL "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RUN_ONLY_SMALL "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RUN_ONLY_SMALL "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RUN_ONLY_SMALL "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_VITALIZE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_VITALIZE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_VITALIZE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_VITALIZE "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_VITALIZE_SMALL "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_VITALIZE_SMALL "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_VITALIZE_SMALL "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_VITALIZE_SMALL "${CMAKE_STATIC_LINKER_FLAGS}")
endif()

################################################################################
# Nuget packages function stub.
################################################################################
function(use_package TARGET PACKAGE VERSION)
    message(WARNING "No implementation of use_package. Create yours. "
                    "Package \"${PACKAGE}\" with version \"${VERSION}\" "
                    "for target \"${TARGET}\" is ignored!")
endfunction()

################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################



























set(PROJECT_NAME Template)

################################################################################
# Source groups
################################################################################
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/gfxwrapper")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/sdk-files")
set(Header_Files
    "FCommon.h"
    "Main.h"
    "Resource.h"
    "geometry.h"
    "FusionXR.h"


)
source_group("Header Files" FILES ${Header_Files})

#set(Information
#    "../Help/CoxSdk.chm"
#)
#source_group("Information" FILES ${Information})

set(MMF_Headers
    #"../../../Program Files/Microsoft Visual Studio/Vc98/Include/Basetsd.h"
    "../../Inc/Ccx.h"
    "../../Inc/Ccxhdr.h"
    "../../Inc/CfcError.h"
    "../../Inc/CfcFile.h"
    "../../Inc/Cncf.h"
    "../../Inc/cncr.h"
    "../../Inc/Cncy.h"
    "../../Inc/Cnpdll.h"
    "../../Inc/colors.h"
    "../../Inc/EVTCCX.H"
    "../../Inc/Fill.h"
    "../../Inc/FilterMgr.h"
    "../../Inc/ImageFlt.h"
    "../../Inc/ImgFlt.h"
    "../../Inc/Palet.h"
    "../../Inc/PictEdDefs.h"
    "../../Inc/Props.h"
    "../../Inc/PTYPE.H"
    "../../Inc/SoundFilter.h"
    "../../Inc/SoundFilterMgr.h"
    "../../Inc/Surface.h"
    "../../Inc/TigsExt.hpp"
    "../../Inc/WinMacro.h"
)
source_group("MMF Headers" FILES ${MMF_Headers})

set(Source_Files
    "Edittime.cpp"
    "Ext.def"
    "General.cpp"
    "Main.cpp"
    "Runtime.cpp"
    "FusionXR.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Information}
    ${MMF_Headers}
    ${Source_Files}
)


################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})
use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Template)
target_sources(${PROJECT_NAME} PRIVATE Ext.rc)

################################################################################
# Target name
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    TARGET_NAME_DEBUG          "Template"
    TARGET_NAME_RELEASE_SMALL  "Template"
    TARGET_NAME_RELEASE        "Template"
    TARGET_NAME_RUN_ONLY_SMALL "Template"
    TARGET_NAME_RUN_ONLY       "Template"
    TARGET_NAME_VITALIZE_SMALL "Template"
    TARGET_NAME_VITALIZE       "Template"
)
################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_DEBUG          "${CMAKE_CURRENT_SOURCE_DIR}/./Debug"
    OUTPUT_DIRECTORY_RELEASE_SMALL  "${CMAKE_CURRENT_SOURCE_DIR}/./Release_Small"
    OUTPUT_DIRECTORY_RELEASE        "${CMAKE_CURRENT_SOURCE_DIR}/./Release"
    OUTPUT_DIRECTORY_RUN_ONLY_SMALL "${CMAKE_CURRENT_SOURCE_DIR}/./Run_Only_Small"
    OUTPUT_DIRECTORY_RUN_ONLY       "${CMAKE_CURRENT_SOURCE_DIR}/./Run_Only"
    OUTPUT_DIRECTORY_VITALIZE_SMALL "${CMAKE_CURRENT_SOURCE_DIR}/./Vitalize_Small"
    OUTPUT_DIRECTORY_VITALIZE       "${CMAKE_CURRENT_SOURCE_DIR}/./Vitalize"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG          "${CMAKE_CURRENT_SOURCE_DIR}/./Debug"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE_SMALL  "${CMAKE_CURRENT_SOURCE_DIR}/./Release_Small"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE        "${CMAKE_CURRENT_SOURCE_DIR}/./Release"
    ARCHIVE_OUTPUT_DIRECTORY_RUN_ONLY_SMALL "${CMAKE_CURRENT_SOURCE_DIR}/./Run_Only_Small"
    ARCHIVE_OUTPUT_DIRECTORY_RUN_ONLY       "${CMAKE_CURRENT_SOURCE_DIR}/./Run_Only"
    ARCHIVE_OUTPUT_DIRECTORY_VITALIZE_SMALL "${CMAKE_CURRENT_SOURCE_DIR}/./Vitalize_Small"
    ARCHIVE_OUTPUT_DIRECTORY_VITALIZE       "${CMAKE_CURRENT_SOURCE_DIR}/./Vitalize"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_NAME_DEBUG          "Template"
    ARCHIVE_OUTPUT_NAME_RELEASE_SMALL  "Template"
    ARCHIVE_OUTPUT_NAME_RELEASE        "Template"
    ARCHIVE_OUTPUT_NAME_RUN_ONLY_SMALL "Template"
    ARCHIVE_OUTPUT_NAME_RUN_ONLY       "Template"
    ARCHIVE_OUTPUT_NAME_VITALIZE_SMALL "Template"
    ARCHIVE_OUTPUT_NAME_VITALIZE       "Template"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    PDB_OUTPUT_DIRECTORY_DEBUG          "${CMAKE_CURRENT_SOURCE_DIR}/./Debug"
    PDB_OUTPUT_DIRECTORY_RELEASE_SMALL  "${CMAKE_CURRENT_SOURCE_DIR}/./Release_Small"
    PDB_OUTPUT_DIRECTORY_RELEASE        "${CMAKE_CURRENT_SOURCE_DIR}/./Release"
    PDB_OUTPUT_DIRECTORY_RUN_ONLY_SMALL "${CMAKE_CURRENT_SOURCE_DIR}/./Run_Only_Small"
    PDB_OUTPUT_DIRECTORY_RUN_ONLY       "${CMAKE_CURRENT_SOURCE_DIR}/./Run_Only"
    PDB_OUTPUT_DIRECTORY_VITALIZE_SMALL "${CMAKE_CURRENT_SOURCE_DIR}/./Vitalize_Small"
    PDB_OUTPUT_DIRECTORY_VITALIZE       "${CMAKE_CURRENT_SOURCE_DIR}/./Vitalize"
)
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
    $<$<CONFIG:Debug>:
        MultiThreadedDebug
    >
    $<$<CONFIG:Release_Small>:
        MultiThreaded
    >
    $<$<CONFIG:Release>:
        MultiThreaded
    >
    $<$<CONFIG:Run_Only_Small>:
        MultiThreaded
    >
    $<$<CONFIG:Run_Only>:
        MultiThreaded
    >
    $<$<CONFIG:Vitalize_Small>:
        MultiThreaded
    >
    $<$<CONFIG:Vitalize>:
        MultiThreaded
    >
    $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release_Small>,$<CONFIG:Release>,$<CONFIG:Run_Only_Small>,$<CONFIG:Run_Only>,$<CONFIG:Vitalize_Small>,$<CONFIG:Vitalize>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
)
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../../Inc"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "_DEBUG"
    ">"
    "$<$<CONFIG:Release_Small>:"
        "NDEBUG"
    ">"
    "$<$<CONFIG:Release>:"
        "NDEBUG"
    ">"
    "$<$<CONFIG:Run_Only_Small>:"
        "NDEBUG;"
        "RUN_ONLY"
    ">"
    "$<$<CONFIG:Run_Only>:"
        "NDEBUG;"
        "RUN_ONLY"
    ">"
    "$<$<CONFIG:Vitalize_Small>:"
        "NDEBUG;"
        "RUN_ONLY;"
        "VITALIZE"
    ">"
    "$<$<CONFIG:Vitalize>:"
        "NDEBUG;"
        "RUN_ONLY;"
        "VITALIZE"
    ">"
    "WIN32;"
    "_WINDOWS;"
    "STRICT"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /Gm;
            /Od;
            /RTC1;
            /ZI
        >
        $<$<CONFIG:Release_Small>:
            /O1;
            /Ob1;
            /GF;
            /Gy;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /GR
        >
        $<$<CONFIG:Release>:
            /O2;
            /Ob1;
            /GF;
            /Gy;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
        >
        $<$<CONFIG:Run_Only_Small>:
            /O1;
            /Ob1;
            /GF;
            /Gy;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /GR
        >
        $<$<CONFIG:Run_Only>:
            /O2;
            /Ob1;
            /GF;
            /Gy;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
        >
        $<$<CONFIG:Vitalize_Small>:
            /O1;
            /Ob1;
            /GF;
            /Gy;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /GR
        >
        $<$<CONFIG:Vitalize>:
            /O2;
            /Ob1;
            /GF;
            /Gy;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
        >
        /W3;
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /DEBUG;
            /INCREMENTAL
        >
        $<$<CONFIG:Release_Small>:
            /DEBUG:FULL;
            /INCREMENTAL:NO
        >
        $<$<CONFIG:Release>:
            /DEBUG:FULL;
            /INCREMENTAL:NO
        >
        $<$<CONFIG:Run_Only_Small>:
            /DEBUG:FULL;
            /INCREMENTAL:NO
        >
        $<$<CONFIG:Run_Only>:
            /DEBUG:FULL;
            /INCREMENTAL:NO
        >
        $<$<CONFIG:Vitalize_Small>:
            /DEBUG:FULL;
            /INCREMENTAL:NO
        >
        $<$<CONFIG:Vitalize>:
            /DEBUG:FULL;
            /INCREMENTAL:NO
        >
        /NOLOGO;
        /MACHINE:X86
    )
endif()

################################################################################
# Post build events
################################################################################
add_custom_command_if(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMANDS
    COMMAND  $<CONFIG:Release_Small> copy $<SHELL_PATH:$<TARGET_FILE:${PROJECT_NAME}>> .\\ToInstall\\Files\\Extensions
    COMMAND        $<CONFIG:Release> copy $<SHELL_PATH:$<TARGET_FILE:${PROJECT_NAME}>> .\\ToInstall\\Files\\Extensions
    COMMAND $<CONFIG:Run_Only_Small> copy $<SHELL_PATH:$<TARGET_FILE:${PROJECT_NAME}>> .\\ToInstall\\Files\\Data\\Runtime
    COMMAND       $<CONFIG:Run_Only> copy $<SHELL_PATH:$<TARGET_FILE:${PROJECT_NAME}>> .\\ToInstall\\Files\\Data\\Runtime
    COMMENT "Copy extension to installer directory"
)

################################################################################
# Dependencies
################################################################################
set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "kernel32;"
    "user32;"
    "gdi32;"
    "winspool;"
    "comdlg32;"
    "advapi32;"
    "shell32;"
    "ole32;"
    "oleaut32;"
    "uuid;"
    "../../../lib/mmfs2;"
	"openxr_loaderd;"
	"jsoncpp_lib;"
	"pathcch"
    "opengl32"
)
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

