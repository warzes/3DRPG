#pragma once

#if defined(_MSC_VER)
#	pragma warning(push, 3)
//#	pragma warning(disable : 5039) 
#endif

#define _USE_MATH_DEFINES

#include <cmath>
#include <string>
#include <filesystem>
#include <algorithm>
#include <memory>
#include <chrono>
#include <array>

#include <glad/gl.h>

#if defined(_WIN32)
#include <glfw/glfw.h>
#endif

#if defined(__EMSCRIPTEN__)
#	include <emscripten/emscripten.h>
#	include <emscripten/html5.h>
#endif

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#define GLM_FORCE_XYZW_ONLY
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/type_aligned.hpp>

#include <stb/stb_image.h>
#include <stb/stb_truetype.h>

#include <ktx.h>

#include <tiny_obj_loader.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif