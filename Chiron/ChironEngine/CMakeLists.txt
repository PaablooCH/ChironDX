set(PROJECT_NAME ChironEngine)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "ChironEngine.h"
)
source_group("" FILES ${no_group_source_files})

set(Source
    "Source/Application.cpp"
    "Source/Application.h"
    "Source/Core.h"
    "Source/EntryPoint.h"
    "Source/Pch.cpp"
    "Source/Pch.h"
)
source_group("Source" FILES ${Source})

set(Source__Auxiliar
    "Source/Auxiliar/Formatter.cpp"
    "Source/Auxiliar/Formatter.h"
)
source_group("Source\\Auxiliar" FILES ${Source__Auxiliar})

set(Source__ChironClasses
    "Source/ChironClasses/ChironTodo.h"
    "Source/ChironClasses/Log.cpp"
    "Source/ChironClasses/Log.h"
)
source_group("Source\\ChironClasses" FILES ${Source__ChironClasses})

set(ALL_FILES
    ${no_group_source_files}
    ${Source}
    ${Source__Auxiliar}
    ${Source__ChironClasses}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/Source/Pch.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE ChironEngine)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUGENGINE   "${PROJECT__NAME}$ENV{TargetExt}"
        TARGET_NAME_DEBUGGAME     "${PROJECT__NAME}$ENV{TargetExt}"
        TARGET_NAME_RELEASEENGINE "${PROJECT__NAME}$ENV{TargetExt}"
        TARGET_NAME_RELEASEGAME   "${PROJECT__NAME}$ENV{TargetExt}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUGENGINE   "${PROJECT__NAME}$ENV{TargetExt}"
        TARGET_NAME_DEBUGGAME     "${PROJECT__NAME}$ENV{TargetExt}"
        TARGET_NAME_RELEASEENGINE "${PROJECT__NAME}$ENV{TargetExt}"
        TARGET_NAME_RELEASEGAME   "${PROJECT__NAME}$ENV{TargetExt}"
    )
endif()
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUGENGINE   "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/${PROJECT_NAME}"
        OUTPUT_DIRECTORY_DEBUGGAME     "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/${PROJECT_NAME}"
        OUTPUT_DIRECTORY_RELEASEENGINE "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/${PROJECT_NAME}"
        OUTPUT_DIRECTORY_RELEASEGAME   "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/${PROJECT_NAME}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUGENGINE   "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/${PROJECT_NAME}"
        OUTPUT_DIRECTORY_DEBUGGAME     "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/${PROJECT_NAME}"
        OUTPUT_DIRECTORY_RELEASEENGINE "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/${PROJECT_NAME}"
        OUTPUT_DIRECTORY_RELEASEGAME   "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/${PROJECT_NAME}"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASEENGINE "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASEGAME   "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASEENGINE "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASEGAME   "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/Source;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ExternalDependencies/Vulkan/Include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ExternalDependencies/SDL/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ExternalDependencies/MathGeoLib/Include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ExternalDependencies/ImGui;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ExternalDependencies/spdlog/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/Source;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ExternalDependencies/Vulkan/Include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ExternalDependencies/SDL/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ExternalDependencies/MathGeoLib/Include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ExternalDependencies/ImGui;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ExternalDependencies/spdlog/include"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:DebugEngine>:"
            "ENGINE;"
            "DEBUG"
        ">"
        "$<$<CONFIG:DebugGame>:"
            "DEBUG"
        ">"
        "$<$<CONFIG:ReleaseEngine>:"
            "ENGINE;"
            "NDEBUG;"
            "_CONSOLE"
        ">"
        "$<$<CONFIG:ReleaseGame>:"
            "NDEBUG;"
            "_CONSOLE"
        ">"
        "CORE;"
        "SPDLOG_ACTIVE_LEVEL=0;"
        "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING;"
        "CRN_PLATFORM_WINDOW;"
        "CRN_BUILD_DLL;"
        "_CRT_SECURE_NO_WARNINGS;"
        "NOMINMAX;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:DebugEngine>:"
            "ENGINE;"
            "DEBUG"
        ">"
        "$<$<CONFIG:DebugGame>:"
            "DEBUG"
        ">"
        "$<$<CONFIG:ReleaseEngine>:"
            "ENGINE;"
            "NDEBUG;"
            "_CONSOLE"
        ">"
        "$<$<CONFIG:ReleaseGame>:"
            "NDEBUG;"
            "_CONSOLE"
        ">"
        "CORE;"
        "SPDLOG_ACTIVE_LEVEL=0;"
        "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING;"
        "CRN_PLATFORM_WINDOW;"
        "CRN_BUILD_DLL;"
        "_CRT_SECURE_NO_WARNINGS;"
        "NOMINMAX;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:ReleaseEngine>:
                /MP;
                /Oi;
                /Gy
            >
            $<$<CONFIG:ReleaseGame>:
                /Oi;
                /Gy
            >
            /permissive-;
            /std:c++20;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:ReleaseEngine>:
                /MP;
                /Oi;
                /Gy
            >
            $<$<CONFIG:ReleaseGame>:
                /Oi;
                /Gy
            >
            /permissive-;
            /std:c++20;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:ReleaseEngine>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:ReleaseGame>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:WINDOWS
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:ReleaseEngine>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:ReleaseGame>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:WINDOWS
        )
    endif()
endif()

################################################################################
# Post build events
################################################################################

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND xcopy /y /d "$(OutDir)ChironEngine.dll" "$(SolutionDir)bin\\$(Configuration)-$(Platform)\\Sandbox\\"
)

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    ExternalDependencies
)

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:DebugEngine>:"
            "ExternalDependencies"
        ">"
        "$<$<CONFIG:DebugGame>:"
            "ExternalDependencies"
        ">"
        "$<$<CONFIG:ReleaseGame>:"
            "ExternalDependencies"
        ">"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:DebugEngine>:"
            "ExternalDependencies"
        ">"
        "$<$<CONFIG:DebugGame>:"
            "ExternalDependencies"
        ">"
        "$<$<CONFIG:ReleaseGame>:"
            "ExternalDependencies"
        ">"
    )
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:DebugEngine>:"
            "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/ExternalDependencies/"
        ">"
        "$<$<CONFIG:DebugGame>:"
            "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/ExternalDependencies/"
        ">"
        "$<$<CONFIG:ReleaseGame>:"
            "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/ExternalDependencies/"
        ">"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:DebugEngine>:"
            "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/ExternalDependencies/"
        ">"
        "$<$<CONFIG:DebugGame>:"
            "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/ExternalDependencies/"
        ">"
        "$<$<CONFIG:ReleaseGame>:"
            "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/ExternalDependencies/"
        ">"
    )
endif()

