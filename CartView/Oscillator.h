//---------------------------------------------------------------------------
#ifndef OscillatorH
#define OscillatorH
//---------------------------------------------------------------------------

#include "RailFigure.h"
//---------------------------------------------------------------------------

#define REFLECT_COUNT 16
#define PROECTIONS 3

typedef unsigned int UINT;
//---------------------------------------------------------------------------

class TOscillator
{
protected:
	float total_length, step;
	RailFigure *rail;


	void  get_work_length(int vect_numb);


public:
	Point3F oscill_point[REFLECT_COUNT + 1];
	Vector3F Vectors[REFLECT_COUNT];
	int reflect_count, begin_work_reflect;
	float control_zone_begin, control_zone_end, VRC_zone, PhyDelay,
		tick_time, speed;
	std::vector<Point3F> points;

	static int std_points_on_line;


	TOscillator(RailFigure *in_rail);

	// ���������� �������� �� ������ ������� ������
	//		���� ����� � ��������� � ��������, oscill_point1 - �������� ����� ������
	void  reset(float oscill_point1, int face_type, float alpha, float beta);

	// ���������� �������� ������������ �������
	// 		oscill_point1 � income_vector - ��������� �������
	void  reset(float* oscill_point1, float* income_vector);

	// ��������������� ��������� � ��������� ������
	virtual void generate_reflections();

	// ����������������� ���������� ����� �� ����, ����� ���������� �������� �� ������������� ��������
	void SetPointsCount(int q);


};
//---------------------------------------------------------------------------

#endif


