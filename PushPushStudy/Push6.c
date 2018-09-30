#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <string.h>

/* ========================================================================== */

#define SIZE_MAP_X	30
#define SIZE_MAP_Y	24
#define STAGE_MAX   10

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

const char *stage_table[] = {
"040300222n00232n00212222n22200132n23100222n222212n000232n000222n",
"010122222n20002n201120222n201020232n222022232n022000032n020002002n020002222n022222n",
"020302222222n0200000222n2212220002n2000100102n2033201022n223320002n022222222n",
"010202222n22002n20102n221022n220102n231002n233432n222222n",
"0201022222n0200022n0201002n22202022n23202002n23100202n23000102n22222222n",
"08010002222222n0022002002n0020002002n0021010102n0020122002n2220102022n233333002n2222222222",
"0803000222222n022200002n2230122022n2331010002n2330101022n222222002n000002222",
"08070222222222n0200220002n0200010002n0210222012n0202333202n22023332022n20100100102n20000020002n22222222222n",
"010300222222n00200002n22211102n20013302n20133322n2222002n0002222n",
"0804022220022222n220020020002n201022221002n200133330102n220000200022n02222222222n",
"05010022222n2220002n20013022n20031302n22204102n00200022n0022222n",
"0306002222n002332n0220322n0200132n22010022n20021102n20000002n22222222n",
"010322222222n20020002n20133102n20134022n20133102n20020002n22222222n",
"04050222222n22000022n20101102n23333332n20110102n22200222n002222n",
"060700222222n0020000222n0020100002n2220102202n2333010002n2333121022n2222020102n0002000002n0002222222n",
"0305222222n200002n2011122n200233222n220033102n020000002n022222222n",
"07070022222222n0020002302n0220013332n0200102432n2202212022n2000100102n2000200002n2222222002n0000002222n",
"050602222222n02333302n2223331222n2001210102n2011002102n2000020002n2222000222n00022222n",
"04062222222n2331332n2332332n2011102n2001002n2011102n2002002n2222222n",
"070722222222n20000002n20211002n20333202n223331022n020220102n021001002n020020002n022222222n",
};
/* ========================================================================== */

char map[SIZE_MAP_X][SIZE_MAP_Y];

int stage; //현재 스테이지
int stage_clear_flag; //스테이지를 초기화하는 판단요소

int stage_x; //중앙값
int stage_y; //중앙값

int player_x; //캐릭터 좌표
int player_y; //캐릭터 좌표

int player_move_count; //움직인 횟수
int player_push_count; //박스를 민 횟수

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

/* x, y 좌표로 커서를 움직이는 함수 */
void gotoxy(int x, int y) /*커서이동 함수 부분*/
{
    COORD XY = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), XY);
}

/* 콘솔 색깔을 지정하는 함수 */
void SetColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

/* ========================================================================== */
void GameStateDraw(); //게임에 관한 정보출력
void TitleDraw(); //제목출력
void StageInfoDraw(); //스테이지 정보출력
void PlayerMoveCountDraw(); //움직인 횟수 출력
void PlayerPushCountDraw(); //박스를 민 횟수를 출력
void GameHelpDraw(); //도움말 출력

void GameDraw(); //맵 전체를 순회하며 그리는것
void MapDraw(int x, int y); // 맵의 각 구성요소를 그리는것

void GameStageInit(); //스테이지 초기화
void GameStageToMap(const char *pnt); //맵배열 내용을 작성
void GameMapClear(); //맵배열에 0이라는 초기값 부여
void GameStageSizeSet(const char *pnt); //스테이지 사이즈 측정
void GamePlayerPositionSet(const char *pnt); //플레이어 좌표값 초기화
int GameGetPosition(const char *pnt); //플레이어 좌표 계산

int StageClearCheck(); //스테이지를 클리어 했는지 검사
void PlayerMoveAction(); //플레이어 움직임 제어

void PlayerDraw(); //플레이어를 그리는것
void PlayerErase(); //플레이어가 이동하면 거기에 원래있던 맵 데이터를 그린다

/*********************************************************************************/

