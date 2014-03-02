/*

 * 2D.hpp
 *
 *  Created on: Mar 1, 2014
 *      Author: elchaschab
 */

#ifndef TWD_HPP_
#define TWD_HPP_

#include <stdint.h>
#include <math.h>

namespace tankwar {
typedef double Coord;
typedef double Thrust;

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

}

#endif /* TWD_HPP_ */
