//---------------------------------------------------------------------------
#ifndef RailFigureH
#define RailFigureH
//---------------------------------------------------------------------------

#include "myMath.h"
#include <vector>
//---------------------------------------------------------------------------

//#define RAIL_PARTS		9
//#define RAIL_GRAPH_PARTS	25
#define MAX_PROFILE_ARR		6000

#define VIS_POINTS			0
#define VIS_LINES			1
#define VIS_TRIANGLES		2

#define RAIL_3D_RT			1
#define RAIL_2D_RT			2
#define RAIL_DISSESION_RT	4
#define RAIL_PROFILE_RT		8

#define NO_RAIL				0x0
#define LEFT_RAIL			0x1
#define RIGHT_RAIL			0x2
#define BOTH_RAILS			0x3

#define RT_NONE	0
#define RT_R50	50
#define RT_R65	65
#define RT_R75	75
//---------------------------------------------------------------------------

class RailFigure
{
protected:
	int type;


	void ElvVariables();
	bool LineByNumber(int number, TFLine &out_line);


public:
	std::vector<Point2F> points,
		graph_points;
	std::vector<float> lengths,
		graph_lengths,
		sum_lengths;


	RailFigure();
	int GetType();
	void Inicialize(int in_type);
	void InitR50();
	void InitR65();
	void InitR75();

	bool IsInside(const Point3F &point);
	float CheckPointConnected(const Point3F &point, TFLine &result);
	float CheckVectorLength(const TFLine &connected_line,
							const Point3F &point, const Vector3F &Nvector, TFLine &end_line);
	void GetReflectionVector(const TFLine &line, const Point3F &point, const Vector3F &in_vector, Vector3F &result);
};
//---------------------------------------------------------------------------

#endif


