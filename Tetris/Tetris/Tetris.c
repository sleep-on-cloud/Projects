#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <Windows.h>

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define ESC 27

#define WIDTH 10 //너비
#define HEIGHT 20 //높이

enum {HIDDEN, SHOW};
enum {EMPTY, BRICK, WALL};
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
/* ============================================== */
void CursorView(char show)
{
	HANDLE hConsole;
	CONSOLE_CURSOR_INFO consoleCursor;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	consoleCursor.bVisible = show;
	consoleCursor.dwSize = 1;

	SetConsoleCursorInfo(hConsole, &consoleCursor);
}

void gotoxy(int x, int y)
{
	COORD XY = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), XY);
}

void SetColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
/* ============================================= */

int nCurrentX;
int nCurrentY;
int nCurrentPiece;
int nCurrentRotation;
int gameOver;

int board[HEIGHT + 2][WIDTH + 2]; // [y][x]
char *Tile[] = { " .", "■", "▩" }; //empty, BRICK, WALL
char shape[][4][4] = {
	{
		"0010",  // ■
		"0010",  // ■
		"0010",  // ■
		"0010"   // ■
	},
	{
		"0110",  //
		"0110",  // ■■
		"0000",  // ■■
		"0000"   //
	},
	{
		"0010",  //  ■
		"0110",  // ■■
		"0100",  // ■
		"0000"   //
	},
	{
		"0100",  // ■
		"0110",  // ■■
		"0010",  //  ■
		"0000"   //
	},
	{
		"0010",  //  ■
		"0110",  // ■■
		"0010",  //  ■
		"0000"   //
	},
	{
		"0010",  //
		"0010",  //  ■
		"0110",  //  ■
		"0000"   // ■■
	},
	{
		"0100",  //
		"0100",  // ■
		"0110",  // ■
		"0000"   // ■■
	}
};
/* ============================================= */

void showInit();
void showBoard();
void printPiece(char show);
int getAround(int x, int y, int p, int r);
int moveDown();
int pressKey();
void lineClear();
int Rotate(int px, int py, int r);
/* ============================================= */
int main()
{
	int x, y;
	int timer;

	for (y = 0; y < HEIGHT + 2; y++)
		for (x = 0; x < WIDTH + 2; x++)
			board[y][x] = (x == 0 || y == 0 || x == WIDTH + 1 || y == HEIGHT + 1) ? WALL : EMPTY;

	showInit();
	gameOver = 0;
	srand(time(NULL));
	
	while (!gameOver)
	{
		nCurrentPiece = rand() % (sizeof(shape)/sizeof(shape[0]));
		nCurrentX = WIDTH / 2 - 1;
		nCurrentY = 1;
		nCurrentRotation = 0;

		printPiece(SHOW);
		timer = HEIGHT;

		for (; 1;)
		{
			if (--timer == 0)
			{
				timer = HEIGHT;
				if (moveDown()) break;
			}
			if (pressKey()) break;
			Sleep(1000 / 20);
		}
	}
}

void showInit()
{
	int x, y;
	for (y = 0; y < HEIGHT + 2; y++)
		for (x = 0; x < WIDTH + 2; x++)
		{
			switch (board[y][x])
			{
			case EMPTY:
				SetColor(WHITE);
				gotoxy(x * 2, y);
				break;
			case BRICK:
				SetColor(WHITE);
				gotoxy(x * 2, y);
				break;
			case WALL :
				SetColor(RED);
				gotoxy(x * 2, y);
				break;
			}
			puts(Tile[board[y][x]]);
		}

	SetColor(YELLOW);
	gotoxy(50, 3); printf("TETRIS");
	gotoxy(50, 5); printf("좌우: 이동, 위: 회전, 아래: 내림");
	gotoxy(50, 6); printf("스페이스: 바로 내리기");
	//gotoxy(0, 22);
}
void showBoard()
{
	int x, y;

	for(y = 1; y < HEIGHT + 1; y++)
		for (x = 1; x < WIDTH + 1; x++)
		{
			switch (board[y][x])
			{
			case EMPTY:
				SetColor(WHITE);
				gotoxy(x * 2, y);
				break;
			case BRICK:
				SetColor(WHITE);
				gotoxy(x * 2, y);
				break;
			case WALL:
				SetColor(RED);
				gotoxy(x * 2, y);
				break;
			}
			puts(Tile[board[y][x]]);
		}

}

