#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif // !GLEW_STATIC

#ifndef GLFW_INCLUDE_GLU
#define GLFW_INCLUDE_GLU
#endif

#include "GL/glew.h"
#include "GLFW/glfw3.h"

/// Possible results of CheckGLError. Either successful or error
enum class GLResult
{
	FAILED,
	SUCCESS
};

namespace Details
{
	/// Performs OpenGL error handling via glGetError and outputs results into the log.
	/// \param openGLFunctionName
	///		Name of the last OpenGL function that should is implicitly checked.
	GLResult CheckGLError(const char* openGLFunctionName);

	/// Checks weather the given function pointer is not null and reports an error if it does not exist.
	/// \returns false if fkt is nullptr.
	bool CheckGLFunctionExistsAndReport(const char* openGLFunctionName, const void* fkt);
};

#if _DEBUG

	// Internal macro that defines what happens on an OpenGL call.
	/// Macro for normal gl call.
	#define __GL_CALL_HANDLING(openGLFunctionName, openGLFunction, ...) \
	{ \
		if (!CheckGLFunctionExistsAndReport(openGLFunctionName, openGLFunction)) \
			return GLResult::FAILED; \
		openGLFunction(__VA_ARGS__); \
		return CheckGLError(openGLFunctionName); \
	}

	/// Macro for gl call with return value.
	#define __GL_CALL_HANDLING_RET(openGLFunctionName, openGLFunction, ...) \
	{ \
		if (!CheckGLFunctionExistsAndReport	(openGLFunctionName, openGLFunction)) \
			return 0; \
		auto out = openGLFunction(__VA_ARGS__); \
		CheckGLError(openGLFunctionName); \
		return out; \
	}

#else

	// Internal macro that defines what happens on an OpenGL call.
	#define __GL_CALL_HANDLING(openGLFunctionName, openGLFunction, ...) \
	{ \
		openGLFunction(__VA_ARGS__); \
		return GLResult::SUCCESS; \
	}

	#define __GL_CALL_HANDLING_RET(openGLFunctionName, openGLFunction, ...) \
	{ \
		return openGLFunction(__VA_ARGS__); \
	}

#endif

namespace Details
{
	// GL call helper. The advantage of template function instead of a mere macro is that it can also be used in conditions!
	template<typename GlFunction, typename... Args>
	GLResult CheckedGLCall(const char* openGLFunctionName, GlFunction openGLFunction, Args... args) __GL_CALL_HANDLING(openGLFunctionName, openGLFunction, args...);

	template<typename GlFunction, typename... Args>
	GLuint CheckedGLCall_Ret(const char* openGLFunctionName, GlFunction openGLFunction, Args... args) __GL_CALL_HANDLING_RET(openGLFunctionName, openGLFunction, args...);
}

// Remove internal call handling helper macro.
#undef __GL_CALL_HANDLING
#undef __GL_CALL_HANDLING_RET

/// Recommend way to call any OpenGL function. Will perform optional nullptr and glGetError checks.
#define GL_CALL(OpenGLFunction, ...) \
	Details::CheckedGLCall(#OpenGLFunction, OpenGLFunction, __VA_ARGS__)

/// There are a few functions that have a return value (glGet, glIsX, glCreateShader, glCreateProgram). Use this macro for those.
#define GL_RET_CALL(OpenGLFunction, ...) \
	Details::CheckedGLCall_Ret(#OpenGLFunction, OpenGLFunction, __VA_ARGS__)

#define GL_RET_CALL_NO_ARG(OpenGLFunction) \
	Details::CheckedGLCall_Ret(#OpenGLFunction, OpenGLFunction)
