//---------------------------------------------------------------------------
#ifdef __BORLANDC__
	#pragma hdrstop
	#pragma package(smart_init)
#endif

#include "myMath.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//	TFLine
//
//---------------------------------------------------------------------------

TFLine& TFLine::reset(float x1, float y1, float x2, float y2)
{
	point1.reset(x1, y1);
    point2.reset(x2, y2);
    return *this;
}

TFLine& TFLine::reset(const Point3F p1, const Point3F p2)
{
	point1 = p1;
	point2 = p2;
    return *this;
}

float TFLine::check_distance(Point3F point)
{
	return std::fabs((point2.y-point1.y)*point.x - (point2.x-point1.x)*point.y +
					point2.x*point1.y - point2.y*point1.x) /
					sqrtf((point2.x-point1.x) * (point2.x-point1.x) + (point2.y-point1.y) * (point2.y-point1.y));
}

float TFLine::alt_check_going_through(const Point3F &in_point, const Vector3F &in_D)
{
	if(in_D.x == 0 && in_D.y == 0)
		return -1;
	Vector3F D(in_D);
	Point3F point02(point2.x - point1.x, point2.y - point1.y);
	Point3F point(in_point.x - point1.x, in_point.y - point1.y);
    //point.reset(point.x - point1.x, point.y - point1.y);

	if(point02.x < 0)
	{
    	point02.x = -point02.x;
    	point.x = -point.x;
    	D.x = -D.x;
	}
	if(point02.y < 0)
	{
    	point02.y = -point02.y;
    	point.y = -point.y;
    	D.y = -D.y;
	}
	float len = point02.get_len();
	if(point02.x == 0)
	{
		point.reset(point.y, point.x);
        D.reset(D.y, D.x);
	}
	else
	{
    	float cosb = point02.x / len, sinb = point02.y / len;
    	point.reset(+point.x * cosb + point.y * sinb, -point.x * sinb + point.y * cosb);
		D.reset(+D.x * cosb + D.y * sinb, -D.x * sinb + D.y * cosb);
	}
	if(D.y == 0 || sign(point.y) == sign(D.y))
		return -1;
    float xn = point.x - point.y * D.x / D.y;
	if(xn < 0 || xn > len)
		return -1;
    point.x -= xn;
	return (xn = point.get_len());
}

float TFLine::check_going_through(const Point3F &point, const Vector3F &D)
{
	if(D.x == 0 && D.y == 0)
		return -1;
	Point3F p3 = point + D * 10,
		r_p;
	float det = (point1.x - point2.x) * (p3.y - point.y) - (point1.y - point2.y) * (p3.x - point.x);
	if(!det)
		return -1;
	r_p.x = ((point1.x * point2.y - point1.y * point2.x) * (p3.x - point.x) -
			   (point1.x - point2.x) * (p3.x * point.y - p3.y * point.x)) / det;
	r_p.y = ((point1.x * point2.y - point1.y * point2.x) * (p3.y - point.y) -
			   (point1.y - point2.y) * (p3.x * point.y - p3.y * point.x)) / det;
	if((r_p.x > point1.x && r_p.x > point2.x) || (r_p.y > point1.y && r_p.y > point2.y) ||
		(r_p.x < point1.x && r_p.x < point2.x) || (r_p.y < point1.y && r_p.y < point2.y))
		return -1;
	if(std::fabs(D.x) > std::fabs(D.y))
	{
		if(sign(r_p.x - point.x) != sign(D.x))
			return -1;
	}
	else if(sign(r_p.y - point.y) != sign(D.y))
		return -1;
	return point.get_len(r_p);
}

void TFLine::get_reflect(float* reflect, Point3F vect)
{
    Point3F point02(point2.x - point1.x, point2.y - point1.y);
	if(sign(vect.y) != sign(point02.y))
		{ point02 *= -1; }
    double alpha = atan2(vect.y, vect.x), beta = atan2(point02.y, point02.x);
	float xy = vect.get_len(),
		theta = float(2 * beta - alpha);
	reflect[0] = cos(theta) * xy;
	reflect[1] = sin(theta) * xy;
}

