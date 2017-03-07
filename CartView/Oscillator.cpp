//---------------------------------------------------------------------------
#ifdef __BORLANDC__
	#pragma hdrstop
	#pragma package(smart_init)
#endif

//---------------------------------------------------------------------------

#include "Oscillator.h"
//---------------------------------------------------------------------------

TOscillator::TOscillator(RailFigure *in_rail)
			: rail(in_rail)
{
    begin_work_reflect = -1;
	control_zone_begin = 0;
	control_zone_end = 240;
	VRC_zone = 25;
    total_length = 0;
	PhyDelay = 0;
	tick_time = 1.56f; // * 2
	speed = 3.260f; //5.900f;
	SetPointsCount(std_points_on_line);
}

void TOscillator::reset(float oscill_point1, int face_type, float alpha, float beta)
{
    if(alpha >= 0 && beta >= 0 && rail)
	{
		oscill_point[0].reset(0, rail->points[rail->points.size() - 1].y, oscill_point1);
		if(face_type == 0)
			Vectors[0].reset(0, -30, 0);
        else
		{
            float t = 30 * sinf(float(alpha / 180.0 * M_PI));
			Vectors[0].x = t * sinf(float(beta / 180.0 * M_PI));
			Vectors[0].y = -30 * cosf(float(alpha / 180.0 * M_PI));
			Vectors[0].z = t * cosf(float(beta / 180.0 * M_PI)) * ((face_type == 1) ? -1 : 1 );
		}
    	generate_reflections();
    }
	else reflect_count = 0;
}

void  TOscillator::reset(float* oscill_point1, float* income_vector)
{
	if(oscill_point1 == NULL || income_vector == NULL)
		return;
	oscill_point[0].reset_arr(oscill_point1);
	Vectors[0].reset_arr(income_vector);
	//for(int i = 0; i < 2; ++i) if(sign(vector[0][i]) != sign(income_vector[i])) return;
    generate_reflections();
}

void TOscillator::generate_reflections()
{
	step = speed * tick_time / 2;
    reflect_count = 1;
	float last_length = total_length = 0;
    begin_work_reflect = -1;
	reflect_count = 1;
    Vectors[0].norm();
	while(true)
    {
        get_work_length(reflect_count - 1);
		if(reflect_count > 7 || last_length == total_length)
			break;
		if(total_length >= control_zone_end / 240 * step * points.size())
			break;
		last_length = total_length;
		++reflect_count;
	}

	float iterator = 0.5f,
		numb00 = -1;
	int reflect_n = -1;
	for(UINT k = 0; k < points.size(); ++k, ++iterator)
	{
		if(iterator > numb00)
        {
			if(++reflect_n >= reflect_count)
				break;
			iterator = 0.5f;
			numb00 = oscill_point[reflect_n + 1].get_len(oscill_point[reflect_n]) / step;
		}
		points[k] = oscill_point[reflect_n] + Vectors[reflect_n] * (iterator * step);
	}
}

void TOscillator::SetPointsCount(int q)
{
	points.resize(q);
}

void TOscillator::get_work_length(int vect_numb)
{
	TFLine connected_line,
		end_line;
	rail->CheckPointConnected(oscill_point[vect_numb], connected_line);
	float vect_len = rail->CheckVectorLength(connected_line, oscill_point[vect_numb], Vectors[vect_numb], end_line);

	if(total_length == control_zone_begin / 240 * step * points.size() && begin_work_reflect == -1)
		begin_work_reflect = vect_numb;
	if(vect_len == 0)
		vect_len = control_zone_end / 240 * step * points.size();
	if(Vectors[vect_numb].z != 1 && Vectors[vect_numb].z != -1)
		vect_len /= sqrtf(1 - Vectors[vect_numb].z * Vectors[vect_numb].z);
	total_length += vect_len;
	bool cutted = false;

	if(total_length > control_zone_end / 240 * step * points.size())
    {
		float n = total_length - control_zone_end / 240 * step * points.size();
        total_length -= n;
		vect_len -= n;
		cutted = true;
    }

	oscill_point[vect_numb + 1] = oscill_point[vect_numb] + Vectors[vect_numb] * vect_len;
	if(rail->IsInside(oscill_point[vect_numb]) && !cutted)
		rail->GetReflectionVector(end_line, oscill_point[vect_numb + 1],
									Vectors[vect_numb], Vectors[vect_numb + 1]);
	else Vectors[vect_numb + 1] = Vectors[vect_numb];
}
//---------------------------------------------------------------------------

int TOscillator::std_points_on_line = 128;
//---------------------------------------------------------------------------


