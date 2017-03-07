#include <cstdio>
#include "ViewContainer.h"

using namespace ScreenView;

ViewContainer::ViewContainer()
	: m_ActiveView(InvalidViewIndex)
{

}
ViewContainer::~ViewContainer()
{
	for (auto iView : m_vecViews)
		delete iView;
}
size_t ViewContainer::AddView(BaseView* aView)
{
	if (!aView)
		return InvalidViewIndex;
	m_vecViews.push_back(aView);
	m_ActiveView = m_vecViews.size() - 1;
	return m_ActiveView;
}
bool ViewContainer::SetActiveView(size_t aViewIndex)
{
	if (InvalidViewIndex == aViewIndex || m_vecViews.size() <= aViewIndex)
		return false;
	m_ActiveView = aViewIndex;
	return true;
}
bool ViewContainer::NextView()
{
	return false;
}
bool ViewContainer::PrevView()
{
	return false;
}
void ViewContainer::Render()
{
	if (InvalidViewIndex == m_ActiveView)
		return;
	m_vecViews[m_ActiveView]->Render();
}
