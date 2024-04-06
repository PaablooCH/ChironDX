set(PROJECT_NAME ExternalDependencies)

################################################################################
# Source groups
################################################################################
set(ImGui
    "ImGui/imconfig.h"
    "ImGui/imgui.cpp"
    "ImGui/imgui.h"
    "ImGui/imgui_bezier.cpp"
    "ImGui/imgui_bezier.h"
    "ImGui/imgui_color_gradient.cpp"
    "ImGui/imgui_color_gradient.h"
    "ImGui/imgui_demo.cpp"
    "ImGui/imgui_draw.cpp"
    "ImGui/imgui_impl_opengl3.cpp"
    "ImGui/imgui_impl_opengl3.h"
    "ImGui/imgui_impl_opengl3_loader.h"
    "ImGui/imgui_impl_sdl.cpp"
    "ImGui/imgui_impl_sdl.h"
    "ImGui/imgui_internal.h"
    "ImGui/imgui_tables.cpp"
    "ImGui/imgui_widgets.cpp"
    "ImGui/ImGuizmo.cpp"
    "ImGui/ImGuizmo.h"
    "ImGui/imstb_rectpack.h"
    "ImGui/imstb_textedit.h"
    "ImGui/imstb_truetype.h"
)
source_group("ImGui" FILES ${ImGui})

set(MathGeoLib
    "MathGeoLib/Include/MathBuildConfig.h"
    "MathGeoLib/Include/MathGeoLib.h"
    "MathGeoLib/Include/MathGeoLibFwd.h"
)
source_group("MathGeoLib" FILES ${MathGeoLib})

set(MathGeoLib__Algorithm
    "MathGeoLib/Include/Algorithm/GJK.cpp"
    "MathGeoLib/Include/Algorithm/GJK.h"
    "MathGeoLib/Include/Algorithm/GJK2D.cpp"
    "MathGeoLib/Include/Algorithm/GJK2D.h"
    "MathGeoLib/Include/Algorithm/SAT.cpp"
    "MathGeoLib/Include/Algorithm/SAT.h"
)
source_group("MathGeoLib\\Algorithm" FILES ${MathGeoLib__Algorithm})

set(MathGeoLib__Algorithm__Random
    "MathGeoLib/Include/Algorithm/Random/LCG.cpp"
    "MathGeoLib/Include/Algorithm/Random/LCG.h"
)
source_group("MathGeoLib\\Algorithm\\Random" FILES ${MathGeoLib__Algorithm__Random})

