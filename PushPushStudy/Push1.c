#include<stdio.h>
#include<windows.h>
/**************************************/
#define STAGE_X 30
#define STAGE_Y 8
#define ONE_SEC 40
/**************************************/
enum {HIDDEN, SHOW};
/**************************************/
enum {
    BLACK, // 0
    DARK_BLUE,
    DARK_GREEN,
    DARK_SKY_BLUE,
    DARK_RED,
    DARK_VIOLET,
    DARK_YELLOW,
    GRAY,
    DARK_GRAY,
    BLUE,
    GREEN,
    SKY_BLUE,
    RED,
    VIOLET,
    YELLOW,
    WHITE,
};
/**************************************/
const char stage[] = "040300222n00212n00202222n22230312n21030222n222202n000242n000222n";
/**************************************/
int player_x;
int player_y;
/**************************************/
/*화면의 커서를 보이거나 숨기기*/
void CursorView(char show) 
{
    HANDLE hConsole;
    CONSOLE_CURSOR_INFO ConsoleCursor;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    ConsoleCursor.bVisible = show;
    ConsoleCursor.dwSize = 1;
    SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}
/**************************************/
/*x, y좌표로 커서를 움직이는 함수*/
void gotoxy(int x, int y)
{
    COORD XY = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), XY);
}
/**************************************/
/*콘솔 색깔을 지정하는 함수*/
void SetColor(int color) 
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
/**************************************/
void PlayerDraw(void)
{
    gotoxy(STAGE_X + player_x, STAGE_Y + player_y);
    SetColor(VIOLET);
    printf("*");
}
/**************************************/
void GameDraw(void)
{
    const char *pnt = stage;
    char data;
    int x, y;

    data = *pnt++;
    player_x = data - '0';
    data = *pnt++;
    player_x *= 10;
    player_x += data - '0';
    
    data = *pnt++;
    player_y = data - '0';
    data = *pnt++;
    player_y *= 10;
    player_y += data -'0';

    x = STAGE_X;
    y = STAGE_Y;
    for(;;) {
        data = *pnt++;
        if(data == 0) break;
        if(data == 'n') 
        {
            x = STAGE_X;
            y++;
            continue;    
        }

        gotoxy(x, y);
        switch(data) {
        case '0': printf(" "); break;
        case '1': SetColor(RED); printf("a"); break;
        case '2': SetColor(GREEN); printf("#"); break;
        case '3': SetColor(RED); printf("o"); break;
        case '4': SetColor(YELLOW); printf("@"); break;
        }
        x += 1;
    }
}
/**************************************/
int main()
{
    CursorView(HIDDEN);
    /*콘솔 창의 타이틀 바에 표시*/
    //SetConsoleTitle("PushPush");
    GameDraw();
    PlayerDraw();

    for(;;)
    {
        /*ESC == 종료*/
        if(GetAsyncKeyState(VK_ESCAPE) < 0) break;
        Sleep(17);
    }

    CursorView(SHOW);
    return 0;
}