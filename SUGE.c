/**
 * Name: SUGE.c v2.3.0
 * Date: 2021-09-12
 * Intro: A lib to simulate astronomical phenomenons.
 * Update: Use gdiTemplate.h. Change configure file from SUGE.cfg2 to SUGE and 
 *		allow to appoint configure file. Upgrade configure file from v2 to v3.
 * Open configure file SUGE by default, which can be appointed by cmd argument:
 * - v2:
 *		ver: 2
 *		num: Numbers of celestial bodies
 *		X position (a list of length num)
 *		Y position (a list of length num)
 *		X velocity (a list of length num)
 *		Y velocity (a list of length num)
 *		Quality (a list of length num)
 *		Foreground color (a list of length num in hex BBGGRR)
 *		Background color
 *		Simulate accuracy
 *		Display frequent (ticks in each display)
 *		Width of the line
 *		X/Y offset
 * - v3:
 *		ver: 3
 *		num: Numbers of celestial bodies
 *		X position (a list of length num)
 *		Y position (a list of length num)
 *		X velocity (a list of length num)
 *		Y velocity (a list of length num)
 *		Quality (a list of length num)
 *		Width (a list of length num)
 *		Foreground color (a list of length num in hex BBGGRR)
 *		Background color
 *		Simulate accuracy
 *		Display frequent (ticks in each display)
 *		X/Y offset
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

#define TITLE "SUGE"
COLORREF BACKGROUND_COLOR;

#include "gdiTemplate.c"

#ifdef USE_SSE
#include <xmmintrin.h>
__m128 m, n;
#endif

int ver = -1, num, display_freq, offset[2], count, i, j;
double simulate_acc, distance, p, q, r, s;
LPSTR fileName = "SUGE";
DWORD WINAPI threadProc(LPVOID lpParamter) {
	FILE *file = fopen(fileName, "r");
	HWND hwnd = (HWND)lpParamter;
	fscanf(file, "%d%d", &ver, &num);
	HDC hdc[num];
	HPEN hpen[num];
	double position[num][2], velocity[num][2], quality[num];
	int width[num];
	COLORREF color[num];
	if(ver == 2) {
		for(i=0; i<num; ++i) fscanf(file, "%lf", &position[i][0]);
		for(i=0; i<num; ++i) fscanf(file, "%lf", &position[i][1]);
		for(i=0; i<num; ++i) fscanf(file, "%lf", &velocity[i][0]);
		for(i=0; i<num; ++i) fscanf(file, "%lf", &velocity[i][1]);
		for(i=0; i<num; ++i) fscanf(file, "%lf", &quality[i]);
		for(i=0; i<num; ++i) fscanf(file, "%lx", &color[i]);
		fscanf(file, "%lx %lf %d %d %d %d", &BACKGROUND_COLOR, &simulate_acc, &display_freq, &width[0], &offset[0], &offset[1]);
		for(i=0; i<num; ++i) width[i] = width[0];
	} else if(ver == 3) {
		for(i=0; i<num; ++i) fscanf(file, "%lf", &position[i][0]);
		for(i=0; i<num; ++i) fscanf(file, "%lf", &position[i][1]);
		for(i=0; i<num; ++i) fscanf(file, "%lf", &velocity[i][0]);
		for(i=0; i<num; ++i) fscanf(file, "%lf", &velocity[i][1]);
		for(i=0; i<num; ++i) fscanf(file, "%lf", &quality[i]);
		for(i=0; i<num; ++i) fscanf(file, "%d", &width[i]);
		for(i=0; i<num; ++i) fscanf(file, "%lx", &color[i]);
		fscanf(file, "%lx %lf %d %d %d", &BACKGROUND_COLOR, &simulate_acc, &display_freq, &offset[0], &offset[1]);
	} else {
		printf("Version not support: %d", ver);
		return 1;
	}
	fclose(file);
	for(i=0; i<num; ++i) {
		quality[i] *= simulate_acc * simulate_acc;
		velocity[i][0] *= simulate_acc;
		velocity[i][1] *= simulate_acc;
	}
	for(i=0; i<num; ++i) {
		hdc[i] = GetDC(hwnd);
		hpen[i] = CreatePen(PS_SOLID, width[i], color[i]);
		SelectObject(hdc[i], hpen[i]);
		position[i][0] += offset[0];
		position[i][1] += offset[1];
		MoveToEx(hdc[i], position[i][0], position[i][1], 0);
	}
	while(1) {
		for(i=0; i<num; ++i) {
			for(j=0; j<i; ++j) {
				p = position[j][0] - position[i][0];
				q = position[j][1] - position[i][1];
				if(p==0 && q==0) continue;
#ifdef USE_SSE
				m = _mm_set1_ps(p * p + q * q);
				n = _mm_rsqrt_ps(m);
				distance = n[0] * n[0] * n[0];
#else
				distance = pow(p * p + q * q, -1.5);
#endif
				r = quality[j] * distance;
				s = quality[i] * distance;
				velocity[i][0] += p * r;
				velocity[i][1] += q * r;
				velocity[j][0] -= p * s;
				velocity[j][1] -= q * s;
			}
		}
		if(count % display_freq == 0) {
			for(i=0; i<num; ++i) {
				LineTo(hdc[i], position[i][0], position[i][1]);
			}
		}
		for(i=0; i<num; ++i) {
			position[i][0] += velocity[i][0];
			position[i][1] += velocity[i][1];
		}
		++count;
	}
}

int WINAPI initProc(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR pCmdLine, int nCmdShow) {
	if(strlen(pCmdLine) != 0) {
		fileName = pCmdLine;
	}
	return 0;
}
