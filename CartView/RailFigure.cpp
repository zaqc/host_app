//---------------------------------------------------------------------------
#ifdef __BORLANDC__
	#pragma hdrstop
	#pragma package(smart_init)
#endif

#include "RailFigure.h"
//---------------------------------------------------------------------------

typedef unsigned int UINT;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//  RailFigure
//
//---------------------------------------------------------------------------

RailFigure::RailFigure()
			: type(RT_NONE)
{
	//
}

int RailFigure::GetType()
{
	return type;
}

void RailFigure::Inicialize(int in_type)
{
	if(in_type == RT_R65) InitR65();
	else if(in_type == RT_R75) InitR75();
	else if(in_type == RT_R50) InitR50();
}

void RailFigure::InitR50()
{
	type = RT_R50;
	float widthR50[] = { 	66, 66,
			24.4f, 20.3f, 14.6f, 10.9f, 9.3f,
			8,
			10.1f, 10.9f, 13.8f, 16.6f,
			36, 34.6f, 31.8f, 25.6f, 12.5f };
	float heightR50[] = { -76, -65.5f,
			-55.1f, -52.6f, -47.7f, -40.1f, -29.4f,
			-7.6f,
			19.6f, 31.3f, 36, 38.2f,
			43, 60.8f, 68.8f, 73.4f, 76 };
	points.resize(sizeof(widthR50) / sizeof(float));
	for(UINT i = 0; i < points.size(); ++i)
	{
		points[i].x = widthR50[i];
		points[i].y = heightR50[i];
	}
	graph_points.resize(sizeof(widthR50) / sizeof(float));
	for(UINT i = 0; i < points.size(); ++i)
	{
		graph_points[i].x = widthR50[i];
		graph_points[i].y = heightR50[i];
	}

	ElvVariables();
}

void RailFigure::InitR65()
{
	type = RT_R65;
	//float simple_widthR65[] =	{ 75, 75, 29, 11, 11, 33.0f, 37.5f, 36, 24 };
	//float simple_heightR65[] = { -90, -78.8f, -67, -45, 41.0f, 57.5f, 58.2f, 78, 90 };
	float widthR65[] = { 		75, 75,
			29.6f, 25, 21, 17, 14, 12, 10.8f,
			9,
			12.0f, 13, 15, 17, 19.6f,
			37.5f, 36.5f,
			36, 35, 32.5f, 31, 27.5f, 24.55f, 20.2f, 10 };
	float heightR65[] = { 	-95, -83.8f,
			-72.4f, -70, -67.5f, -64, -60, -55, -50,
			5,
			39.2f, 43, 46, 48, 50,
			54.4f, 74.3f,
			77, 80, 83.5f, 85, 87, 88.3f, 89.6f, 90 };
	points.resize(sizeof(widthR65) / sizeof(float));
	for(UINT i = 0; i < points.size(); ++i)
	{
		points[i].x = widthR65[i];
		points[i].y = heightR65[i];
	}
	graph_points.resize(sizeof(widthR65) / sizeof(float));
	for(UINT i = 0; i < points.size(); ++i)
	{
		graph_points[i].x = widthR65[i];
		graph_points[i].y = heightR65[i];
	}

	ElvVariables();
}

void RailFigure::InitR75()
{
	type = RT_R75;
	float widthR75[] = { 	75, 75,
			31.7f, 21.3f, 15.4f, 11.9f, 11.2f,
			10.0f,
			11.4f, 13.2f, 17.1f,
			35.8f, 37, 37.5f, 35.3f, 32.3f,
			28.6f, 23.5f, 9.5f };
	float heightR75[] = { 	-96, -82,
			-70.9f, -66.1f, -58.9f, -49, -40.5f,
			-10.8f,
			26.5f, 39.3f, 44.8f,
			50.4f, 51.9f, 54.2f, 80.6f, 88.5f,
			92.1f, 94.2f, 96 };
	points.resize(sizeof(widthR75) / sizeof(float));
	for(UINT i = 0; i < points.size(); ++i)
	{
		points[i].x = widthR75[i];
		points[i].y = heightR75[i];
	}
	graph_points.resize(sizeof(widthR75) / sizeof(float));
	for(UINT i = 0; i < points.size(); ++i)
	{
		graph_points[i].x = widthR75[i];
		graph_points[i].y = heightR75[i];
	}

	ElvVariables();
}

