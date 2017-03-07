//---------------------------------------------------------------------------
#ifndef myMathH
#define myMathH
//---------------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
//---------------------------------------------------------------------------

//#define MAX(a, b)		(((a) > (b)) ? (a) : (b))
//#define MIN(a, b)		(((a) < (b)) ? (a) : (b))
#define sign(n)			(((n) == 0) ? 0 : (((n) > 0) ? 1 : -1 ))
#define my_abs(a)		(((a) < 0) ? -(a) : (a))
#define max_abs(a, b)	((my_abs(a) > my_abs(b)) ? (a) : (b))
#define min_abs(a, b)	((my_abs(a) < my_abs(b)) ? (a) : (b))
//---------------------------------------------------------------------------

typedef unsigned int UINT;

class TFLine;
class TFPlane;
class TFTriangle;

template<class NumberT> class Point2_T;
template<class NumberT> class Point3_T;
//---------------------------------------------------------------------------

template<class NumberT> class Point2_T {
public:
	NumberT x,
		y;


	Point2_T(NumberT x1 = 0, NumberT y1 = 0)
				: x(x1), y(y1) {/*pos = &x;*/}
	Point2_T(NumberT *pos)
				: x(pos[0]), y(pos[1]) {/*pos = &x;*/}
	bool  Exist() const { return (x || y); }
	float  angle(const Point2_T &v) const
	{
		float a = x * v.x + y * v.y;
		return (a / (get_len() * v.get_len()));
	}
	float  get_len() const
	{
		return sqrtf(x * x + y * y);
	}
	float  get_len(const Point2_T &point) const
	{
		return get_len(point.x, point.y);
	}
	float  get_len(NumberT x1, NumberT y1 = 0) const
    {
		x1 -= x; y1 -= y;
        return sqrtf(x1 * x1 + y1 * y1);
	}
	inline Point2_T&  reset_arr(NumberT *a)
	{
		x = a[0];
		y = a[1];
		return *this;
	}
	inline Point2_T&  reset(NumberT x1 = 0, NumberT y1 = 0)
    {
		x = x1;
		y = y1;
        return *this;
	}
	float  max_part() const { return max_abs(x, y); }

	Point2_T&  norm()
	{
		float sum = sqrt(x * x + y * y);
		if(sum)
			reset(x / sum, y / sum);
		return *this;
	}
	Point2_T& getNormal(Point2_T<NumberT> &Ndir_vect) const
	{
		float sum = sqrtf(x * x + y * y);
		if(sum == 0) return (Ndir_vect = 0.0f);
		else return Ndir_vect.reset(x / sum, y / sum);
	}
	Point2_T<NumberT> getNormal() const
	{
		Point2_T<NumberT> Ndir_vect;
		return getNormal(Ndir_vect);
	}
    float GetTriangleArea(Point2_T &pt1, Point2_T &pt2) const
	{
		float a = pt1.get_len(pt1),
			b = pt2.get_len(pt2),
			c = get_len(),
			p = (a + b + c) / 2;
		return sqrtf(std::fabs(p * (p - a) * (p - b) * (p - c)));
	}


	Point2_T operator +(const Point2_T &p0) const { return Point2_T<NumberT>(x + p0.x, y + p0.y); }
	Point2_T& operator +=(const Point2_T &p0) { return reset(x + p0.x, y + p0.y); }
	Point2_T operator -(const Point2_T &p0) const { return Point2_T<NumberT>(x - p0.x, y - p0.y); }
	Point2_T operator -() const { return Point2_T(-x, -y); }
	Point2_T& operator -=(const Point2_T &p0) const { return reset(x - p0.x, y - p0.y); }
	Point2_T operator *(NumberT numb) const { return Point2_T<NumberT>(x * numb, y * numb); }
	Point2_T& operator *=(NumberT numb) { return reset(x * numb, y * numb); }
	Point2_T operator /(NumberT numb) const { return Point2_T<NumberT>(x / numb, y / numb); }
	Point2_T& operator /=(NumberT numb) { return reset(x / numb, y / numb); }

	bool operator ==(const Point2_T &p0) const { return (p0.x == x && p0.y == y); }
	bool operator !=(const Point2_T &p0) const { return (p0.x != x || p0.y != y); }
	bool operator !() const { return !(x || y); }
	operator Point3_T<NumberT>() const { return Point3_T<NumberT>(x, y); }

};
//---------------------------------------------------------------------------

typedef Point2_T<float> Point2F;
typedef Point2F Vector2F;
//---------------------------------------------------------------------------