set(MathGeoLib__Geometry
    "MathGeoLib/Include/Geometry/AABB.cpp"
    "MathGeoLib/Include/Geometry/AABB.h"
    "MathGeoLib/Include/Geometry/AABB2D.h"
    "MathGeoLib/Include/Geometry/Capsule.cpp"
    "MathGeoLib/Include/Geometry/Capsule.h"
    "MathGeoLib/Include/Geometry/Circle.cpp"
    "MathGeoLib/Include/Geometry/Circle.h"
    "MathGeoLib/Include/Geometry/Circle2D.cpp"
    "MathGeoLib/Include/Geometry/Circle2D.h"
    "MathGeoLib/Include/Geometry/Frustum.cpp"
    "MathGeoLib/Include/Geometry/Frustum.h"
    "MathGeoLib/Include/Geometry/GeometryAll.h"
    "MathGeoLib/Include/Geometry/GeomType.h"
    "MathGeoLib/Include/Geometry/HitInfo.h"
    "MathGeoLib/Include/Geometry/KDTree.h"
    "MathGeoLib/Include/Geometry/KDTree.inl"
    "MathGeoLib/Include/Geometry/Line.cpp"
    "MathGeoLib/Include/Geometry/Line.h"
    "MathGeoLib/Include/Geometry/LineSegment.cpp"
    "MathGeoLib/Include/Geometry/LineSegment.h"
    "MathGeoLib/Include/Geometry/LineSegment2D.cpp"
    "MathGeoLib/Include/Geometry/LineSegment2D.h"
    "MathGeoLib/Include/Geometry/OBB.cpp"
    "MathGeoLib/Include/Geometry/OBB.h"
    "MathGeoLib/Include/Geometry/OBB2D.h"
    "MathGeoLib/Include/Geometry/PBVolume.cpp"
    "MathGeoLib/Include/Geometry/PBVolume.h"
    "MathGeoLib/Include/Geometry/Plane.cpp"
    "MathGeoLib/Include/Geometry/Plane.h"
    "MathGeoLib/Include/Geometry/Polygon.cpp"
    "MathGeoLib/Include/Geometry/Polygon.h"
    "MathGeoLib/Include/Geometry/Polygon2DRef.h"
    "MathGeoLib/Include/Geometry/Polyhedron.cpp"
    "MathGeoLib/Include/Geometry/Polyhedron.h"
    "MathGeoLib/Include/Geometry/QuadTree.h"
    "MathGeoLib/Include/Geometry/QuadTree.inl"
    "MathGeoLib/Include/Geometry/Ray.cpp"
    "MathGeoLib/Include/Geometry/Ray.h"
    "MathGeoLib/Include/Geometry/Sphere.cpp"
    "MathGeoLib/Include/Geometry/Sphere.h"
    "MathGeoLib/Include/Geometry/Triangle.cpp"
    "MathGeoLib/Include/Geometry/Triangle.h"
    "MathGeoLib/Include/Geometry/Triangle2D.cpp"
    "MathGeoLib/Include/Geometry/Triangle2D.h"
    "MathGeoLib/Include/Geometry/TriangleMesh.cpp"
    "MathGeoLib/Include/Geometry/TriangleMesh.h"
    "MathGeoLib/Include/Geometry/TriangleMesh_IntersectRay_AVX.inl"
    "MathGeoLib/Include/Geometry/TriangleMesh_IntersectRay_CPP.inl"
    "MathGeoLib/Include/Geometry/TriangleMesh_IntersectRay_SSE.inl"
)
source_group("MathGeoLib\\Geometry" FILES ${MathGeoLib__Geometry})

set(MathGeoLib__Math
    "MathGeoLib/Include/Math/assume.h"
    "MathGeoLib/Include/Math/BitFuncs.h"
    "MathGeoLib/Include/Math/BitOps.cpp"
    "MathGeoLib/Include/Math/BitOps.h"
    "MathGeoLib/Include/Math/Callstack.cpp"
    "MathGeoLib/Include/Math/Callstack.h"
    "MathGeoLib/Include/Math/FixedPoint.h"
    "MathGeoLib/Include/Math/float2.cpp"
    "MathGeoLib/Include/Math/float2.h"
    "MathGeoLib/Include/Math/float2.inl"
    "MathGeoLib/Include/Math/float3.cpp"
    "MathGeoLib/Include/Math/float3.h"
    "MathGeoLib/Include/Math/float3x3.cpp"
    "MathGeoLib/Include/Math/float3x3.h"
    "MathGeoLib/Include/Math/float3x4.cpp"
    "MathGeoLib/Include/Math/float3x4.h"
    "MathGeoLib/Include/Math/float4.cpp"
    "MathGeoLib/Include/Math/float4.h"
    "MathGeoLib/Include/Math/float4_neon.h"
    "MathGeoLib/Include/Math/float4_sse.h"
    "MathGeoLib/Include/Math/float4d.cpp"
    "MathGeoLib/Include/Math/float4d.h"
    "MathGeoLib/Include/Math/float4x4.cpp"
    "MathGeoLib/Include/Math/float4x4.h"
    "MathGeoLib/Include/Math/float4x4_neon.h"
    "MathGeoLib/Include/Math/float4x4_sse.h"
    "MathGeoLib/Include/Math/FloatCmp.h"
    "MathGeoLib/Include/Math/grisu3.c"
    "MathGeoLib/Include/Math/grisu3.h"
    "MathGeoLib/Include/Math/grisu3_cpp.cpp"
    "MathGeoLib/Include/Math/InclWindows.h"
    "MathGeoLib/Include/Math/Interpolate.h"
    "MathGeoLib/Include/Math/MathAll.h"
    "MathGeoLib/Include/Math/MathConstants.h"
    "MathGeoLib/Include/Math/MathFunc.cpp"
    "MathGeoLib/Include/Math/MathFunc.h"
    "MathGeoLib/Include/Math/MathLog.cpp"
    "MathGeoLib/Include/Math/MathLog.h"
    "MathGeoLib/Include/Math/MathNamespace.h"
    "MathGeoLib/Include/Math/MathOps.cpp"
    "MathGeoLib/Include/Math/MathTypes.h"
    "MathGeoLib/Include/Math/Matrix.inl"
    "MathGeoLib/Include/Math/MatrixProxy.h"
    "MathGeoLib/Include/Math/MiniFloat.cpp"
    "MathGeoLib/Include/Math/MiniFloat.h"
    "MathGeoLib/Include/Math/MultiLineMacro.h"
    "MathGeoLib/Include/Math/myassert.h"
    "MathGeoLib/Include/Math/Polynomial.cpp"
    "MathGeoLib/Include/Math/Polynomial.h"
    "MathGeoLib/Include/Math/Quat.cpp"
    "MathGeoLib/Include/Math/Quat.h"
    "MathGeoLib/Include/Math/quat_simd.h"
    "MathGeoLib/Include/Math/Rect.h"
    "MathGeoLib/Include/Math/Reinterpret.h"
    "MathGeoLib/Include/Math/simd.h"
    "MathGeoLib/Include/Math/sse_mathfun.h"
    "MathGeoLib/Include/Math/SSEMath.cpp"
    "MathGeoLib/Include/Math/SSEMath.h"
    "MathGeoLib/Include/Math/Swap.h"
    "MathGeoLib/Include/Math/TransformOps.cpp"
    "MathGeoLib/Include/Math/TransformOps.h"
    "MathGeoLib/Include/Math/vec2d.h"
)
source_group("MathGeoLib\\Math" FILES ${MathGeoLib__Math})

