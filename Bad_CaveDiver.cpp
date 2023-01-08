#include <iostream>
#include <thread>
#include <math.h>

#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h>

#ifdef UNICODE_WAS_UNDEFINED
#undef UNICODE_WAS_UNDEFINED
#endif

using namespace std;
using namespace chrono;



int main()
{
	DWORD d = 0;
	GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &d);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), d | 0x0004); 
	GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &d);
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), d | 0x0200);

	auto tp1 = system_clock::now();	auto tp2 = tp1;
	char s[80 * 30] = {'\0'};
	char n[10];
	
	auto pr_draw = [](const char* b, int x, int y)	{printf("\x1B[%d;%dH%s", y, x, b);};
	auto row_fill = [&](int x, int y, char c){s[y * 80 + x] = c; };

	float t = 0.0f, f = 0.0f, tt = 0.0f; // Timing Variables
	float w = 20.0f, m = 40.0f, tw = 20.0f, tm = 40.0f; // World Shape Variables
	float e = 1.0f, c = 40.0f; // Difficulty, Player Position
	int r = 0, h = 1000; // Player Score
	
	while (h>0)
	{
		// Timing
		this_thread::sleep_for(10ms);
		tp2 = system_clock::now();
		duration<float> elapsedTime = tp2 - tp1; tp1 = tp2;
		t+=(f=elapsedTime.count());	tt += f;
		
		// Get and Handle Input
		for (int i = 0; i < 2; i++)	if (GetAsyncKeyState("\x25\x27"[i])!= 0) c += (i?1:-1) * 40.0f * f;

		// Update World
		if (t >= 1.5f){	t -= 1.5f;	tw = rand() % 10 + 10;	tm = rand() % (76 - ((int)tw >> 1)) + 4; }
		e += f*0.001f;	w += (tw - w) * e * f; m += (tm - m) * e * f;
		float p = sinf(tt * e) * m / 2.0f + 40.0f;

		// Fill Row
		for (int x = 0; x < 79; x++)
			if (x+1 < p-w/2 || x> p+w/2) row_fill(x,r,'.');
			else if (x == (int)(p - (w / 2)) || x== (int)(p + (w / 2)))	row_fill(x, r, '#');
			else row_fill(x,r,' ');
	
		// Scrolling Effect
		row_fill(79, r, '\0'); (r += 1) %= 28;

		// Draw To Screen
		for (int y = 1; y < 28; y++) pr_draw(&s[((y+28+r)%28) * 80], 0, y);
		pr_draw("O-V-O", c - 2, 2); pr_draw(" O-O ", c - 2, 3); pr_draw("  V  ", c - 2, 4);
		
		// Collision Checking & Health Update
		h -= s[((4 + r) % 28) * 80 + ((int)c - 2)] == '.';
		
		// Draw HUD	
		pr_draw("===============================================================================", 0, 28);
		pr_draw("Cave Diver - Pretty Cool huh ? - Left Arrow / Right Arrow - Survive!", 2, 29);
		_itoa_s((int)(tt * 100.0f), n, 10, 10);
		pr_draw("DISTANCE FALLEN :           ", 2, 30); pr_draw(n, 19, 30);

		_itoa_s(h, n, 10, 10);
		pr_draw("HEALTH :            ", 40, 30); pr_draw(n, 48, 30);
	}

	pr_draw("Dead...\n", 70, 30);
	system("pause"); // :-P
	return 0;
}