void RailFigure::ElvVariables()
{
    lengths.resize(points.size() - 1);
	for(UINT i = 0; i < lengths.size(); ++i)
		lengths[i] = points[i + 1].get_len(points[i]);
	graph_lengths.resize(graph_points.size() - 1);
	for(UINT i = 0; i < graph_lengths.size(); ++i)
		graph_lengths[i] = graph_points[i + 1].get_len(graph_points[i]);

	sum_lengths.resize(points.size());
	int i = points.size() - 1;
	sum_lengths[i] = -points[i].x;
	--i;
	sum_lengths[i] = points[i + 1].x / 2;
	while(--i >= 0) sum_lengths[i] = sum_lengths[i + 1] + lengths[i + 1];
}

bool RailFigure::LineByNumber(int number, TFLine &out_line)
{
	if(!points.size() || number < 0)
		return false;
	if(number == 0)
		out_line = TFLine(-points[0].x, points[0].y, points[0].x, points[0].y);
	else if(number < int(points.size() * 2 - 1))
	{
		int integ = (number - 1) / 2,
			mult = ((number % 2 == 0) ? -1 : 1);
		out_line = TFLine(mult * points[integ].x, points[integ].y, mult * points[integ + 1].x, points[integ + 1].y);
	}
	else if(number == int(points.size() * 2 - 1))
		out_line = TFLine(-points[points.size() - 1].x, points[points.size() - 1].y,
							points[points.size() - 1].x, points[points.size() - 1].y);
	else return false;
	return true;
}

bool RailFigure::IsInside(const Point3F &point)
{
	int counter = 0;
	Point2F *p = &points[0];
	for(int i = points.size() - 2; i >= 0; --i)
		if((point.y <= p[i].y && point.y >= p[i + 1].y) || (point.y >= p[i].y && point.y <= p[i + 1].y))
		{
			float x = (point.y - p[i].y) / (p[i + 1].y - p[i].y) * (p[i + 1].x - p[i].x) + p[i].x + 0.05f;
			if(point.x < x && point.x > -x)
				++counter;
		}
	return (counter % 2 != 0);
}

float RailFigure::CheckPointConnected(const Point3F &point, TFLine &result)
{
	float distance = -1,
		l_dist;

	Point3F myPoint(point.x, point.y);
	TFLine myLine;
	//TFTriangle myTriangle;
	for(int i = 0; LineByNumber(i, myLine); ++i)
	{
		//point1 = myLine.point1; point1.z = 1;
		//myTriangle.reset(myLine.point1, myLine.point2, point1);
		//dist1 = myTriangle.CheckPointConnected(myPoint);
        l_dist = myLine.check_distance(myPoint);
		if(distance < 0 || distance > l_dist)
        {
            distance = l_dist;
			result = myLine;
		}
		if(l_dist < 0.01f)
			return l_dist;
    }
	return ( (distance >= 10) ? -1 : distance );
}

float RailFigure::CheckVectorLength(const TFLine &connected_line,
			const Point3F &point, const Vector3F &Nvector, TFLine &end_line)
{
	if(Nvector.x == 0 && Nvector.y == 0)
		return -1;
    float result1, result = -1;
	Point3F myPoint(point.x, point.y);
	Vector3F vect(Nvector.x, Nvector.y);
	TFLine myLine;
	for(int i = 0; LineByNumber(i, myLine); ++i)
    {
		if(myLine == connected_line)
			continue;
        result1 = myLine.check_going_through(myPoint, vect);
		if(result1 > 0.01f && (result == -1 || result1 < result))
		{
			result = result1;
			end_line = myLine;
		}
    }
	return ( (result != -1) ? result : 0 );
}

void RailFigure::GetReflectionVector(const TFLine &line,
			const Point3F &point, const Vector3F &in_vector, Vector3F &result)
{
	Vector3F vect(in_vector.x, in_vector.y);
	Point3F point1 = line.point1;
	point1.z = 1;
	TFTriangle triangle0(line.point1, line.point2, point1);
	triangle0.GetReflectVector(vect, &result);
	//cur_line.get_reflect(result, vect);
    result.z = in_vector.z;
}
//---------------------------------------------------------------------------


