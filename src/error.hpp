#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#ifndef ERROR_HPP_
#define ERROR_HPP_

namespace neurocid {

void print_stacktrace(FILE *out = stderr, unsigned int max_frames = 63);

inline void error(const std::string& msg) {
	std::cerr << "### Error: " << msg << std::endl;
	print_stacktrace();
	exit(1);
}

#ifndef NDEBUG
#define CHECK_MSG(exp, msg) if(!(exp)) neurocid::error(std::string(msg) + std::string(": ") + std::string(#exp));
#define CHECK(exp) if(!(exp)) neurocid::error(std::string("check failed: ") + std::string(#exp) + std::string(" (") + std::string(__FILE__) + std::string(":") + std::to_string(__LINE__) + std::string(")"))
#else
#define CHECK_MSG(exp, msg) ;
#define CHECK(exp) if(!(exp)) ;
#endif

} /* namespace neurcoid */

#endif /* ERROR_HPP_ */
