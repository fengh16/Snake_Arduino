///fengh16. 17.06.29 v1.0
///fengh16. 17.06.30 v1.1
///Arduino nano
///接口：摇杆：x-A1；y-A0；z-D2；蜂鸣器：D5（3.3V）
///接口：LED点阵1（上面的）：DIN = 12, CS = 11, CLK = 10；2：CLK = 10, DIN2 = 13, CS2 = 7

#include "LedControl.h"

int PushX, PushY, PushZ, nowx, nowy, BodyX[128], BodyY[128], BodyLen, x, y, i, FoodX, FoodY, Map[8][16], FoodShow = 0, TarX, TarY, p = 0, LimitTime, nowDire, PausePress;
//xyi是全局变量（用于遍历）
char NowMap[16];
int Dire = 3;//Dire表示蛇的方向：上下左右分别是1243
int DIN = 12, CS = 11, CLK = 10, DIN2 = 13, CS2 = 7;//接口
int speakerPin = 5;//蜂鸣器接口
int State = 0;//State表示状态：0代表开始界面；1代表游戏状态；2代表死亡；3代表通关；4代表暂停；5代表阶段性胜利
int LimitTimeBase = 6;//限制( +1)次刷新之后接受蛇的输入
int AgoLen = 0;//上一次蛇过关的长度

LedControl lc = LedControl(DIN, CLK, CS, 4);
LedControl lc2 = LedControl(DIN2, CLK, CS2, 4);

int chars[8]	= { 1, 2, 4, 8, 16, 32, 64, 128 };
char Died[8]	= { 0xE7,0x7E,0x3C,0x18,0x18,0x3C,0x7E,0xE7 };
char Suc[8]		= { 0x00,0x01,0x03,0x06,0x8C,0xD8,0x70,0x20 };
char Start[8]	= { 0x1E,0x20,0x20,0x3E,0x02,0x02,0x02,0x7C };
char Pause[8]	= { 0x00,0x38,0x24,0x24,0x38,0x20,0x20,0x00 };
char Two[8]		= { 0x00,0x7E,0x18,0x18,0x18,0x18,0x18,0x00 };
char OK[8]		= { 0x62,0x66,0x6C,0x78,0x78,0x6C,0x66,0x62 };
char ScoresNow[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
char secondDig[10][8] =
{ { 0x00,0xE0,0xA0,0xA0,0xA0,0xA0,0xE0,0x00 }, { 0x00,0x40,0x40,0x40,0x40,0x40,0x40,0x00 }, { 0x00,0xE0,0x20,0xE0,0x80,0x80,0xE0,0x00 }, { 0x00,0xE0,0x20,0xE0,0x20,0x20,0xE0,0x00 }, { 0x00,0xA0,0xA0,0xA0,0xF0,0x20,0x20,0x00 }, { 0x00,0xE0,0x80,0xE0,0x20,0x20,0xE0,0x00 }, { 0x00,0xE0,0x80,0xE0,0xA0,0xA0,0xE0,0x00 }, { 0x00,0xE0,0x20,0x20,0x20,0x20,0x20,0x00 }, { 0x00,0xE0,0xA0,0xE0,0xA0,0xA0,0xE0,0x00 }, { 0x00,0xE0,0xA0,0xE0,0x20,0x20,0xE0,0x00 } };
char firstDig[10][8] =
{ { 0x00,0x0E,0x0A,0x0A,0x0A,0x0A,0x0E,0x00 }, { 0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x00 },{ 0x00,0x0E,0x02,0x0E,0x08,0x08,0x0E,0x00 }, { 0x00,0x0E,0x02,0x0E,0x02,0x02,0x0E,0x00 },{ 0x00,0x0A,0x0A,0x0A,0x0F,0x02,0x02,0x00 }, { 0x00,0x0E,0x08,0x0E,0x02,0x02,0x0E,0x00 },{ 0x00,0x0E,0x08,0x0E,0x0A,0x0A,0x0E,0x00 }, { 0x00,0x0E,0x02,0x02,0x02,0x02,0x02,0x00 },{ 0x00,0x0E,0x0A,0x0E,0x0A,0x0A,0x0E,0x00 }, { 0x00,0x0E,0x0A,0x0E,0x02,0x02,0x0E,0x00 } };


//蜂鸣器需要的一系列东西
int Eat_length = 2; // the number of notes
char Eat_notes[] = "dg"; // a space represents a rest
int Eat_beats[] = { 1, 2 };
int Die_length = 2; // the number of notes
char Die_notes[] = "gd"; // a space represents a rest
int Die_beats[] = { 2, 1 };
int Pass_length = 4; // the number of notes
char Pass_notes[] = "defb"; // a space represents a rest
int Pass_beats[] = { 1, 1, 1, 2 };
int OK_length = 7; // the number of notes
char OK_notes[] = "cdefgab"; // a space represents a rest
int OK_beats[] = { 1, 1, 1, 1, 1, 1, 2 };
int tempo = 64;

void playTone(int tone, int duration) {
	for (long i = 0; i < duration * 1000L; i += tone * 2) {
		digitalWrite(speakerPin, LOW);
		delayMicroseconds(tone);
		digitalWrite(speakerPin, HIGH);
		delayMicroseconds(tone);
	}
}

void playNote(char note, int duration) {
	char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
	int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };

	for (int i = 0; i < 8; i++) {
		if (names[i] == note) {
			playTone(tones[i], duration);
		}
	}
}

