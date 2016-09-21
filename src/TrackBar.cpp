/*
 * TrackBar.cpp
 *
 *  Created on: Sep 12, 2016
 *      Author: zaqc
 */

#include "TrackBar.h"

#include <iostream>

#include <SDL.h>

TrackBar::TrackBar(int aX, int aY, int aW, int aH) :
		Control() {
	m_X = aX;
	m_Y = aY;
	m_W = aW;
	m_H = aH;
	m_ThumbWidth = 48;
	m_ThumbHeight = 24;
	m_TrackSize = 12;
	m_Border = 4;
	m_Min = 0;
	m_Max = 100;
	m_Pos = 0;

	m_MouseDownX = m_MouseDownY = 0;
	m_MouseDown = false;

	m_PosDelta = 0;

	m_Revert = true;
}

TrackBar::~TrackBar() {
}

bool TrackBar::ProcessEvent(SDL_Event aEvent) {
	switch (aEvent.type) {
	case SDL_MOUSEBUTTONDOWN: {
		SDL_Rect r = CalcThumbRect();
		int x = aEvent.button.x;
		int y = aEvent.button.y;
		if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.y) {
			m_PosDelta = m_Pos;
			m_MouseDown = true;
			m_MouseDownX = aEvent.button.x - (r.x + r.w / 2);
			m_MouseDownY = aEvent.button.y - (r.y + r.h / 2);

			return true;
		}
		break;
	}

	case SDL_MOUSEBUTTONUP:
		if (m_MouseDown) {
			int x = aEvent.motion.x;
			int y = aEvent.motion.y;
			if (x >= m_X && x < m_X + m_W && y >= m_Y && y < m_Y + m_H) {
				m_Pos = m_PosDelta;
			}
			m_MouseDown = false;

			return true;
		}
		break;

	case SDL_MOUSEMOTION: {
		if (m_MouseDown) {
			int y = aEvent.motion.y;
			int delta = y - m_Y - m_ThumbHeight / 2 - m_MouseDownY;
			if (m_Revert)
				delta = m_MouseDownY + m_Y + m_H - m_ThumbHeight / 2 - m_Border
						- y;

			m_PosDelta = m_Min
					+ (double) (delta)
							/ (double) (m_H - m_Border * 2 - m_ThumbHeight)
							* (double) (m_Max - m_Min);
			if (m_PosDelta < m_Min)
				m_PosDelta = m_Min;
			if (m_PosDelta > m_Max)
				m_PosDelta = m_Max;

			return true;
		}
		break;
	}
	}

	return false;
}

void TrackBar::PaintBackground(SDL_Renderer *aRnd, int aX, int aY, int aW,
		int aH) {
	SDL_SetRenderDrawColor(aRnd, 192, 192, 202, 255);
	SDL_Rect r = { aX, aY, aW, aH };
	SDL_RenderFillRect(aRnd, &r);

	SDL_SetRenderDrawColor(aRnd, 0, 0, 0, 255);
	SDL_RenderDrawRect(aRnd, &r);
}

void TrackBar::PaintTrack(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH) {
	SDL_SetRenderDrawColor(aRnd, 0, 0, 128, 255);
	SDL_Rect r = { aX, aY, aW, aH };
	SDL_RenderFillRect(aRnd, &r);

	SDL_SetRenderDrawColor(aRnd, 0, 0, 32, 255);
	SDL_RenderDrawLine(aRnd, aX + aW, aY, aX + aW, aY + aH);
	SDL_RenderDrawLine(aRnd, aX, aY + aH, aX + aW, aY + aH);

	SDL_SetRenderDrawColor(aRnd, 0, 0, 222, 255);
	SDL_RenderDrawLine(aRnd, aX, aY, aX + aW, aY);
	SDL_RenderDrawLine(aRnd, aX, aY, aX, aY + aH);
}

void TrackBar::PaintThumb(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH) {
	SDL_SetRenderDrawBlendMode(aRnd, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(aRnd, 32, 32, 32, 192);
	SDL_Rect r = { aX, aY, aW, aH };
	SDL_RenderFillRect(aRnd, &r);
	SDL_SetRenderDrawBlendMode(aRnd, SDL_BLENDMODE_NONE);
}

SDL_Rect TrackBar::CalcThumbRect() {
	int pos = m_Pos;
	if (m_MouseDown)
		pos = m_PosDelta;

	int x = m_X + (m_W - m_ThumbWidth) / 2;
	int y = m_Y + m_Border
			+ (int) ((double) (m_H - m_Border * 2 - m_ThumbHeight)
					/ (double) (m_Max - m_Min) * (double) (pos - m_Min));

	if (m_Revert) {
		y = m_Y + m_H - m_Border - m_ThumbHeight
				- (int) ((double) (m_H - m_Border * 2 - m_ThumbHeight)
						/ (double) (m_Max - m_Min) * (double) (pos - m_Min));
	}

	SDL_Rect r = { x, y, m_ThumbWidth, m_ThumbHeight };

	return r;
}

bool TrackBar::PtInThumb(int aX, int aY) {
	SDL_Rect r = CalcThumbRect();
	if (aX >= r.x && aX < r.x + r.w && aY >= r.y && aY < r.y + r.h)
		return true;

	return false;
}

void TrackBar::Render(SDL_Renderer *aRnd) {
	PaintBackground(aRnd, m_X, m_Y, m_W, m_H);
	int x = m_X + (m_W - m_TrackSize) / 2;
	int y = m_Y + m_Border + m_ThumbHeight / 2;
	PaintTrack(aRnd, x, y, m_TrackSize, m_H - m_Border * 2 - m_ThumbHeight);

	SDL_Rect r = CalcThumbRect();
	PaintThumb(aRnd, r.x, r.y, m_ThumbWidth, m_ThumbHeight);
}