template<class NumberT> class Point3_T {
public:
	NumberT x,
		y,
		z;


	Point3_T(NumberT x1 = 0, NumberT y1 = 0, NumberT z1 = 0)
				: x(x1), y(y1), z(z1) {/*pos = &x;*/}
	Point3_T(NumberT *pos)
				: x(pos[0]), y(pos[1]), z(pos[2]) {/*pos = &x;*/}
    bool  Exist() const { return (x || y || z); }
	float  angle(const Point3_T &v) const
	{
		float a = x * v.x + y * v.y + z * v.z;
		return (a / (get_len() * v.get_len()));
	}
	float  get_len() const { return sqrtf(x * x + y * y + z * z); }
	float  get_len(const Point3_T &point) const
	{
		return get_len(point.x, point.y, point.z);
	}
	float  get_len(NumberT x1, NumberT y1 = 0, NumberT z1 = 0) const
    {
		x1 -= x; y1 -= y; z1 -= z;
        return sqrtf(x1 * x1 + y1 * y1 + z1 * z1);
	}
	Point3_T&  reset_arr(NumberT *a) { return reset(a[0], a[1], a[2]); }
	Point3_T&  reset(NumberT x1 = 0, NumberT y1 = 0, NumberT z1 = 0)
    {
		x = x1;
		y = y1;
		z = z1;
        return *this;
	}
	float  max_part() const { return max_abs(x, max_abs(y, z)); }
	Point3_T&  norm()
	{
		float sum = sqrt(x * x + y * y + z * z);
		if(sum)
			reset(x / sum, y / sum, z / sum);
		return *this;
	}
	Point3_T& getNormal(Point3_T<NumberT> &Ndir_vect) const
	{
		float sum = sqrtf(x * x + y * y + z * z);
		if(sum == 0) return (Ndir_vect = 0.0f);
		else return Ndir_vect.reset(x / sum, y / sum, z / sum);
	}
	Point3_T<NumberT> getNormal() const
	{
		Point3_T<NumberT> Ndir_vect;
		return getNormal(Ndir_vect);
	}
	float GetTriangleArea(Point3_T &pt1, Point3_T &pt2) const
	{
		float a = pt1.get_len(pt1),
			b = pt2.get_len(pt2),
			c = get_len(),
			p = (a + b + c) / 2;
		return sqrtf(std::fabs(p * (p - a) * (p - b) * (p - c)));
	}

	Point3_T operator +(const Point3_T &p0) const { return Point3_T<NumberT>(x + p0.x, y + p0.y, z + p0.z); }
	Point3_T& operator +=(const Point3_T &p0) { return reset(x + p0.x, y + p0.y, z + p0.z); }
	Point3_T operator -(const Point3_T &p0) const { return Point3_T<NumberT>(x - p0.x, y - p0.y, z - p0.z); }
	Point3_T operator -() const { return Point3_T(-x, -y, -z); }
	Point3_T& operator -=(const Point3_T &p0) { return reset(x - p0.x, y - p0.y, z - p0.z); }
	Point3_T operator *(NumberT numb) const { return Point3_T<NumberT>(x * numb, y * numb, z * numb); }
	Point3_T& operator *=(NumberT numb) { return reset(x * numb, y * numb, z * numb); }
	Point3_T operator /(NumberT numb) const { return Point3_T<NumberT>(x / numb, y / numb, z / numb); }
	Point3_T& operator /=(NumberT numb) { return reset(x / numb, y / numb, z / numb); }

	bool operator ==(const Point3_T &p0) const { return (p0.x == x && p0.y == y && p0.z == z); }
	bool operator !=(const Point3_T &p0) const { return (p0.x != x || p0.y != y || p0.z != z); }
	bool operator !() const { return !(x || y || z); }
	operator Point2_T<NumberT>() const { return Point2_T<NumberT>(x, y); }
};
//---------------------------------------------------------------------------

typedef Point3_T<float> Point3F;
typedef Point3F Vector3F;
//---------------------------------------------------------------------------

class TFLine
{
public:
	Point3F point1, point2;


    TFLine(float x1 = 0, float y1 = 0, float x2 = 0, float y2 = 0): point1(x1, y1, 0), point2(x2, y2, 0) {}
    TFLine(Point3F p1, Point3F p2): point1(p1), point2(p2) {}

	TFLine&  reset(float x1 = 0, float y1 = 0, float x2 = 0, float y2 = 0);
    TFLine&  reset(const Point3F p1, const Point3F p2);
	float  GetLength() { return point1.get_len(point2); }

	float  check_distance(Point3F point);
	float  alt_check_going_through(const Point3F &point, const Vector3F &D);
	float  check_going_through(const Point3F &point, const Vector3F &D);
	void  get_reflect(float* reflect, Point3F point);
	float  get_position(float* point);

	bool operator==(const TFLine &line);
};
//---------------------------------------------------------------------------

class TFPlane
{
public:
	float A, B, C, D;

	TFPlane(float a1 = 0, float b1 = 0, float c1 = 0, float d1 = 0): A(a1), B(b1), C(c1), D(d1) {}
	TFPlane(float *arr) { A = arr[0]; B = arr[1]; C = arr[2]; D = arr[3]; }
	bool  Exist() { return (A || B || C); }
	TFPlane&  GetPlaneByPoints(Point3F p1, Point3F p2, Point3F p3, float d1 = 1);
	void  GetNormal(float *norm);
	void  GetNormal(Vector3F *v0) { GetNormal((float*)v0); }
};
//---------------------------------------------------------------------------

class TFTriangle
{
public:
	Point3F point1, point2, point3;
	TFPlane plane;


	TFTriangle() {}
	TFTriangle(Point3F pt1, Point3F pt2, Point3F pt3);
	TFTriangle&  reset(Point3F pt1, Point3F pt2, Point3F pt3);
	TFTriangle&  reset(float *pt1, float *pt2, float *pt3);
	float  CheckPointConnected(Point3F point0);
	void  GetReflectVector(Vector3F in_vect, Vector3F *out_vect);
	float  GetPlaneConnectionPoint(Point3F pt0, Vector3F vect0, Point3F *out_pt);
	float  GetTriangleConnectionPoint(Point3F vect_pt, Vector3F vect0, Point3F *out_pt);
};
//---------------------------------------------------------------------------

#define WRONG_PLANE TFPlane()
//---------------------------------------------------------------------------

#endif