int main()
{
    exit_flag = 0;
    stage = 0;
    stage_clear_flag = 1;

	CursorView(HIDDEN);
/* 콘솔 창의 타이틀 바에 표시 */
	SetConsoleTitle("PushPush");

	

	for (;;) {
		if(stage_clear_flag != 0)
        {
            system("cls");
            GameStageInit();
	        GameDraw();
	        PlayerDraw();
            GameStateDraw();
        }
        PlayerMoveAction();

        if(exit_flag != 0) break;
	}
    
    gotoxy(0, 24);
	CursorView(SHOW);

	return 0;
}

/*********************************************************************************/

//게임에 관한 정보출력
void GameStateDraw()
{
    TitleDraw();
    StageInfoDraw();
    PlayerMoveCountDraw();
    PlayerPushCountDraw();
    GameHelpDraw();
}

//제목출력
void TitleDraw()
{
    int x = SIZE_MAP_X * 2;
    int y = 0;
    SetColor(DARK_YELLOW);
    gotoxy(x, y + 0);   printf("┏ ━ ━ ━ ━ ━ ━ ━ ━ ┓");
    gotoxy(x, y + 1);   printf("┃ P U S H  P U S H┃");
    gotoxy(x, y + 2);   printf("┗ ━ ━ ━ ━ ━ ━ ━ ━ ┛");

    SetColor(YELLOW);
    gotoxy(x + 2, y + 1);   printf("P U S H  P U S H");
}

//스테이지 정보출력
void StageInfoDraw()
{
    int x = SIZE_MAP_X * 2;
    int y = 4;
    SetColor(RED);
    gotoxy(x + 2, y); printf("Stage: %d", stage + 1);
}

//움직인 횟수 출력
void PlayerMoveCountDraw()
{
    int x = SIZE_MAP_X * 2;
    int y = 6;
    SetColor(VIOLET);
    gotoxy(x + 2, y); printf("Move: %d", player_move_count);
}

//박스를 민 횟수를 출력
void PlayerPushCountDraw()
{
    int x = SIZE_MAP_X * 2;
    int y = 8;
    SetColor(SKY_BLUE);
    gotoxy(x + 2, y); printf("Push: %d", player_push_count);
}
//도움말 출력
void GameHelpDraw()
{
    int x = SIZE_MAP_X * 2;
	int y = 10;

	SetColor(GRAY);
	gotoxy(x + 2, y++);	printf("       UP");
	gotoxy(x + 2, y++);	printf("       ↑");
	gotoxy(x + 2, y++);	printf("LEFT ←  → RIGHT");
	gotoxy(x + 2, y++);	printf("       ↓");
	gotoxy(x + 2, y++);	printf("      DOWN");
}

/* ========================================================================== */

/* ========================================================================== */

//맵 전체를 순회하며 그리는것
void GameDraw()
{
	int loopx, loopy;

	for (loopy = 0; loopy < SIZE_MAP_Y; loopy++) {
		for (loopx = 0; loopx < SIZE_MAP_X; loopx++) {
			MapDraw(loopx, loopy);
		}
	}
}

//맵의 각 구성요소를 그리는것
void MapDraw(int x, int y)
{
    char data;
    gotoxy(x * 2, y);
    data = map[y][x];
	switch (data) {
	case	'0':	SetColor(BLACK);	printf("  ");	break;	/* 빈공간 */
	case	'1':	SetColor(RED);		printf("♥");	break;	/* 움직일 박스 */
	case	'2':	SetColor(GREEN);	printf("▩");	break;	/* 벽처리 */
	case	'3':	SetColor(YELLOW);	printf("♀");	break;	/* 목적지 */
	case	'4':	SetColor(RED);		printf("♀");	break;	/* 완성된 목적지 */
	}
}

/* ========================================================================== */


/* ========================================================================== */

//스테이지 초기화
void GameStageInit()
{
	const char *pnt = stage_table[stage];

	GamePlayerPositionSet(pnt);
	pnt += 4; //x, y좌표 건너뜀

	GameStageSizeSet(pnt);

    player_x += stage_x;
    player_y += stage_y;

	GameStageToMap(pnt);

    stage_clear_flag = 0;
    player_move_count = 0;
    player_push_count = 0;
}

//맵배열 내용을 작성
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

