#pragma once
//#include "GUSDataCacher.h"

namespace ScreenView
{
	class BaseView
	{
	public:
		BaseView();
		~BaseView();
		virtual void AddPacket();
		virtual void Render();
	};
}
