#include <stdio.h>
#include <windows.h>

/* ========================================================================== */

#define SIZE_MAP_X	40
#define SIZE_MAP_Y	24

#define ONE_SEC		60

enum {HIDDEN, SHOW};

enum {
	BLACK,			/*  0 : ��� */
	DARK_BLUE,		/*  1 : ��ο� �Ķ� */
	DARK_GREEN,		/*  2 : ��ο� �ʷ� */
	DARK_SKY_BLUE,	/*  3 : ��ο� �ϴ� */
	DARK_RED,		/*  4 : ��ο� ���� */
	DARK_VIOLET,	/*  5 : ��ο� ���� */
	DARK_YELLOW,	/*  6 : ��ο� ��� */
	GRAY,			/*  7 : ȸ�� */
	DARK_GRAY,		/*  8 : ��ο� ȸ�� */
	BLUE,			/*  9 : �Ķ� */
	GREEN,			/* 10 : �ʷ� */
	SKY_BLUE,		/* 11 : �ϴ� */
	RED,			/* 12 : ���� */
	VIOLET,			/* 13 : ���� */
	YELLOW,			/* 14 : ��� */
	WHITE,			/* 15 : �Ͼ� */
};

/* ========================================================================== */

const char *stage = "040300222n00212n00232222n22200312n21300222n222232n000212n000222n";

/* ========================================================================== */

char map[SIZE_MAP_X][SIZE_MAP_Y];

int stage_x;
int stage_y;

int player_x;
int player_y;

/* ========================================================================== */
/* ȭ���� Ŀ���� ���̰ų� ����� */
void CursorView(char show)
{
    HANDLE hConsole;
    CONSOLE_CURSOR_INFO ConsoleCursor;

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    ConsoleCursor.bVisible = show;
    ConsoleCursor.dwSize = 1;

    SetConsoleCursorInfo(hConsole , &ConsoleCursor);
}

/* ========================================================================== */
/* x, y ��ǥ�� Ŀ���� �����̴� �Լ� */
void gotoxy(int x, int y) /*Ŀ���̵� �Լ� �κ�*/
{
    COORD XY = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), XY);
}

/* ========================================================================== */
/* �ܼ� ������ �����ϴ� �Լ� */
void SetColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

/* ========================================================================== */

void PlayerDraw(void)
{
	gotoxy((stage_x + player_x) * 2, stage_y + player_y);
	SetColor(SKY_BLUE);	printf("��");
}

/* ========================================================================== */

void GameDraw(void)
{
	int loopx, loopy;
	char data;

	for (loopy = 0; loopy < SIZE_MAP_Y; loopy++) {
		for (loopx = 0; loopx < SIZE_MAP_X; loopx++) {
			gotoxy(loopx * 2, loopy);
			data = map[loopy][loopx];
			switch (data) {
			case	'0':	SetColor(BLACK);	printf("  ");	break;	/* ����� */
			case	'1':	SetColor(YELLOW);	printf("��");	break;	/* ������ */
			case	'2':	SetColor(GREEN);	printf("��");	break;	/* ��ó�� */
			case	'3':	SetColor(RED);		printf("��");	break;	/* ������ �ڽ� */
			case	'4':	SetColor(RED);		printf("��");	break;	/* �ϼ��� ������ */
			}
		}
	}
}

/* ========================================================================== */

void GameMapClear(void)
{
	int loopx, loopy;

	for (loopy = 0; loopy < SIZE_MAP_Y; loopy++) {
		for (loopx = 0; loopx < SIZE_MAP_X; loopx++) {
			map[loopy][loopx] = 0;
		}
	}
}

/* ========================================================================== */

void GameStageToMap(const char *pnt)
{
	int x = stage_x;
	int y = stage_y;
	char data;

	GameMapClear();

	for (;;) {
		data = *pnt++;
		if (data == 0) break;	/* �������� �������� ���� üũ */
		if (data == 'n') {	/* �ٹٲ� ó�� */
			x = stage_x;
			y++;
			continue;
		}

		map[y][x] = data;

		x++;
	}
}

/* ========================================================================== */

void GameStageSizeSet(const char *pnt)
{
	int max_x = 0;	/* ���� ū ����ũ�� ã�� */
	int x = 0;	/* ���������� ���� ũ�� */
	int y = 0;	/* ���������� ���� ũ�� */
	char data;

	for (;;) {
		data = *pnt++;
		if (data == 0) break;	/* �������� �������� ���� üũ */
		if (data == 'n') {	/* �ٹٲ� ó�� */
			if (x > max_x)	max_x = x;
			x = 0;
			y++;
			continue;
		}

		x++;
	}

	stage_x = (SIZE_MAP_X - max_x) / 2;
	stage_y = (SIZE_MAP_Y - y) / 2;
}

/* ========================================================================== */

int GameGetPosition(const char *pnt)
{
	int ret;
	char data;

	data = *pnt++;
	ret = data - '0';
	data = *pnt++;
	ret *= 10;
	ret += data - '0';

	return ret;
}

/* ========================================================================== */

void GamePlayerPositionSet(const char *pnt)
{
	player_x = GameGetPosition(pnt); //x��ǥ xx
	player_y = GameGetPosition(pnt + 2); //y��ǥ yy
}

/* ========================================================================== */

void GameStageInit(void)
{
	const char *pnt = stage;

	GamePlayerPositionSet(pnt);
	pnt += 4; //x, y��ǥ �ǳʶ�

	GameStageSizeSet(pnt);
	GameStageToMap(pnt);
}

/* ========================================================================== */

int main(void)
{
	CursorView(HIDDEN);
/* �ܼ� â�� Ÿ��Ʋ �ٿ� ǥ�� */
	SetConsoleTitle("PushPush");

	GameStageInit();
	GameDraw();
	PlayerDraw();

	for (;;) {
		/* Esc�� ������ ������ �����Ѵ�. */
		if (GetAsyncKeyState(VK_ESCAPE) < 0)	break;
		
//�� 1/60�ʾ� ��ٸ���.
		Sleep(17);
	}

	CursorView(SHOW);

	return 0;
}

/* ========================================================================== */