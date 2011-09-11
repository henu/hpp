#ifndef HPP_GLDEBUG_H
#define HPP_GLDEBUG_H

#include "display.h"
#include "exception.h"
#include "inc_gl.h"

#include <SDL/SDL.h>
#include <iostream>
#include <map>
#include <string>
#include <stdint.h>
#include <cstdlib>
#include <sstream>


// Macro HppCheckGlErrors()
#ifndef NDEBUG
	#define HppCheckGlErrors() Hpp::GlDebug::doErrorCheckOfGl(__FILE__, __LINE__, __FUNCTION__);
	#define HppCheckGlErrorsIfCorrectThread() Hpp::GlDebug::doErrorCheckOfGlIfCorrectThread(__FILE__, __LINE__, __FUNCTION__);
	#define HppCheckForCorrectThread() Hpp::GlDebug::ensureThisIsDisplayThread(__FILE__, __LINE__, __FUNCTION__);
#else
	#define HppCheckGlErrors()
	#define HppCheckGlErrorsIfCorrectThread()
	#define HppCheckForCorrectThread()
#endif

namespace Hpp
{

namespace GlDebug
{


// Functions to check if errors has been occured in OpenGL. If errors are found,
// then program is halted immediately.
inline void doErrorCheckOfGl(std::string file,
                             unsigned int linenumber,
                             std::string function);
inline void doErrorCheckOfGlIfCorrectThread(std::string file,
                                            unsigned int linenumber,
                                            std::string function);

// Ensures that this is the same thread than which display is opened at.
inline void ensureThisIsDisplayThread(std::string file,
                                      unsigned int linenumber,
                                      std::string function);

inline void doErrorCheckOfGl(std::string file,
                             unsigned int linenumber,
                             std::string function)
{

	// Ensure this is called from correct thread
	doAssertion(file, linenumber, function,
	                 Display::isThisRenderingThread(),
	                 "Display::isThisRenderingThread()",
	                 "HppCheckGlErrors() may only be called in main OpenGL thread!");

	// Convert possible error to string
	std::string error_str;
	GLenum error = glGetError();
	switch (error) {

	case GL_NO_ERROR:
		return;

	case GL_INVALID_ENUM:
		error_str = "Invalid enum!";
		break;

	case GL_INVALID_VALUE:
		error_str = "Numeric argument out of range!";
		break;

	case GL_INVALID_OPERATION:
		error_str = "Invalid operation!";
		break;

	case GL_STACK_OVERFLOW:
		error_str = "Stack overflow!";
		break;

	case GL_STACK_UNDERFLOW:
		error_str = "Stack underflow!";
		break;

	case GL_OUT_OF_MEMORY:
		error_str = "Out of memory!";
		break;

	default:
		std::ostringstream errorcode_strm;
		errorcode_strm << error;
		std::string errorcode_str;
		errorcode_str = errorcode_strm.str();
		error_str = "Unrecognized error #" +  errorcode_str + "!";
		break;

	}

	std::cerr << "----------------------------" << std::endl;
	std::cerr << "Error has occured in OpenGL!" << std::endl;
	std::cerr << "----------------------------" << std::endl;
	std::cerr << "Caught in" << std::endl;
	std::cerr << "File       : " << file << std::endl;
	std::cerr << "Line       : " << linenumber << std::endl;
	std::cerr << "Function   : " << function << std::endl;
	std::cerr << "Description: " << error_str << std::endl;

	#ifdef HPP_ASSERT_ABORTS
	abort();
	#else
	throw Exception("GL assertion has failed!");
	#endif
}


inline void doErrorCheckOfGlIfCorrectThread(std::string file,
                                            unsigned int linenumber,
                                            std::string function)
{

	// Ensure this is called from correct thread
	if (!Display::isThisRenderingThread()) {
		return;
	}

	// Convert possible error to string
	std::string error_str;
	GLenum error = glGetError();
	switch (error) {

	case GL_NO_ERROR:
		return;

	case GL_INVALID_ENUM:
		error_str = "Invalid enum!";
		break;

	case GL_INVALID_VALUE:
		error_str = "Numeric argument out of range!";
		break;

	case GL_INVALID_OPERATION:
		error_str = "Invalid operation!";
		break;

	case GL_STACK_OVERFLOW:
		error_str = "Stack overflow!";
		break;

	case GL_STACK_UNDERFLOW:
		error_str = "Stack underflow!";
		break;

	case GL_OUT_OF_MEMORY:
		error_str = "Out of memory!";
		break;

	default:
		std::ostringstream errorcode_strm;
		errorcode_strm << error;
		std::string errorcode_str;
		errorcode_str = errorcode_strm.str();
		error_str = "Unrecognized error #" +  errorcode_str + "!";
		break;

	}

	std::cerr << "----------------------------" << std::endl;
	std::cerr << "Error has occured in OpenGL!" << std::endl;
	std::cerr << "----------------------------" << std::endl;
	std::cerr << "Caught in" << std::endl;
	std::cerr << "File       : " << file << std::endl;
	std::cerr << "Line       : " << linenumber << std::endl;
	std::cerr << "Function   : " << function << std::endl;
	std::cerr << "Description: " << error_str << std::endl;

	#ifdef HPP_ASSERT_ABORTS
	abort();
	#else
	throw Exception("GL assertion has failed!");
	#endif

}


// ----------------------------------------
// ensureThisIsDisplayThread()
// ----------------------------------------

inline void ensureThisIsDisplayThread(std::string file,
                                      unsigned int linenumber,
                                      std::string function)
{

	// If things are okay, then quit from this check.
	if (Display::isThisRenderingThread()) {
		return;
	}

	std::cerr << "----------------------------" << std::endl;
	std::cerr << "Invalid usage of threads!" << std::endl;
	std::cerr << "----------------------------" << std::endl;
	std::cerr << "Caught in" << std::endl;
	std::cerr << "File       : " << file << std::endl;
	std::cerr << "Line       : " << linenumber << std::endl;
	std::cerr << "Function   : " << function << std::endl;
	std::cerr << "Description: Function is called from different thread than where display was opened at!" << std::endl;

	exit(EXIT_FAILURE);

}


}

}

#endif