void printPiece(char show)
{
	int x, y;

	for(y = 0; y < 4; ++y)
		for (x = 0; x < 4; ++x)
		{
			int pi = Rotate(x, y, nCurrentRotation);
			if ((*(*shape[nCurrentPiece] + pi)) == '1')
			{
				gotoxy((nCurrentX + x) * 2, nCurrentY + y);
				puts(Tile[show ? BRICK : EMPTY]);
			}
		}
}

int getAround(int x, int y)
{
	int px, py;
	for(py = 0; py < 4; py++)
		for (px = 0; px < 4; px++)
		{
			int pi = Rotate(px, py, nCurrentRotation);
			int fi = board[y + py][x + px];

			if (y + py >= 0 && y + py < HEIGHT + 2)
			{
				if (x + px >= 0 && x + px < WIDTH + 2)
				{
					if(fi != EMPTY && (*(*shape[nCurrentPiece] + pi)) == '1')
						return !EMPTY;
				}
			}
		}
	return EMPTY;
}
int moveDown()
{
	if (getAround(nCurrentX, nCurrentY + 1) != EMPTY)
	{
		lineClear();
		return 1;
	}
	printPiece(HIDDEN);
	nCurrentY++;
	printPiece(SHOW);
	return 0;
}

int pressKey()
{
	char key;
	int temp;

	if (_kbhit())
	{
		key = _getch();

		if (key == ESC)
		{
			gameOver = 1;
			return 1;
		}
		else if (key == -32)
		{
			key = _getch();
			switch (key)
			{
			case LEFT:
				if (getAround(nCurrentX - 1, nCurrentY) == EMPTY)
				{
					printPiece(HIDDEN);
					nCurrentX--;
					printPiece(SHOW);
				}
				break;
			case RIGHT:
				if (getAround(nCurrentX + 1, nCurrentY) == EMPTY)
				{
					printPiece(HIDDEN);
					nCurrentX++;
					printPiece(SHOW);
				}
				break;
			case UP:
				temp = (nCurrentRotation == 3 ? 0 : nCurrentRotation + 1);
				if (getAround(nCurrentX, nCurrentY) == EMPTY)
				{
					printPiece(HIDDEN);
					nCurrentRotation = temp;
					printPiece(SHOW);
				}
				break;
			case DOWN:
				if (moveDown())
					return 1;
				break;
			}
		}
		else
		{
			switch (key)
			{
			case ' ':
				while (moveDown() == FALSE);
				return 1;
			}
		}
	}
	return 0;
}

void lineClear()
{
	int i, x, y, ty;
	
	for (y = 0; y < 4; ++y)
		for (x = 0; x < 4; ++x)
		{
			int pi = Rotate(x, y, nCurrentRotation);
			if ((*(*shape[nCurrentPiece] + pi)) != '0')
			{
				board[nCurrentY + y][nCurrentX + x] = BRICK;
			}
		}

	for (y = 1; y < HEIGHT + 1; y++)
	{
		for (x = 1; x < WIDTH + 1; x++)
			if (board[y][x] != BRICK) break;

		if (x == WIDTH + 1)
		{
			for (ty = y; ty > 1; ty--)
				for (x = 1; x < WIDTH + 1; x++)
					board[ty][x] = board[ty - 1][x];
			showBoard();
			Sleep(200);
		}
	}
}

int Rotate(int px, int py, int r)
{
	switch (r % 4)
	{
	case 0: return py * 4 + px;
	case 1: return 12 + py - (px * 4);
	case 2: return 15 - (py * 4) - px;
	case 3: return 3 - py + (px * 4);
	}
}