float TFLine::get_position(float* point)
{
	if(point1.y > point[1] && point2.y > point[1])
		if((point1.x > point[0] && point2.x > point[0]) || (point1.x < point[0] && point2.x < point[0]))
			return 0;
    return std::min(point2.get_len(point[0],point[1]), point2.get_len(point1));
}

bool TFLine::operator==(const TFLine &line)
{
	//return ( (point1 != line.point1) ? (point2 == line.point1 && point1 == line.point2) : (point2 == line.point2) );
	if (point1 == line.point1 && point2 == line.point2)
		return true;
	else if (point2 == line.point1 && point1 == line.point2)
		return true;
	else return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//	TFPlane
//
//---------------------------------------------------------------------------

TFPlane& TFPlane::GetPlaneByPoints(Point3F p1, Point3F p2, Point3F p3, float d1)
{
	if (p1 == p2 || p2 == p3 || p3 == p1)
		return *this = WRONG_PLANE;
	p2 = p2 - p1;
	p3 = p3 - p1;
	A = p2.y * p3.z - p3.y * p2.z;
	B = p3.x * p2.z - p2.x * p3.z;
	C = p2.x * p3.y - p3.x * p2.y;
	D = -(A * p1.x + B * p1.y + C * p1.z);

	return *this;
}

void TFPlane::GetNormal(float *norm)
{
	float sum = sqrt(A * A + B * B + C * C);
	norm[0] = A / sum;
	norm[1] = B / sum;
	norm[2] = C / sum;
}


//---------------------------------------------------------------------------
//
//	TFTriangle
//
//---------------------------------------------------------------------------

TFTriangle::TFTriangle(Point3F pt1, Point3F pt2, Point3F pt3): point1(pt1), point2(pt2), point3(pt3)
{
	plane.GetPlaneByPoints(point1, point2, point3);
}

TFTriangle& TFTriangle::reset(Point3F pt1, Point3F pt2, Point3F pt3)
{
	point1 = pt1;
	point2 = pt2;
	point3 = pt3;
	plane.GetPlaneByPoints(point1, point2, point3);
	return *this;
}

TFTriangle& TFTriangle::reset(float *pt1, float *pt2, float *pt3)
{
	reset(*(Point3F *)pt1, *(Point3F *)pt2, *(Point3F *)pt3);
	return *this;
}

float TFTriangle::CheckPointConnected(Point3F point0)
{
	return point0.GetTriangleArea(point2, point3) + point1.GetTriangleArea(point0, point3) +
			point1.GetTriangleArea(point2, point0) - point1.GetTriangleArea(point2, point3);
}

void TFTriangle::GetReflectVector(Vector3F in_vect, Vector3F *out_vect)
{
	Vector3F norm_vect;
	plane.GetNormal(&norm_vect);
	float cos_tetta;
	cos_tetta =	plane.A * in_vect.x + plane.B * in_vect.y + plane.C * in_vect.z;
	cos_tetta = -cos_tetta / sqrt((plane.A * plane.A + plane.B * plane.B + plane.C * plane.C) *
			(in_vect.x * in_vect.x + in_vect.y * in_vect.y + in_vect.z * in_vect.z));
	norm_vect *= 2 * cos_tetta * in_vect.get_len();
	*out_vect = norm_vect + in_vect;
}

float TFTriangle::GetPlaneConnectionPoint(Point3F pt0, Vector3F vect0, Point3F *out_pt)
{
	float t = plane.A * vect0.x + plane.B * vect0.y + plane.C * vect0.z;
	if (!t)
	{
		out_pt->reset();
		return -1;
	}
	t = -(plane.A * pt0.x + plane.B * pt0.y + plane.C * pt0.z + plane.D) / t;

	*out_pt = vect0 * t + pt0;
	return t;
}

float TFTriangle::GetTriangleConnectionPoint(Point3F vect_pt, Vector3F vect0, Point3F *out_pt)
{
	float f = GetPlaneConnectionPoint(vect_pt, vect0, out_pt);
	return ((f < 0 || CheckPointConnected(*out_pt) > 1) ? -1 : f);
}
//---------------------------------------------------------------------------