set(MathGeoLib__Time
    "MathGeoLib/Include/Time/Clock.cpp"
    "MathGeoLib/Include/Time/Clock.h"
)
source_group("MathGeoLib\\Time" FILES ${MathGeoLib__Time})

set(ALL_FILES
    ${ImGui}
    ${MathGeoLib}
    ${MathGeoLib__Algorithm}
    ${MathGeoLib__Algorithm__Random}
    ${MathGeoLib__Geometry}
    ${MathGeoLib__Math}
    ${MathGeoLib__Time}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE ExternalDependencies)

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
        INTERPROCEDURAL_OPTIMIZATION_RELEASEENGINE "FALSE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASEGAME   "FALSE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASEENGINE "FALSE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASEGAME   "FALSE"
    )
endif()
################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/.;"
        "${CMAKE_CURRENT_SOURCE_DIR}/ImGui;"
        "${CMAKE_CURRENT_SOURCE_DIR}/SDL/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/MathGeoLib/Include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/Include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/.;"
        "${CMAKE_CURRENT_SOURCE_DIR}/ImGui;"
        "${CMAKE_CURRENT_SOURCE_DIR}/SDL/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/MathGeoLib/Include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/Include"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:DebugEngine>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugGame>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:ReleaseEngine>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseGame>:"
            "NDEBUG"
        ">"
        "_LIB;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:DebugEngine>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugGame>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:ReleaseEngine>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseGame>:"
            "NDEBUG"
        ">"
        "_LIB;"
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
            $<$<CONFIG:DebugEngine>:
                /JMC;
                /ZI
            >
            $<$<CONFIG:DebugGame>:
                /JMC;
                /ZI
            >
            $<$<CONFIG:ReleaseEngine>:
                /Oi-;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseGame>:
                /Oi-;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            /MP;
            /std:c++17;
            /sdl-;
            /W1;
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:DebugEngine>:
                /JMC;
                /ZI
            >
            $<$<CONFIG:DebugGame>:
                /JMC;
                /ZI
            >
            $<$<CONFIG:ReleaseEngine>:
                /Oi-;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseGame>:
                /Oi-;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            /MP;
            /std:c++17;
            /sdl-;
            /W1;
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
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "Shell32;"
        "SDL2;"
        "SDL2main;"
        "vulkan-1"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "Shell32;"
        "SDL2;"
        "SDL2main;"
        "vulkan-1"
    )
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/./SDL/lib/x64;"
        "${CMAKE_CURRENT_SOURCE_DIR}/./Vulkan/Lib"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/./SDL/lib/x64;"
        "${CMAKE_CURRENT_SOURCE_DIR}/./Vulkan/Lib"
    )
endif()

