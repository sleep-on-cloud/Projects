#include <stdio.h>
#include <windows.h>
#include <conio.h>

/* ========================================================================== */

#define SIZE_MAP_X	40
#define SIZE_MAP_Y	24

#define ONE_SEC		60

#define ESC         27
#define UP          72
#define DOWN        80
#define LEFT        75
#define RIGHT       77

enum {HIDDEN, SHOW};

enum {
	BLACK,			/*  0 : 까망 */
	DARK_BLUE,		/*  1 : 어두운 파랑 */
	DARK_GREEN,		/*  2 : 어두운 초록 */
	DARK_SKY_BLUE,	/*  3 : 어두운 하늘 */
	DARK_RED,		/*  4 : 어두운 빨강 */
	DARK_VIOLET,	/*  5 : 어두운 보라 */
	DARK_YELLOW,	/*  6 : 어두운 노랑 */
	GRAY,			/*  7 : 회색 */
	DARK_GRAY,		/*  8 : 어두운 회색 */
	BLUE,			/*  9 : 파랑 */
	GREEN,			/* 10 : 초록 */
	SKY_BLUE,		/* 11 : 하늘 */
	RED,			/* 12 : 빨강 */
	VIOLET,			/* 13 : 보라 */
	YELLOW,			/* 14 : 노랑 */
	WHITE,			/* 15 : 하양 */
};

/* ========================================================================== */

const char *stage = "040300222n00212n00232222n22200312n21300222n222232n000212n000222n";

/* ========================================================================== */

char map[SIZE_MAP_X][SIZE_MAP_Y];

int stage_x; //중앙값
int stage_y; //중앙값

int player_x; //캐릭터 좌표
int player_y; //캐릭터 좌표

int exit_flag;

/* ========================================================================== */
/* 화면의 커서를 보이거나 숨기기 */
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
/* x, y 좌표로 커서를 움직이는 함수 */
void gotoxy(int x, int y) /*커서이동 함수 부분*/
{
    COORD XY = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), XY);
}

/* ========================================================================== */
/* 콘솔 색깔을 지정하는 함수 */
void SetColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

/* ========================================================================== */
//맵 구성요소를 그리는것
void MapDraw(int x, int y)
{
    char data;
    gotoxy(x * 2, y);
    data = map[y][x];
	switch (data) {
	case	'0':	SetColor(BLACK);	printf("  ");	break;	/* 빈공간 */
	case	'1':	SetColor(YELLOW);	printf("♀");	break;	/* 목적지 */
	case	'2':	SetColor(GREEN);	printf("▩");	break;	/* 벽처리 */
	case	'3':	SetColor(RED);		printf("♥");	break;	/* 움직일 박스 */
	case	'4':	SetColor(RED);		printf("♀");	break;	/* 완성된 목적지 */
	}
}

/* ========================================================================== */
//플레이어가 이동하면 거기에 원래있던 맵 데이터를 그린다
void PlayerErase(void)
{
    MapDraw(player_x, player_y);
}

/* ========================================================================== */

void PlayerDraw(void)
{
	gotoxy(player_x * 2, player_y);
	SetColor(SKY_BLUE);	printf("★");
}

/* ========================================================================== */
//맵 전체를 그리는것
void GameDraw(void)
{
	int loopx, loopy;

	for (loopy = 0; loopy < SIZE_MAP_Y; loopy++) {
		for (loopx = 0; loopx < SIZE_MAP_X; loopx++) {
			MapDraw(loopx, loopy);
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
		if (data == 0) break;	/* 스테이지 데이터의 끝을 체크 */
		if (data == 'n') {	/* 줄바꿈 처리 */
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
	int max_x = 0;	/* 가장 큰 가로크기 찾기 */
	int x = 0;	/* 스테이지의 가로 크기 */
	int y = 0;	/* 스테이지의 세로 크기 */
	char data;

	for (;;) {
		data = *pnt++;
		if (data == 0) break;	/* 스테이지 데이터의 끝을 체크 */
		if (data == 'n') {	/* 줄바꿈 처리 */
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
	player_x = GameGetPosition(pnt); //x좌표 xx
	player_y = GameGetPosition(pnt + 2); //y좌표 yy
}

/* ========================================================================== */

void GameStageInit(void)
{
	const char *pnt = stage;

	GamePlayerPositionSet(pnt);
	pnt += 4; //x, y좌표 건너뜀

	GameStageSizeSet(pnt);

    player_x += stage_x;
    player_y += stage_y;

	GameStageToMap(pnt);
}

/* ========================================================================== */

void PlayerMoveAction()
{
    int dx, dy;
    char key;
    char data;

    key = getch();

    if(key == ESC)
    {
        exit_flag = 1;
        return;
    }

    if(key == -32) key = getch();

    switch(key) {
    case    UP: dx = 0; dy = -1; break;
    case  DOWN: dx = 0; dy = 1; break;
    case  LEFT: dx = -1; dy = 0; break;
    case RIGHT: dx = 1; dy = 0; break;
    }

    data = map[player_y + dy][player_x + dx];
    if(data != '2')
    {
        PlayerErase();
        player_x += dx;
        player_y += dy;
        PlayerDraw();
    }
}
//플레이어 원래좌표의 맵 구성요소를 그리고 이동한후 플레이어를 그린다
/* ========================================================================== */

int main(void)
{
    exit_flag = 0;

	CursorView(HIDDEN);
/* 콘솔 창의 타이틀 바에 표시 */
	SetConsoleTitle("PushPush");

	GameStageInit();
	GameDraw();
	PlayerDraw();

	for (;;) {
		PlayerMoveAction();

        if(exit_flag != 0) break;
	}
    
    gotoxy(0, 24);
	CursorView(SHOW);

	return 0;
}

/* ========================================================================== */