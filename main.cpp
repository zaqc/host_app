/*
 * main.cpp
 *
 *  Created on: Sep 12, 2016
 *      Author: zaqc
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <memory.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <locale.h>
#include <bits/wchar.h>

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

#include "hw/Transmitter.h"

#include <zlib.h>
#include <ftdi.h>
//----------------------------------------------------------------------------

#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (x), (y))

int fr_data_len = 0;

#define	PB_SIZE		(1024 * 4)
unsigned char pre_buf[PB_SIZE];
int pb_len = 0;
int pb_ptr = 0;

int get_data(ftdi_context *aFTDI, unsigned char *aBuf, int aSize) {
	int s = 0;
	int bc;

	if (pb_len < PB_SIZE / 2) {
		if (pb_len != 0) {
			memmove(pre_buf, &pre_buf[pb_ptr], pb_len);
			pb_ptr = 0;
		}
		while (pb_len < PB_SIZE) {
			bc = ftdi_read_data(aFTDI, &(pre_buf[pb_len]), PB_SIZE - pb_len);
			if (bc < 0)
				return -1;

			if (bc == 0) {
				usleep(1);
			}
			else
				pb_len += bc;
		}
	}

	if (aSize < pb_len) {
		memcpy(aBuf, &pre_buf[pb_ptr], aSize);
		pb_ptr += aSize;
		pb_len -= aSize;

		fr_data_len += aSize;

		return aSize;
	}
	else {
		memcpy(aBuf, &pre_buf[pb_ptr], pb_len);
		pb_len = 0;
		pb_ptr = 0;
		s += pb_len;
	}

	while (s < aSize) {
		bc = ftdi_read_data(aFTDI, &(aBuf[s]), aSize - s);

		if (bc < 0)
			return -1;

		if (bc == 0) {
			usleep(1);
		}
		else
			s += bc;
	}

	fr_data_len += s;

	return s;
}
//----------------------------------------------------------------------------

enum rcv_state {
	none,
	wait_preamble,
	get_preamble,
	get_data_buf,
	get_pkt_counter_1,
	get_pkt_counter_2,
	get_data_len,
	get_ldata,
	get_lch_mask,
	get_rdata,
	get_rch_mask,
	get_crc
} state;
//----------------------------------------------------------------------------

unsigned int pkt_cntr_1;
unsigned int pkt_cntr_2;
int pkt_cntr_err = 0;
int ch_error = 0;

#define REV_BYTE_ORDER(a)	(((unsigned int)(a & 0xFF) << 24) | ((unsigned int)(a & 0xFF00) << 8) | ((unsigned int)(a & 0xFF0000) >> 8) | ((unsigned int)(a & 0xFF000000) >> 24))

void decode_buf(unsigned char *aBuf, int aSize) {
	int len = aSize / 5;
	unsigned char *buf = aBuf;
	for (int i = 0; i < len; i++) {
		if ((*buf & 0xF0) == 0xA0) {
			if ((*buf & 0x0F) == 0x0F) {
				unsigned int pkt_cntr = REV_BYTE_ORDER(*(unsigned int*) (buf + 1));

				if (pkt_cntr_1 + 1 != pkt_cntr)
					pkt_cntr_err++;
				pkt_cntr_1 = pkt_cntr;
			}
		}
		else if ((*buf & 0xF0) == 0x50) {
			if ((*buf & 0x0F) == 0x0F) {
				unsigned int pkt_cntr = REV_BYTE_ORDER(*(unsigned int*) (buf + 1));

				if (pkt_cntr_2 + 1 != pkt_cntr)
					pkt_cntr_err++;
				pkt_cntr_2 = pkt_cntr;
			}
		}
		else
			ch_error++;

		buf += 5;
	}
}

void * decode_thread_0(void * context) {
	ftdi_context *ftdi;

	clear();

	if ((ftdi = ftdi_new()) == 0) {
		std::cout << "ftdi_new failed" << std::endl;
		throw "ftdi_new failed";
	}

	ftdi_set_interface(ftdi, INTERFACE_A);
	int f = ftdi_usb_open(ftdi, 0xEDA3, 0x2179); // 0x0403, 0x6010); //
	if (f < 0 && f != -5) {
		std::cout << "unable to open ftdi device: " << f << ftdi_get_error_string(ftdi) << std::endl;
		ftdi_free(ftdi);
		throw "ftdi_new failed";
	}
	std::cout << "ftdi open succeeded(channel 1): " << f << std::endl;

	int res = ftdi_set_bitmode(ftdi, 0xFF, BITMODE_SYNCFF);
	std::cout << "ftdi_set_bitmode res=" << res << std::endl;

	res = ftdi_read_data_set_chunksize(ftdi, 64 * 1024);
	std::cout << "ftdi_read_data_set_chunksize res=" << res << std::endl;

	ftdi_set_latency_timer(ftdi, 255);

	ftdi_setflowctrl(ftdi, SIO_DISABLE_FLOW_CTRL);

	res = ftdi_usb_purge_buffers(ftdi);
	std::cout << "ftdi_usb_purge_buffers res=" << res << std::endl;

	clear();

	unsigned char buf[1024];
	int n = 0;

	state = none;
	int prm_count = 0;
	unsigned char ch;
	unsigned int w;
	int l_len = 0;
	int r_len = 0;
	unsigned char data_buf[1024 * 4];
	bool running = true;

	int err_count = 0;
	int pk_counter = 0;
	int pkcnt = 0;

	timeval ts, ts_prev;
	gettimeofday(&ts, 0);
	int fc = 0;
	ts_prev = ts;

	while (running) {
		switch (state) {
			case none:
				state = wait_preamble;
				break;

			case wait_preamble: {
				if (get_data(ftdi, &ch, 1) == 1) {
					if (ch == 0x55)
						prm_count++;
					else {
						if (ch == 0xD5 && prm_count == 4)
							state = get_data_buf;
						//else
						//err_count++;

						prm_count = 0;
					}
				}
				else
					running = false;

				break;
			}

			case get_preamble: {
				if (get_data(ftdi, data_buf, 5) == 5) {
					state = get_data_buf;
					for (int i = 0; i < 4; i++)
						if (data_buf[i] != 0x55) {
							state = wait_preamble;
							break;
						}

					if (state == get_data_buf && data_buf[4] != 0xD5)
						state = wait_preamble;

					if (state != get_data_buf)
						err_count++;
				}
				else
					running = false;
				break;
			}

			case get_data_buf: {
				if (get_data(ftdi, data_buf, 100) == 100) {
					decode_buf(data_buf, 100);
					if (pk_counter++ >= 100000) {
						gotoxy(1, 1);
						printf("error_counter=%i\n", err_count);
						printf("data size=%i\n", fr_data_len);
						printf("channel mask error=%i\n", ch_error);
						printf("packet counter error=%i\n", pkt_cntr_err);
						fr_data_len = 0;
						pk_counter = 0;

						gettimeofday(&ts, 0);
						float delta = (ts.tv_sec * 1000000 + ts.tv_usec) - (ts_prev.tv_sec * 1000000 + ts_prev.tv_usec);
						printf("time elepsed=%6.4f\n", delta / 1000000.0);
						ts_prev = ts;
					}

					state = get_preamble;
				}
				else
					running = false;
				break;
			}
		}
	}

	while (running) {
		if (get_data(ftdi, buf, 1024) == 1024) {
			if (n++ > 1000) {
				n = 0;
				clear();
				gotoxy(1, 1);
				for (int i = 0; i < 1024; i++) {
					printf("0x%02X ", buf[i]);
					if (buf[i] == 0xD5) {
						printf("\n");
						//running = false;
					}
				}
				printf("\n");
			}
		}
		else {
			running = false;
		}
	}

	exit(0);

	return NULL;
}
//----------------------------------------------------------------------------

void * decode_thread(void * context) {
	ftdi_context *ftdi;

	clear();

	if ((ftdi = ftdi_new()) == 0) {
		std::cout << "ftdi_new failed" << std::endl;
		throw "ftdi_new failed";
	}

	ftdi_set_interface(ftdi, INTERFACE_A);
	int f = ftdi_usb_open(ftdi, 0xEDA3, 0x2179); // 0x0403, 0x6010); //
	if (f < 0 && f != -5) {
		std::cout << "unable to open ftdi device: " << f << ftdi_get_error_string(ftdi) << std::endl;
		ftdi_free(ftdi);
		throw "ftdi_new failed";
	}
	std::cout << "ftdi open succeeded(channel 1): " << f << std::endl;

	int res = ftdi_set_bitmode(ftdi, 0xFF, BITMODE_SYNCFF);
	std::cout << "ftdi_set_bitmode res=" << res << std::endl;

	res = ftdi_read_data_set_chunksize(ftdi, 64 * 1024);
	std::cout << "ftdi_read_data_set_chunksize res=" << res << std::endl;

	ftdi_set_latency_timer(ftdi, 255);

	ftdi_setflowctrl(ftdi, SIO_DISABLE_FLOW_CTRL);

	res = ftdi_usb_purge_buffers(ftdi);
	std::cout << "ftdi_usb_purge_buffers res=" << res << std::endl;

	clear();

	state = none;
	int prm_count = 0;
	unsigned char ch;
	unsigned int w;
	volatile unsigned int pkt_cntr_1;
	volatile unsigned int pkt_cntr_2;
	volatile unsigned int prev_pkt_cntr_1;
	volatile unsigned int prev_pkt_cntr_2;
	int l_len = 0;
	int r_len = 0;
	unsigned char data_buf[1024 * 4];
	bool running = true;

	int err_count = 0;
	int pk_counter = 0;
	int pkcnt = 0;
	while (running) {
		switch (state) {
			case none:
				state = wait_preamble;
				break;

			case wait_preamble: {
				if (get_data(ftdi, &ch, 1) == 1) {
					if (ch == 0x55)
						prm_count++;
					else {
						if (ch == 0xD5 && prm_count == 4)
							state = get_pkt_counter_1;
						//else
						//err_count++;

						prm_count = 0;
					}

				}
				else
					running = false;

				break;
			}

			case get_preamble: {
				if (get_data(ftdi, data_buf, 8) == 8) {
					state = get_pkt_counter_1;
					for (int i = 0; i < 7; i++)
						if (data_buf[i] != 0x55) {
							state = wait_preamble;
							//err_count++;
							break;
						}
					if (state == get_pkt_counter_1 && data_buf[7] != 0xD5) {
						state = wait_preamble;
						//err_count++;
					}

					if (state != get_pkt_counter_1)
						err_count++;
				}
				else
					running = false;
				break;
			}

			case get_pkt_counter_1: {
				pk_counter++;
				if (pk_counter == 100) {
					pk_counter = 100;
					//gotoxy(0, 5);
				}
				if (get_data(ftdi, (unsigned char *) &pkt_cntr_1, 4) == 4) {
//					if (pkt_cntr_1 != prev_pkt_cntr_1 + 1)
//						err_count++;
					prev_pkt_cntr_1 = pkt_cntr_1;
					if (pk_counter == 0)
						printf("pkt_counter 1 = %i ", pkt_cntr_1);
					state = get_pkt_counter_2;
				}
				else
					running = false;
				break;
			}

			case get_pkt_counter_2: {
				if (get_data(ftdi, (unsigned char *) &pkt_cntr_2, 4) == 4) {
//					if (pkt_cntr_2 != prev_pkt_cntr_2 + 1)
//						err_count++;
					prev_pkt_cntr_2 = pkt_cntr_2;

					if (pk_counter == 0)
						printf("pkt_counter 2 = %i\n", pkt_cntr_2);
					state = get_data_len;
				}
				else
					running = false;
				break;
			}

			case get_data_len: {
				if (get_data(ftdi, (unsigned char *) &w, 4) == 4) {
					l_len = w & 0x0000FFFF;
//					if (l_len != 32 * 14 + 1)
//						err_count++;
					r_len = (w & 0xFFFF0000) >> 16;
//					if (r_len != 32 * 14 + 1)
//						err_count++;
					if (pk_counter == 0)
						printf("l_len = %i r_len=%i ", l_len, r_len);
					if (l_len < 1024 * 4 && r_len < 1024 * 4)
						state = get_ldata;
					else {
						state = wait_preamble;
						err_count++;
					}
				}
				else
					running = false;

				break;
			}

			case get_ldata: {
				if (get_data(ftdi, data_buf, l_len * 4) == l_len * 4) {
					if (pk_counter == 0) {
						printf("\n");

						for (int i = 0; i < 32; i++) {
							printf("0x%08X ", ((unsigned int *) data_buf)[i]);
						}
						if (pk_counter == 0)
							printf("\n");
						for (int i = 0; i < 32; i++) {
							printf("0x%08X ", ((unsigned int *) data_buf)[32 + i]);
						}
						if (pk_counter == 0)
							printf("\n");
						for (int i = 0; i < 32; i++) {
							printf("0x%08X ", ((unsigned int *) data_buf)[32 + 32 + i]);
						}
						printf("\n");
					}
					state = get_rdata; //get_lch_mask;
				}
				else
					running = false;
				break;
			}

			case get_lch_mask: {
				if (get_data(ftdi, data_buf, 32) == 32) {
					if (pk_counter == 0) {
						printf("ch_l_mask={");
						for (int i = 0; i < 16; i++) {
							printf("%i", (int) ((short*) data_buf)[i]);
							if (i != 15)
								printf(", ");
						}
						printf("} \n");
					}
					state = get_rdata;
				}
				else
					running = false;
				break;
			}

			case get_rdata: {
				if (get_data(ftdi, data_buf, l_len * 4) == l_len * 4) {
					if (pk_counter == 0) {
						for (int i = 0; i < 64; i++) {
							printf("0x%08X ", ((unsigned int *) data_buf)[i]);
						}
						if (l_len >= 1 && l_len < 1024)
							printf("\n\tcounter=%i\n", ((unsigned int *) data_buf)[l_len - 1]);
					}
					state = get_crc; //get_rch_mask;
				}
				else
					running = false;
				break;
			}

			case get_rch_mask: {
				if (get_data(ftdi, data_buf, 32) == 32) {
					if (pk_counter == 0) {
						printf("ch_r_mask={");
						for (int i = 0; i < 16; i++) {
							printf("%i", (int) ((short*) data_buf)[i]);
							if (i != 15)
								printf(", ");
						}
						printf("}\n");
					}
					state = get_crc;
				}
				else
					running = false;
				break;
			}

			case get_crc: {
				if (get_data(ftdi, (unsigned char *) &w, 4) == 4) {
					if ((pkcnt % 1000) == 0) {
						gotoxy(1, 10);
						printf("\n");
						printf("CRC = 0x%08X\n", w);
						printf("pk1=%i pk2=%i        \n", pkt_cntr_1, pkt_cntr_2);
						printf("l_len=%i r_len=%i    \n", l_len, r_len);
						printf("error count = %i     \n", err_count);
						printf("data_len = %i        \n", fr_data_len);
						fr_data_len = 0;
						usleep(10);
					}
					pkcnt++;

					//if (pk_counter == 100)
					//printf("error count = %i", err_count);
					state = get_preamble;					// wait_preamble;
				}
				else
					running = false;
				break;
			}

			default: {
				state = wait_preamble;
				break;
			}
		}

	}
	return NULL;
}
//----------------------------------------------------------------------------

int main(void) {
//	int fd = open("/dev/fb0", O_RDWR);
//	std::cout << "Hi! fd=" << fd << std::endl;
//

	unsigned char buf[4];
	buf[0] = '1';
	buf[1] = '2';
	buf[2] = '3';
	buf[3] = '4';
	unsigned int val = crc32(0l, buf, 4);
	printf("CRC32 = 0x%08X \n", val);

	decode_thread_0(NULL);
	decode_thread(NULL);

	ftdi_context *m_FTDI;
	if ((m_FTDI = ftdi_new()) == 0) {
		std::cout << "ftdi_new failed" << std::endl;
		throw "ftdi_new failed";
	}

	ftdi_set_interface(m_FTDI, INTERFACE_A);
	int f = ftdi_usb_open(m_FTDI, 0xEDA3, 0x2179); // 0x0403, 0x6010); //
	if (f < 0 && f != -5) {
		std::cout << "unable to open ftdi device: " << f << ftdi_get_error_string(m_FTDI) << std::endl;
		ftdi_free(m_FTDI);
		throw "ftdi_new failed";
	}
	std::cout << "ftdi open succeeded(channel 1): " << f << std::endl;

	int res = ftdi_set_bitmode(m_FTDI, 0xFF, BITMODE_SYNCFF);
	std::cout << "ftdi_set_bitmode res=" << res << std::endl;

	res = ftdi_read_data_set_chunksize(m_FTDI, 64 * 1024);
	std::cout << "ftdi_read_data_set_chunksize res=" << res << std::endl;

	res = ftdi_usb_purge_buffers(m_FTDI);
	std::cout << "ftdi_usb_purge_buffers res=%i\n" << res << std::endl;

	int pmb_cnt = 0;
	int clmn = 0;

	while (1) {
		int s = 0;
		int bc;
		unsigned char buf[32];
		int size = 32;
		while (s < size) {
			bc = ftdi_read_data(m_FTDI, &(buf[s]), 1);
			if (bc < 0) {
				break;
			}
			if (bc == 0)
				usleep(100);
			else {
				for (int i = 0; i < bc; i++) {
					printf("0x%02X ", buf[s + i]);

					if (buf[s + i] == 0x55)
						pmb_cnt++;
					else {
						if (pmb_cnt == 7 && buf[s + i] == 0xD5) {
							printf("\n");
							clmn = 0;
						}
						pmb_cnt = 0;
					}

					if (clmn && (clmn % 4) == 0)
						printf("| ");
					clmn++;
				}
				s += bc;
			}
		}
//printf("\n");
	}

	return 0;

	Transmitter *tr = new Transmitter();
	tr->Init();

	Desktop *dt = new Desktop();
	dt->Init();
	dt->Run();
	dt->Done();
	delete dt;

	tr->Done();
	delete tr;

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

	Label *l = new Label(100, 5, 128, 28, L"0%");
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
			wchar_t str[128];
			swprintf(str, 128, L"%i", val);
			l->SetText(std::wstring(str));
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
		float delta = (ts.tv_sec * 1000000 + ts.tv_usec) - (ts_prev.tv_sec * 1000000 + ts_prev.tv_usec);

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
