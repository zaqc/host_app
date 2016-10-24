/*
 * main.cpp
 *
 *  Created on: Sep 12, 2016
 *      Author: zaqc
 */

#include <unistd.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <memory.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>

#include "TrackBar.h"
#include "Label.h"
#include "Button.h"
#include "Window.h"
#include "AScanWnd.h"
#include "Desktop.h"

int main(void) {
//	int fd = open("/dev/fb0", O_RDWR);
//	std::cout << "Hi! fd=" << fd << std::endl;
//
	Desktop *dt = new Desktop();
	dt->Init();
	dt->Run();
	dt->Done();
	delete dt;

	return 0;

	std::cout << SDL_Init(SDL_INIT_VIDEO) << std::endl;

	SDL_Window *wnd;
	SDL_Renderer *rnd;

	SDL_CreateWindowAndRenderer(800, 480, SDL_WINDOW_BORDERLESS
	/*SDL_WINDOW_BORDERLESS |
	 SDL_WINDOW_FULLSCREEN*/, &wnd, &rnd);

	SDL_GL_SetSwapInterval(1);

	TTF_Init();

	Window *w = new AScanWnd(rnd, 0, 0, 800, 480);
	w->Init();
	//w->Execute();

	return 0;

	Label *l = new Label(100, 5, 128, 28, "0%");
	TrackBar *tb = new TrackBar(20, 20, 64, 400);
	Button *btn = new Button(100, 40, 120, 40, "Button1");

	SDL_Event e;

	timeval ts, ts_prev;
	gettimeofday(&ts, 0);
	int fc = 0;
	ts_prev = ts;

	while (1) {
		bool quit = false;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
				break;
			}
			tb->ProcessEvent(e);
			btn->ProcessEvent(e);
			int val = tb->GetValue();
			char str[128];
			sprintf(str, "%i", val);
			l->SetText(std::string(str));
		}

		if (quit)
			break;

		SDL_SetRenderDrawColor(rnd, 255, 255, 255, 255);
		SDL_Rect r = { 0, 0, 800, 480 };
		SDL_RenderFillRect(rnd, &r);

		l->Render(rnd);
		tb->Render(rnd);
		btn->Render(rnd);

		SDL_RenderClear(rnd);

		stringRGBA(rnd, 100, 100, "asdasdasd", 255, 0, 0, 255);
		roundedRectangleRGBA(rnd, 100, 100, 200, 200, 10, 255, 0, 0, 255);

		//ioctl(fd, FBIO_WAITFORVSYNC, 1);
		SDL_RenderPresent(rnd);
		//usleep(20 * 1000);

		gettimeofday(&ts, 0);
		float delta = (ts.tv_sec * 1000000 + ts.tv_usec)
				- (ts_prev.tv_sec * 1000000 + ts_prev.tv_usec);

		if (delta >= 1000000.0f) {
			std::cout << (float) fc * 1000000.0 / delta << std::endl;

			fc = 0;
			ts_prev = ts;
		}
		fc++;

	}

	TTF_Quit();

	SDL_DestroyRenderer(rnd);
	SDL_DestroyWindow(wnd);

	SDL_Quit();

	return 0;
}
