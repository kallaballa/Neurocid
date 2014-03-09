/*

 * 2D.hpp
 *
 *  Created on: Mar 1, 2014
 *      Author: elchaschab
 */

#ifndef TWD_HPP_
#define TWD_HPP_

#define _USE_MATH_DEFINES
#include <stdint.h>
#include <math.h>
#include <iostream>
#include <cassert>

namespace tankwar {
typedef double Coord;

struct Vector2D {
	Coord x, y;

	Vector2D(Coord a = 0.0f, Coord b = 0.0f) :
			x(a), y(b) {
	}

	//we need some overloaded operators
	Vector2D &operator+=(const Vector2D &rhs) {
		x += rhs.x;
		y += rhs.y;

		return *this;
	}

	Vector2D &operator-=(const Vector2D &rhs) {
		x -= rhs.x;
		y -= rhs.y;

		return *this;
	}

	Vector2D &operator*=(const Coord &rhs) {
		x *= rhs;
		y *= rhs;

		return *this;
	}

	Vector2D &operator/=(const Coord &rhs) {
		x /= rhs;
		y /= rhs;

		return *this;
	}

	bool operator==(const Vector2D &other) const {
		return this->x == other.x && this->y == other.y;
	}

	bool operator!=(const Vector2D &other) const {
		return !operator==(other);
	}

	inline double length() {
		return sqrt(this->x * this->x + this->y * this->y);
	}

	Vector2D& normalize() {
		double vector_length = this->length();
		if(vector_length == 0)
		{
			this->x = 0;
			this->y = 0;
			return *this;
		}
		this->x = this->x / vector_length;
		this->y = this->y / vector_length;
		return *this;
	}

	Vector2D& rotate(int degrees) {
		double radians = degrees * (M_PI / 180);
		double cs = cos(radians);
		double sn = sin(radians);
		Coord x1 = this->x * cs - this->y * sn;
		this->y = this->x * sn + this->y * cs;
		this->x = x1;
		return *this;
	}
};

inline double dot(Vector2D &v1, Vector2D &v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

inline int sign(Vector2D &v1, Vector2D &v2) {
	if (v1.y * v2.x > v1.x * v2.y) {
		return 1;
	} else {
		return -1;
	}
}
//overload the * operator
inline Vector2D operator*(const Vector2D &lhs, Coord rhs) {
	Vector2D result(lhs);
	result *= rhs;
	return result;
}

inline Vector2D operator*(Coord lhs, const Vector2D &rhs) {
	Vector2D result(rhs);
	result *= lhs;
	return result;
}

//overload the - operator
inline Vector2D operator-(const Vector2D &lhs, const Vector2D &rhs) {
	Vector2D result(lhs);
	result.x -= rhs.x;
	result.y -= rhs.y;

	return result;
}

inline Vector2D directionFromRotation(double rad) {
	assert(rad <= M_PI);
	assert(rad >= -M_PI);

	return Vector2D(cos(rad), sin(rad));
}

inline double rotationFromDirection(Vector2D dir) {
	double r = atan2(dir.y, dir.x);
	assert(r <= M_PI);
	assert(r >= -M_PI);
	return r;
}

inline double normRotation(double rotation) {
    //2 * pi = 0

	// -pi .. +pi

	//0 -> -pi
	if(rotation >= 0)
		return fmod(rotation + M_PI, 2 * M_PI) - M_PI;
	else
		return (fmod((-rotation) + M_PI, 2 * M_PI) * -1) + M_PI;
}

inline std::ostream& operator<<(std::ostream& os, Vector2D v) {
	os << '(' << v.x << ',' << v.y << ')';
	return os;
}
}

#endif /* TWD_HPP_ */
