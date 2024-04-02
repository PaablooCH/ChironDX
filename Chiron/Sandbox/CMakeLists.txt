set(PROJECT_NAME Sandbox)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "Source/SandboxApp.cpp"
)
source_group("" FILES ${no_group_source_files})

set(ALL_FILES
    ${no_group_source_files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Sandbox)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUGENGINE   "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/${PROJECT_NAME}/"
        OUTPUT_DIRECTORY_DEBUGGAME     "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/${PROJECT_NAME}/"
        OUTPUT_DIRECTORY_RELEASEENGINE "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/${PROJECT_NAME}/"
        OUTPUT_DIRECTORY_RELEASEGAME   "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/${PROJECT_NAME}/"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUGENGINE   "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/${PROJECT_NAME}/"
        OUTPUT_DIRECTORY_DEBUGGAME     "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/${PROJECT_NAME}/"
        OUTPUT_DIRECTORY_RELEASEENGINE "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/${PROJECT_NAME}/"
        OUTPUT_DIRECTORY_RELEASEGAME   "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/${PROJECT_NAME}/"
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
        "${CMAKE_CURRENT_SOURCE_DIR}/../ChironEngine;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ChironEngine/Source;"
        "${CMAKE_CURRENT_SOURCE_DIR}/Soruce;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ExternalDependencies/spdlog/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../ChironEngine;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ChironEngine/Source;"
        "${CMAKE_CURRENT_SOURCE_DIR}/Soruce;"
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
        "SPDLOG_ACTIVE_LEVEL=0;"
        "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING;"
        "CRN_PLATFORM_WINDOW;"
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
        "SPDLOG_ACTIVE_LEVEL=0;"
        "_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING;"
        "CRN_PLATFORM_WINDOW;"
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
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
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
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:ReleaseEngine>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:ReleaseGame>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:ReleaseEngine>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:ReleaseGame>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    ExternalDependencies
)

# Link with other targets.
target_link_libraries(${PROJECT_NAME} PRIVATE
    ChironEngine
)

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/ChironEngine/"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_VS_PLATFORM_NAME}/ChironEngine/"
    )
endif()

