#include "../src/2d.hpp"
#include <iostream>
#include <string>
#include <functional>

using namespace neurocid;
using std::cerr;
using std::endl;
using std::string;
using std::function;

#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <cxxabi.h>

template<typename T> void assertPrint(string file, size_t line, T t1, T t2, string strOp, function<bool()> op) {
	if(!op()) {
		cerr << "failed " << file << ':' << line << " " << t1 << strOp << t2 << endl;
		exit(1);
	}
}

#define assertVeq(T1,T2) assertPrint(__FILE__, __LINE__, T1, T2, "==", [=](){ return fabs(T1.x_ - T2.x_) < 0.00001 && fabs(T1.y_ - T2.y_) < 0.00001;});
#define assertVne(T1,T2) assertPrint(__FILE__, __LINE__, T1, T2, "!=", [=](){ return fabs(T1.x_ - T2.x_) >= 0.00001 && fabs(T1.y_ - T2.y_) >= 0.00001;});
#define assertEq(T1,T2) assertPrint(__FILE__, __LINE__, T1, T2, "==", [=](){ return fabs(T1 - T2) < 0.00001;});
#define assertNe(T1,T2) assertPrint(__FILE__, __LINE__, T1, T2, "!=", [=](){ return fabs(T1 - T2) >= 0.00001;});
#define assertLt(T1,T2) assertPrint(__FILE__, __LINE__, T1, T2, "<", [=](){ return T1 < T2;});
#define assertGt(T1,T2) assertPrint(__FILE__, __LINE__, T1, T2, ">", [=](){ return T1 > T2;});
#define assertLe(T1,T2) assertPrint(__FILE__, __LINE__, T1, T2, "<=", [=](){ return T1 <= T2;});
#define assertGe(T1,T2) assertPrint(__FILE__, __LINE__, T1, T2, ">=", [=](){ return T1 >= T2;});

void assertNormalized(Vector2D v) {
	assertLe(v.x_,1.0);
	assertGe(v.x_,-1.0);
	assertLe(v.y_,1.0);
	assertGe(v.y_,-1.0);
}

void testVectorToRadConversion(Coord rotation) {
	Vector2D v = dirFromRad(rotation);
	assertNormalized(v);
	assertEq(radFromDir(v),rotation);
}

Vector2D makeVector2D(Coord rotation) {
	Vector2D v = dirFromRad(rotation);
	testVectorToRadConversion(rotation);
	return v;
}

void testVectorRotation() {
	Coord r = 0;
	Vector2D down = makeVector2D(r);
	assertVeq(down,Vector2D(0,-1));

	r = M_PI / 2;
	Vector2D right = makeVector2D(r);
	assertVeq(right,Vector2D(1,0));

	r = M_PI;
	Vector2D up = makeVector2D(r);
	assertVeq(up,Vector2D(0,1));

	r = -M_PI / 2;
	Vector2D left = makeVector2D(r);
	assertVeq(left,Vector2D(-1,0));

	assertEq(normRotation(0.0),0.0);
	assertEq(normRotation(-M_PI/2),-M_PI/2);
	assertEq(normRotation(M_PI / 2),M_PI / 2);
	assertEq(normRotation(-2 * M_PI),0.0);

	for(Coord i = -(2 * M_PI); i < 2 * M_PI; i+=(M_PI / 2)) {
		Vector2D v = makeVector2D(normRotation(i));
		if(fabs(v.x_) < 0.00001)
			v.x_ = 0;

		if(fabs(v.y_) < 0.00001)
			v.y_ = 0;
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
