#include "2d.hpp"
#include <iostream>
#include <string>
#include <functional>

using namespace tankwar;
using std::cerr;
using std::endl;
using std::string;
using std::function;

#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <cxxabi.h>

/** Print a demangled stack backtrace of the caller function to FILE* out. */
static inline void print_stacktrace(FILE *out = stderr, unsigned int max_frames = 63)
{
    fprintf(out, "stack trace:\n");

    // storage array for stack trace address data
    void* addrlist[max_frames+1];

    // retrieve current stack addresses
    int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

    if (addrlen == 0) {
	fprintf(out, "  <empty, possibly corrupt>\n");
	return;
    }

    // resolve addresses into strings containing "filename(function+address)",
    // this array must be free()-ed
    char** symbollist = backtrace_symbols(addrlist, addrlen);

    // allocate string which will be filled with the demangled function name
    size_t funcnamesize = 256;
    char* funcname = (char*)malloc(funcnamesize);

    // iterate over the returned symbol lines. skip the first, it is the
    // address of this function.
    for (int i = 1; i < addrlen; i++)
    {
	char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

	// find parentheses and +address offset surrounding the mangled name:
	// ./module(function+0x15c) [0x8048a6d]
	for (char *p = symbollist[i]; *p; ++p)
	{
	    if (*p == '(')
		begin_name = p;
	    else if (*p == '+')
		begin_offset = p;
	    else if (*p == ')' && begin_offset) {
		end_offset = p;
		break;
	    }
	}

	if (begin_name && begin_offset && end_offset
	    && begin_name < begin_offset)
	{
	    *begin_name++ = '\0';
	    *begin_offset++ = '\0';
	    *end_offset = '\0';

	    // mangled name is now in [begin_name, begin_offset) and caller
	    // offset in [begin_offset, end_offset). now apply
	    // __cxa_demangle():

	    int status;
	    char* ret = abi::__cxa_demangle(begin_name,
					    funcname, &funcnamesize, &status);
	    if (status == 0) {
		funcname = ret; // use possibly realloc()-ed string
		fprintf(out, "  %s : %s ",
			symbollist[i], funcname);
			char syscom[256];
			sprintf(syscom,"addr2line %p -e tests/tests", addrlist[i]);
			system(syscom);
	    }
	    else {
		// demangling failed. Output function name as a C function with
		// no arguments.
		fprintf(out, "  %s : %s()+%s\n",
			symbollist[i], begin_name, begin_offset);
	    }
	}
	else
	{
	    // couldn't parse the line? print the whole line.
	    fprintf(out, "  %s\n", symbollist[i]);
	}
    }

    free(funcname);
    free(symbollist);
}


template<typename T> void assertPrint(string file, size_t line, T t1, T t2, string strOp, function<bool()> op) {
	if(!op()) {
		cerr << "failed " << file << ':' << line << " " << t1 << strOp << t2 << endl;
		print_stacktrace();
		exit(1);
	}
}

#define assertVeq(T1,T2) assertPrint(__FILE__, __LINE__, T1, T2, "==", [=](){ return fabs(T1.x - T2.x) < 0.00001 && fabs(T1.y - T2.y) < 0.00001;});
#define assertVne(T1,T2) assertPrint(__FILE__, __LINE__, T1, T2, "!=", [=](){ return fabs(T1.x - T2.x) >= 0.00001 && fabs(T1.y - T2.y) >= 0.00001;});
#define assertEq(T1,T2) assertPrint(__FILE__, __LINE__, T1, T2, "==", [=](){ return fabs(T1 - T2) < 0.00001;});
#define assertNe(T1,T2) assertPrint(__FILE__, __LINE__, T1, T2, "!=", [=](){ return fabs(T1 - T2) >= 0.00001;});
#define assertLt(T1,T2) assertPrint(__FILE__, __LINE__, T1, T2, "<", [=](){ return T1 < T2;});
#define assertGt(T1,T2) assertPrint(__FILE__, __LINE__, T1, T2, ">", [=](){ return T1 > T2;});
#define assertLe(T1,T2) assertPrint(__FILE__, __LINE__, T1, T2, "<=", [=](){ return T1 <= T2;});
#define assertGe(T1,T2) assertPrint(__FILE__, __LINE__, T1, T2, ">=", [=](){ return T1 >= T2;});

void assertNormalized(Vector2D v) {
	assertLe(v.x,1.0);
	assertGe(v.x,-1.0);
	assertLe(v.y,1.0);
	assertGe(v.y,-1.0);
}

void testVectorToRadConversion(double rotation) {
	Vector2D v = dirFromRad(rotation);
	assertNormalized(v);
	assertEq(radFromDir(v),rotation);
}

Vector2D makeVector2D(double rotation) {
	Vector2D v = dirFromRad(rotation);
	testVectorToRadConversion(rotation);
	return v;
}

void testVectorRotation() {
	double r = 0;
	Vector2D right = makeVector2D(r);
	assertVeq(right,Vector2D(1,0));

	r = M_PI / 2;
	Vector2D up = makeVector2D(r);
	assertVeq(up,Vector2D(0,1));

	r = M_PI;
	Vector2D left = makeVector2D(r);
	assertVeq(left,Vector2D(-1,0));

	r = -M_PI;
	Vector2D down = makeVector2D(r);
	assertVeq(down,Vector2D(-1,0));

	assertEq(normRotation(0.0),0.0);
	assertEq(normRotation(-M_PI/2),-M_PI/2);
	assertEq(normRotation(M_PI / 2),M_PI / 2);
	assertEq(normRotation(-2 * M_PI),0.0);



	for(double i = -(2 * M_PI); i < 2 * M_PI; i+=(M_PI / 2)) {
		Vector2D v = makeVector2D(normRotation(i));
		if(fabs(v.x) < 0.00001)
			v.x = 0;

		if(fabs(v.y) < 0.00001)
			v.y = 0;
/*
		if(v.x != -1 && v.x != 0 && v.x != 1)
			std::cerr << i << " " << normRotation(i) << "    " << v.x << "\t" << v.y << std::endl;
		else
			std::cerr << i << " " << normRotation(i) << " \t" << v.x << "\t\t" << v.y << std::endl;*/
	}
}

void testCoordinateNormalization() {
	Vector2D v(200,600);
	Vector2D v1(600,200);
	Vector2D v2(100,700);
	Vector2D v3(700,100);

	v.normalize(800,800);
	v1.normalize(800,800);
	v2.normalize(800,800);
	v3.normalize(800,800);

	assertVeq(v,Vector2D(0.25,0.75));
	assertVeq(v1,Vector2D(0.75,0.25));
	assertVeq(v2,Vector2D(0.125,0.875));
	assertVeq(v3,Vector2D(0.875,0.125));

}

int main(int argc, char** argv) {
	testCoordinateNormalization();
	testVectorRotation();
	std::cerr << "Tests Successful" << std::endl;
	return 0;
}
