/*
 * Util.h
 *
 *  Created on: Feb 23, 2017
 *      Author: zaqc
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <png.h>

void read_png_file(char* file_name);
void write_png_file(char* file_name);

extern png_bytep * row_pointers;

#endif /* UTIL_H_ */