void Score (int nowlength)
{
	int secondOne = nowlength / 10;
	int firstOne = nowlength % 10;
	for (i = 0; i < 8; i++)
	{
		ScoresNow[i] = secondDig[secondOne][i] + firstDig[firstOne][i];
	}
}

//点阵显示函数  
void printByte(char* character)
{
	for (i = 0; i < 8; i++)
	{
		lc.setRow(0, i, character[i]);
	}
}

void printByte2(char* ch)
{
	for (i = 0; i < 8; i++)
	{
		lc2.setRow(0, i, ch[i]);
	}
}

void setup()
{
	pinMode(2, INPUT_PULLUP);
	pinMode(speakerPin, OUTPUT);
	//摇杆的数字输入需要
	Serial.begin(9600);
	lc.shutdown(0, false); //启动时，MAX72XX处于省电模式
	lc.setIntensity(0, 2); //将亮度设置为合适的值(0 - 8)
	lc.clearDisplay(0); //清除显示
	lc2.shutdown(0, false); //启动时，MAX72XX处于省电模式
	lc2.setIntensity(0, 2); //将亮度设置为合适的值(0 - 8)
	lc2.clearDisplay(0); //清除显示
	digitalWrite(speakerPin, HIGH);
}

void GetMap()
{
	for (x = 0; x < 8; x++)
	{
		NowMap[x] = 0;
		for (y = 0; y < 16; y++)
			Map[x][y] = 0;
	}
	for (x = 8; x < 16; x++)
	{
		NowMap[x] = 0;
	}
	//初始化

	for (i = 0; i < BodyLen; i++)
	{
		NowMap[BodyY[i]] += chars[BodyX[i]];
		Map[BodyX[i]][BodyY[i]] = 1;
	}
}

void DrawMap()
{
	if (State == 0)
	{
		printByte(Start);
		printByte2(Two);
	}
	else if (State == 2)
	{
		printByte(Died);
		Score(BodyLen);
		printByte2(ScoresNow);
	}
	else if (State == 3)
	{
		printByte(OK);
		Score(BodyLen);
		printByte2(ScoresNow);
	}
	else if (State == 4)
	{
		printByte(Pause);
		Score(BodyLen);
		printByte2(ScoresNow);
	}
	else if (State == 5)
	{
		printByte(Suc);
		Score(BodyLen);
		printByte2(ScoresNow);
	}
	else
	{
		GetMap();
		if (FoodShow)
		{
			FoodShow = 0;
			NowMap[FoodY] += chars[FoodX];
		}
		else
			FoodShow = 1;
		printByte(NowMap);
		printByte2(NowMap + 8);
	}
}

