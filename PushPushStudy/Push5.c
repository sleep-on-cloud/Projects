#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <string.h>

/* ========================================================================== */

#define SIZE_MAP_X	40
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

const char *stage_table[] = {
"040300222n00212n00232222n22200312n21300222n222232n000212n000222n",
"010122222n20002n203320222n203020212n222022212n022000012n020002002n020002222n022222n",
"020302222222n0200000222n2232220002n2000300302n2011203022n221120002n022222222n",
"010202222n22002n20302n223022n220302n213002n211412n222222n",
"0201022222n0200022n0203002n22202022n21202002n21300202n21000302n22222222n",
"08010002222222n0022002002n0020002002n0023030302n0020322002n2220302022n211111002n2222222222",
"0803000222222n022200002n2210322022n2113030002n2110303022n222222002n000002222",
"08070222222222n0200220002n0200030002n0230222032n0202111202n22021112022n20300300302n20000020002n22222222222n",
"010300222222n00200002n22233302n20031102n20311122n2222002n0002222n",
"0804022220022222n220020020002n203022223002n200311110302n220000200022n02222222222n",
};
/* ========================================================================== */

char map[SIZE_MAP_X][SIZE_MAP_Y];
int stage; //���� ��������
int stage_clear_flag; //���������� �ʱ�ȭ�ϴ� �Ǵܿ��

int stage_x; //�߾Ӱ�
int stage_y; //�߾Ӱ�

int player_x; //ĳ���� ��ǥ
int player_y; //ĳ���� ��ǥ

int exit_flag;

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
//�� ������Ҹ� �׸��°�
void MapDraw(int x, int y)
{
    char data;
    gotoxy(x * 2, y);
    data = map[y][x];
	switch (data) {
	case	'0':	SetColor(BLACK);	printf("  ");	break;	/* ����� */
	case	'1':	SetColor(YELLOW);	printf("��");	break;	/* ������ */
	case	'2':	SetColor(GREEN);	printf("��");	break;	/* ��ó�� */
	case	'3':	SetColor(RED);		printf("��");	break;	/* ������ �ڽ� */
	case	'4':	SetColor(RED);		printf("��");	break;	/* �ϼ��� ������ */
	}
}

/* ========================================================================== */
//�÷��̾ �̵��ϸ� �ű⿡ �����ִ� �� �����͸� �׸���
void PlayerErase(void)
{
    MapDraw(player_x, player_y);
}

/* ========================================================================== */

void PlayerDraw(void)
{
	gotoxy(player_x * 2, player_y);
	SetColor(SKY_BLUE);	printf("��");
}

/* ========================================================================== */
//�� ��ü�� �׸��°�
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
	const char *pnt = stage_table[stage];

	GamePlayerPositionSet(pnt);
	pnt += 4; //x, y��ǥ �ǳʶ�

	GameStageSizeSet(pnt);

    player_x += stage_x;
    player_y += stage_y;

	GameStageToMap(pnt);

    stage_clear_flag = 0;
}

/* ========================================================================== */
//�������� �� �ڽ��� �ִ��� ������ �˻�
int StageClearCheck(void)
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

/* ========================================================================== */
void PlayerMoveAction()
{
    char* Stage_clear1 = "S T A G E  C L E A R";
    char* Stage_clear2 = "Press space key to next stage.";

    int dx, dy;
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

    data = map[player_y + dy][player_x + dx];
    nextData = map[player_y + dy + dy][player_x + dx + dx];
    
    if(data == '3')
    {
        if(nextData == '0')
        {
            map[player_y + dy +dy][player_x + dx + dx] = '3';
            map[player_y + dy][player_x + dx] = '0';
            MapDraw(player_x + dx + dx, player_y + dy + dy);
            move_flag = 1;
        }
        else if(nextData == '1')
        {
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
            map[player_y + dy +dy][player_x + dx + dx] = '3';
            map[player_y + dy][player_x + dx] = '1';
            MapDraw(player_x + dx + dx, player_y + dy + dy);
            move_flag = 1;
        }
        else if(nextData == '1')
        {
            map[player_y + dy +dy][player_x + dx + dx] = '4';
            map[player_y + dy][player_x + dx] = '1';
            MapDraw(player_x + dx + dx, player_y + dy + dy);
            move_flag = 1;
        }
    }
    else
    {
        if(data != '2') move_flag = 1;
    }

    if(move_flag != 0) //�������� ó��
    {
        PlayerErase();
        player_x += dx;
        player_y += dy;
        PlayerDraw();
        if(StageClearCheck() != 0) // �������� Ŭ��� Ȯ��
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
//�÷��̾� ������ǥ�� �� ������Ҹ� �׸��� �̵����� �÷��̾ �׸���
/* ========================================================================== */

int main(void)
{
    exit_flag = 0;
    stage = 0;
    stage_clear_flag = 1;

	CursorView(HIDDEN);
/* �ܼ� â�� Ÿ��Ʋ �ٿ� ǥ�� */
	SetConsoleTitle("PushPush");

	

	for (;;) {
		if(stage_clear_flag != 0)
        {
            system("cls");
            GameStageInit();
	        GameDraw();
	        PlayerDraw();
        }
        PlayerMoveAction();

        if(exit_flag != 0) break;
	}
    
    gotoxy(0, 24);
	CursorView(SHOW);

	return 0;
}

/* ========================================================================== */