#ifndef TWD_HPP_
#define TWD_HPP_

#define _USE_MATH_DEFINES
#include "error.hpp"
#include <stdint.h>
#include <math.h>
#include <iostream>
#include <limits>
#ifndef _NO_SERIALIZE
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#endif

namespace neurocid {
typedef double Coord;

#define ASSERT_LOC(V) CHECK(V.x_ != NO_COORD && V.y_ != NO_COORD);
#define ASSERT_DIR(V) CHECK(V.x_ >= -1 && V.x_ <= 1 && V.y_ >= -1 && V.y_ <= 1);

struct Vector2D {
#ifndef _NO_SERIALIZE
	friend class boost::serialization::access;
#endif

	Coord x_, y_;

	Vector2D(Coord a = 0.0f, Coord b = 0.0f) :
			x_(a), y_(b) {
	}

	inline Coord distance(const Vector2D& loc) const {
		return hypot(x_ - loc.x_, y_ - loc.y_);
	}

	//we need some overloaded operators
	Vector2D &operator+=(const Vector2D &rhs) {
		x_ += rhs.x_;
		y_ += rhs.y_;

		return *this;
	}

	Vector2D &operator-=(const Vector2D &rhs) {
		x_ -= rhs.x_;
		y_ -= rhs.y_;

		return *this;
	}

	Vector2D &operator*=(const Coord &rhs) {
		x_ *= rhs;
		y_ *= rhs;

		return *this;
	}

	Vector2D &operator/=(const Coord &rhs) {
		x_ /= rhs;
		y_ /= rhs;

		return *this;
	}

	bool operator==(const Vector2D &other) const {
		return this->x_ == other.x_ && this->y_ == other.y_;
	}

	bool operator!=(const Vector2D &other) const {
		return !operator==(other);
	}

	inline Coord length() {
		return sqrt(this->x_ * this->x_ + this->y_ * this->y_);
	}

	Vector2D& normalize(Coord w, Coord h) {
 		CHECK(w > 0);
		CHECK(h > 0);

		Coord ox = this->x_;
		Coord oy = this->y_;

		this->x_ = (this->x_) / (w);
		this->y_ = (this->y_) / (h);

		if(!(this->x_ >= -1 && this->x_ <= 1 && this->y_ >= -1 && this->y_ <= 1))
			std::cerr << "normalize error: " << "(" << ox << "," << oy << ") -> " << "(" << this->x_ << "," << this->y_ << ")" << std::endl;
		CHECK(this->x_ >= -1 && this->x_ <= 1);
		CHECK(this->y_ >= -1 && this->y_ <= 1);

		return *this;
	}

	Vector2D& normalize() {
		Coord vector_length = this->length();
		if(vector_length == 0)
		{
			this->x_ = 0;
			this->y_ = 0;
			return *this;
		}
		this->x_ = this->x_ / vector_length;
		this->y_ = this->y_ / vector_length;
		return *this;
	}

	void rotate(int degrees) {
		Coord radians = degrees * (M_PI / 180);
		Coord cs = cos(radians);
		Coord sn = sin(radians);
		Coord x1 = this->x_ * cs - this->y_ * sn;
		this->y_ = this->x_ * sn + this->y_ * cs;
		this->x_ = x1;
	}

	void rotate(Vector2D by) {
		ASSERT_DIR((*this));
		ASSERT_DIR(by);
		Coord x1 = (by.y_ * -x_ + -by.x_ * -y_);
		Coord y1 = (by.y_ * -y_ + by.x_ * -x_);

		x_ = x1;
		y_ = y1;
	}

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
    //suppress warning about unused variable version
    assert(version >= 0);
    ar & x_;
	  ar & y_;
	}
#endif
};

struct Rect {
	Vector2D ul_;
	Vector2D lr_;
};

inline Coord dot(Vector2D &v1, Vector2D &v2) {
	return v1.x_ * v2.x_ + v1.y_ * v2.y_;
}

inline int sign(Vector2D &v1, Vector2D &v2) {
	if (v1.y_ * v2.x_ > v1.x_ * v2.y_) {
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
	result.x_ -= rhs.x_;
	result.y_ -= rhs.y_;

	return result;
}

inline Vector2D dirFromRad(const Coord rad) {
	CHECK(rad <= M_PI);
	CHECK(rad >= -M_PI);

	return Vector2D(sin(rad), -cos(rad));
}

inline Vector2D dirFromDeg(const Coord degrees) {
	return dirFromRad(degrees * (M_PI / 180));
}

inline Coord radFromDir(const Vector2D dir) {
	Coord r = atan2(dir.x_, -dir.y_);
	CHECK(r <= M_PI);
	CHECK(r >= -M_PI);
	return r;
}

inline Coord normRotation(const Coord rotation) {
	if(rotation >= 0)
		return fmod(rotation + M_PI, 2 * M_PI) - M_PI;
	else
		return (fmod((-rotation) + M_PI, 2 * M_PI) * -1) + M_PI;
}

inline std::ostream& operator<<(std::ostream& os, Vector2D v) {
	os << '(' << v.x_ << ',' << v.y_ << ')';
	return os;
}

#define NO_COORD std::numeric_limits<Coord>().max()
#define NO_VECTOR2D Vector2D(NO_COORD, NO_COORD)

}

#endif /* TWD_HPP_ */