//맵배열에 0이라는 초기값 부여
void GameMapClear()
{
	int loopx, loopy;

	for (loopy = 0; loopy < SIZE_MAP_Y; loopy++) {
		for (loopx = 0; loopx < SIZE_MAP_X; loopx++) {
			map[loopy][loopx] = 0;
		}
	}
}

//스테이지 사이즈 측정
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

//플레이어 좌표값 초기화
void GamePlayerPositionSet(const char *pnt)
{
	player_x = GameGetPosition(pnt); //x좌표 xx
	player_y = GameGetPosition(pnt + 2); //y좌표 yy
}

//플레이어 좌표 계산
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


/* ========================================================================== */

//스테이지를 클리어 했는지 검사
int StageClearCheck()
{
    char data;
    int loopx, loopy;
    for(loopy = 0; loopy < SIZE_MAP_Y; loopy++)
        for(loopx = 0; loopx < SIZE_MAP_X; loopx++)
        {
            data = map[loopy][loopx];
            if(data == '3') return 0;
        }
    return 1;
}

//플레이어 움직임 제어
void PlayerMoveAction()
{
    char* Stage_clear1 = "S T A G E  C L E A R";
    char* Stage_clear2 = "Press space key to next stage.";

    int dx = 0, dy = 0;
    char key;
    char data;
    char nextData;
    int move_flag = 0;

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

    if((dx == 0) && (dy == 0)) return; //예외값 처리

    data = map[player_y + dy][player_x + dx];
    nextData = map[player_y + dy + dy][player_x + dx + dx];
    
    if(data == '1')
    {
        if(nextData == '0')
        {
            player_push_count++;
            PlayerPushCountDraw();
            map[player_y + dy +dy][player_x + dx + dx] = '1';
            map[player_y + dy][player_x + dx] = '0';
            MapDraw(player_x + dx + dx, player_y + dy + dy);
            move_flag = 1;
        }
        else if(nextData == '3')
        {
            player_push_count++;
            PlayerPushCountDraw();
            map[player_y + dy + dy][player_x + dx + dx] = '4';
            map[player_y + dy][player_x + dx] = '0';
            MapDraw(player_x + dx + dx, player_y + dy + dy);
            move_flag = 1;
        }
    }
    else if(data == '4')
    {
        if(nextData == '0')
        {
            player_push_count++;
            PlayerPushCountDraw();
            map[player_y + dy +dy][player_x + dx + dx] = '1';
            map[player_y + dy][player_x + dx] = '3';
            MapDraw(player_x + dx + dx, player_y + dy + dy);
            move_flag = 1;
        }
        else if(nextData == '3')
        {
            player_push_count++;
            PlayerPushCountDraw();
            map[player_y + dy +dy][player_x + dx + dx] = '4';
            map[player_y + dy][player_x + dx] = '3';
            MapDraw(player_x + dx + dx, player_y + dy + dy);
            move_flag = 1;
        }
    }
    else
    {
        if(data != '2') move_flag = 1;
    }

    if(move_flag != 0) //움직임을 처리
    {
        player_move_count++;
        PlayerMoveCountDraw();
        PlayerErase();
        player_x += dx;
        player_y += dy;
        PlayerDraw();
        if(StageClearCheck() != 0) // 스테이지 클리어를 확인
        {
            SetColor(YELLOW);
            gotoxy((SIZE_MAP_X * 2 - strlen(Stage_clear1)) / 2, 4);
            printf("%s", Stage_clear1);
            gotoxy((SIZE_MAP_X * 2 - strlen(Stage_clear2)) / 2, 6);
            printf("%s", Stage_clear2);
            for(;;)
                if((key = getch()) == ' ') break;

            if(++stage >= STAGE_MAX)
                stage = 0;
            stage_clear_flag = 1;
        }
    }
    
}

/* ========================================================================== */


/* ========================================================================== */

//플레이어를 그리는것
void PlayerDraw()
{
	gotoxy(player_x * 2, player_y);
	SetColor(SKY_BLUE);	printf("★");
}

//플레이어가 이동하면 거기에 원래있던 맵 데이터를 그린다
void PlayerErase()
{
    MapDraw(player_x, player_y);
}

/* ========================================================================== */