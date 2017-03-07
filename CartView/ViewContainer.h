#pragma once
#include <vector>
#include "BaseView.h"

namespace ScreenView
{
	class ViewContainer
	{
	protected:
		std::vector<BaseView*> m_vecViews;
		size_t m_ActiveView;
	public:
		const size_t InvalidViewIndex = (size_t)(-1);
	public:
		ViewContainer();
		~ViewContainer();
		size_t AddView(BaseView* aView);
		bool SetActiveView(size_t aViewIndex);
		bool NextView();
		bool PrevView();
		void Render();
	};
}