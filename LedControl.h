#pragma once
#include <iostream>
#include <string>
#include <Windows.h>
#include <time.h>
#define INPUT_PULLUP 1
#define A1 1
#define A0 0
using namespace std;

extern int PushX, PushY, PushZ, nowx, nowy, BodyX[64], BodyY[64], BodyLen, x, y, i, FoodX, FoodY, Map[8][8], FoodShow, TarX, TarY, p;

extern int Dire;//Dire表示蛇的方向：上下左右分别是1234

void pinMode(int a, int b) {};

string orders[] = { "上", "下", "右", "左" };

class LedControl
{
public:
	LedControl(int a = 0, int b = 0, int c = 0, int d = 0);
	~LedControl();
	void setRow(int a, int b, char c);
	void shutdown(int a, bool b) {} //启动时，MAX72XX处于省电模式
	void setIntensity(int a, int b) {} //将亮度设置为合适的值(0 - 8)
	void clearDisplay(int a) {} //清除显示
};

LedControl::LedControl(int a, int b, int c, int d)
{
}

LedControl::~LedControl()
{
}

inline void LedControl::setRow(int a, int b, char c)
{
	if (b == 0)
		cout << "\n\n----------------------------------\n\n";
	unsigned int ttt = (unsigned int)c;
	string k = "";
	for (int i = 0; i < 8; i++)
	{
		if (ttt % 2)
			k = k + "X";
		else
			k = k + " ";
		ttt /= 2;
	}
	cout << k << endl;
}

class Se
{
public:
	Se();
	~Se();
	void begin(int t) {};
	void print(const string a) { cout << a; };
	void println(const string a) { cout << a << endl; };
	void print(const int a) { cout << a; };
	void println(const int a) { cout << a << endl; };
} Serial;

Se::Se()
{
	srand((unsigned int)time(NULL));
}

Se::~Se()
{
}

int analogRead(int t)
{
	int m;
	cout << t << "大于750向左向上！input:";
	cin >> m;
	cout << endl;
	return m;
}

int digitalRead(int k)
{
	int p;
	cout << k << "Diginput:";
	cin >> p;
	cout << endl;
	system("cls");
	if (p)
		return 1;
	else
		return 0;
}

void delay(int time)
{
	Sleep(time / 1000 + 1);
}

int random(int a, int b)
{
	int p = (b - a + 1) * rand() / RAND_MAX;
	return a + p;
}

void setup();
void loop();

int main()
{
	Dire = 3;
	setup();
	while (true)
	{
		loop();
		cout << "现在： " << orders[Dire - 1] << endl;
	}
}