void GetFood()
{
	//随机出现食物
	BodyLen -= 1;
	GetMap();
	BodyLen += 1;
	while (1)
	{
		FoodX = random(0, 7);
		FoodY = random(0, 15);
		if (Map[FoodX][FoodY] == 0 && !(TarX == FoodX && TarY == FoodY))
			break;
	}
	Map[FoodX][FoodY] = 2;
	//2代表食物
}

int Move()
{
	nowDire = Dire;
	TarX = BodyX[0];
	TarY = BodyY[0];
	if (nowDire == 1)
		TarY -= 1;
	else if (nowDire == 2)
		TarY += 1;
	else if (nowDire == 4)
		TarX -= 1;
	else
		TarX += 1;

	//如果已经碰到墙了或者碰到自己了
	if (TarX < 0 || TarY < 0 || TarX > 7 || TarY > 15 || Map[TarX][TarY] == 1)
	{
		State = 2;
		PlayMusic(Die_notes, Die_beats, Die_length);
		return 1;
	}

	//如果吃到了东西
	if (FoodX == TarX && FoodY == TarY)
	{
		BodyLen += 1;
		if (BodyLen >= 99)
		{
			PlayMusic(OK_notes, OK_beats, OK_length);
			State = 3;
			return 1;
		}
		if (BodyLen % 10 == 0 && BodyLen > AgoLen)
		{
			AgoLen = BodyLen;
			PlayMusic(Pass_notes, Pass_beats, Pass_length);
			State = 5;
		}
		else
			PlayMusic(Eat_notes, Eat_beats, Eat_length);
		GetFood();
	}

	for (i = BodyLen - 1; i > 0; i--)
	{
		BodyX[i] = BodyX[i - 1];
		BodyY[i] = BodyY[i - 1];
	}
	BodyX[0] = TarX;
	BodyY[0] = TarY;
	return 0;
}

void PlayMusic(char* Notes, int * Beats, int length)
{
	for (int i = 0; i < length; i++) {
		if (Notes[i] == ' ') {
			delay(Beats[i] * tempo); // rest
		}
		else {
			playNote(Notes[i], Beats[i] * tempo);
		}
		// pause between notes
		delay(tempo / 2);
	}
}

void loop()
{
	LimitTime = LimitTimeBase - BodyLen / 16;
	nowx = analogRead(A1);
	nowy = analogRead(A0);
	PushZ = digitalRead(2);
	if (PushZ)
		PausePress = 0;
	else
		PausePress += 1;
	if (nowx > 768)
		PushX = 1;
	else if (nowx < 256)
		PushX = -1;
	else
		PushX = 0;
	if (nowy > 768)
		PushY = 1;
	else if (nowy < 256)
		PushY = -1;
	else
		PushY = 0;

	if (PushY > 0 && nowDire != 2)
		Dire = 1;
	else if (PushY < 0 && nowDire != 1)
		Dire = 2;
	else if (PushX > 0 && nowDire != 4)
		Dire = 3;
	else if (PushX < 0 && nowDire != 3)
		Dire = 4;

	if (State == 1)
	{
		if (PushZ == 0 && PausePress > 8)
		{
			State = 4;
			DrawMap();
			PausePress = 0;
		}
		else
		{
			BodyLen -= 1;
			GetMap();
			BodyLen += 1;
			if (p >= LimitTime)
			{
				p = 0;
				Move();
			}
			else
			{
				p = p + 1;
			}
			DrawMap();
		}
	}
	else if (PushZ == 0 && State != 1)
	{
		if (State >= 4 && PausePress > 8)
		{
			State = 1;
			PausePress = 0;
			DrawMap();
		}
		else if (State < 4)
		{
			AgoLen = 0;
			State = 1;
			BodyX[0] = 4;
			BodyY[0] = 2;
			BodyX[1] = 3;
			BodyY[1] = 2;
			BodyX[2] = 2;
			BodyY[2] = 2;
			BodyLen = 3;
			TarX = 2;
			TarY = 2;
			GetFood();
			DrawMap();
			nowDire = 3;
			Dire = 3;
		}
		else
			DrawMap();
	}
	else
	{
		DrawMap();
	}
	delay(40);
}