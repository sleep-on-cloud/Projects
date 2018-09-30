#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <string.h>

/* ========================================================================== */

#define SIZE_MAP_X 30
#define SIZE_MAP_Y 24
#define STAGE_MAX 10

#define UNDO_MAX 10

#define ONE_SEC 60

#define ESC 27
#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77
#define BACK_SPACE 8
#define RESTART 114

/* ========================================================================== */

enum
{
    HIDDEN,
    SHOW
};

enum
{
    BLACK,         /*  0 : ��� */
    DARK_BLUE,     /*  1 : ��ο� �Ķ� */
    DARK_GREEN,    /*  2 : ��ο� �ʷ� */
    DARK_SKY_BLUE, /*  3 : ��ο� �ϴ� */
    DARK_RED,      /*  4 : ��ο� ���� */
    DARK_VIOLET,   /*  5 : ��ο� ���� */
    DARK_YELLOW,   /*  6 : ��ο� ��� */
    GRAY,          /*  7 : ȸ�� */
    DARK_GRAY,     /*  8 : ��ο� ȸ�� */
    BLUE,          /*  9 : �Ķ� */
    GREEN,         /* 10 : �ʷ� */
    SKY_BLUE,      /* 11 : �ϴ� */
    RED,           /* 12 : ���� */
    VIOLET,        /* 13 : ���� */
    YELLOW,        /* 14 : ��� */
    WHITE,         /* 15 : �Ͼ� */
};

/* ========================================================================== */

//��ұ���� ������ ����ü: �����̿�
typedef struct
{
    char map[SIZE_MAP_Y][SIZE_MAP_X];
    int player_x;
    int player_y;
    int player_move_count;
    int player_push_count;
} UNDO;

UNDO undo[UNDO_MAX];
int undo_count;

/* ========================================================================== */
/* ========================================================================== */
//	'0':BLACK;	"  "	/* ����� */
//	'1':RED;	"��"	/* ������ �ڽ� */
//	'2':GREEN;	"��"	/* ��ó�� */
//	'3':YELLOW;	"��"	/* ������ */
//	'4':RED;	"��"	/* �ϼ��� ������ */
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
    "0105000222222n000203332n222233332n20022210222n20101001102n20010100002n20002220002n22222022222n",
    "070722222222n20000002n20211002n20333202n223331022n020220102n021001002n020020002n022222222n",
    "08030022222n2220002222n2000101002n2010001002n2221122222n00200332n00233332n00222222n",
    "010800000222222n02222230002n02002332202n02001330002n02002032022n22202212002n20100001102n20212002002n20002222222n22222n",
    "040522222222n23333332n20010202n201020122n221010102n020000002n022222222n",
    "0203002222222222n222000300002n200022122002n200130303122n22012212202n02000030002n02222222222n",
    "0701000222222n222230002n200111002n232232232n200010002n200132022n22220002n00022222n",
    "04060222222n0230332n0230132n22200122n20100102n20212202n20000002n22222222n",
    "0504222222222n200022002n202010102n200432002n220230322n2212224222n2000000002n2000220202n2222220002n0000022222n",
    "050100022222n00020002n00021112n22220002n200032122n201313032n200232322n22222222n",
    "0908222222222n200000002n200101012n220212202n0203303322n0223303302n00202212022n00210101002n00200000002n00222222222n",
    "07060000022222n0222220002n0203301202n0202340002n2204321002n2010010022n200020002n222222222n",
    "040200022222n0222000222n2200010102n20022022022n20132310002n20232420002n20133300222n222120222n0020002n0022222n",
    "02012222222n20020022222n20110010002n20023221202n22123330002n22033322122n20022322002n20010010002n20020002002n22222222222n",
    "050100022222n00020002n00020102n00021312n022231322n22031313222n20013131002n20000300002n22222222222n",
    "06032222222n200310222n203131002n241313002n203131022n20031002n22222222n",
    "030222222222222n20000200002n20101111102n20000000002n22222022222n00023002n00023002n00023332n00023002n00022222n",
    "0904000022222n000220002222n000203340102n222202320002n200003432022n20212212202n20000010102n22002000222n022222222n",
    "0506222222n200002n201002222n201433402n204334102n222200102n000200002n000222222n",
    "010300022222n222230022n201313002n201202102n201303002n222212102n002303002n002222222n",
    "1002222222222222n200003330102n201114440102n200003330102n222222222222n",
    "02042222222222n2200000002n2000212102n2011003132n2002223332n2222222222n",
    "040302222n020022222n221022002n200101002n200022102n2223220222n0233310102n0223300002n0022222222n",
    "03032222222n2303032n2011102n2310132n2011102n2303032n2222222n",
    "0507000222222222n000200020002n000200000002n222224222022n20003330002n202024222122n201000010002n222220020002n000022222222n",
    "080500002222n222220022222n200001000102n200121220002n222023430222n0020333002n0022021222n00020002n00022222n",
    "080400022222222n22220000302n20010101302n20032222322n2013101002n2003002222n2222222n",
    "08040022222n0020002n22213122222n20003010002n20221220002n20003022222n2220302n0020002n0022222n",
    "0301022222n02000222222n02023340002n02033320002n22122010102n20002122222n200010002n222220202n000020002n000022222n",
    "04010022222n222000222n200101002n204343402n203101302n204343402n200101002n222222002n000002222n",
    "05040222222222n0200020002n0201111102n2201010102n2010000002n20102222022n20023333302n22003333302n02222222222n",
    "10080002222222n2222000002n2000322202n20202000022n20201012302n20200400202n20321010202n2200002020222n0202223000002n0200000220002n0222222222222n",
    "080322222n2000222222n2010330102n2210331102n0200330102n0222222222n",
    "10080222222222n0200022002222n0201000000002n0221222022002n0200220402022n020133333302n220222030202n200000122212n200020000102n222221202222n000020002n000022222n",
    "0208222222222222n233302000002n233002022002n233000002002n233002012202n233302101002n222222001102n022001011002n020011100202n022010220002n002000000002n002222222222n",
    "10022222202222222n2000222002002n2010000010002n2202122322002n0200333430102n0201202320202n0220000100002n0020022222222n002222n",
    "01102222222222222n2001010134332n2010101043332n2001010134332n2010101043332n2001010134332n2010101043332n2001010134332n2010101043332n2001010134332n2010101043332n2222222222222n",
    "0102000002222n2222220022222n2010000100102n2122201020202n2002002010002n2012000020222n200102120002n233333333302n222222220002n000000022222n",
    "06040022222n0020002n0020102n222030222n200040002n204444402n200040002n222141222n0020302n0020402n0020302n0022222n",
};
/* ========================================================================== */

char map[SIZE_MAP_Y][SIZE_MAP_X];

int stage;            //���� ��������
int stage_clear_flag; //���������� �ʱ�ȭ�ϴ� �Ǵܿ��

int stage_x; //�߾Ӱ�
int stage_y; //�߾Ӱ�

int player_x; //ĳ���� ��ǥ
int player_y; //ĳ���� ��ǥ

int player_move_count; //������ Ƚ��
int player_push_count; //�ڽ��� �� Ƚ��

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

    SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

/* x, y ��ǥ�� Ŀ���� �����̴� �Լ� */
void gotoxy(int x, int y) /*Ŀ���̵� �Լ� �κ�*/
{
    COORD XY = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), XY);
}

/* �ܼ� ������ �����ϴ� �Լ� */
void SetColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

/* ========================================================================== */
void GameStateDraw();       //���ӿ� ���� �������
void TitleDraw();           //�������
void StageInfoDraw();       //�������� �������
void PlayerMoveCountDraw(); //������ Ƚ�� ���
void PlayerPushCountDraw(); //�ڽ��� �� Ƚ���� ���
void GameHelpDraw();        //���� ���

void GameDraw();            //�� ��ü�� ��ȸ�ϸ� �׸��°�
void MapDraw(int x, int y); // ���� �� ������Ҹ� �׸��°�

void GameStageInit();                        //�������� �ʱ�ȭ
void GameStageToMap(const char *pnt);        //�ʹ迭 ������ �ۼ�
void GameMapClear();                         //�ʹ迭�� 0�̶�� �ʱⰪ �ο�
void GameStageSizeSet(const char *pnt);      //�������� ������ ����
void GamePlayerPositionSet(const char *pnt); //�÷��̾� ��ǥ�� �ʱ�ȭ
int GameGetPosition(const char *pnt);        //�÷��̾� ��ǥ ���

int StageClearCheck();   //���������� Ŭ���� �ߴ��� �˻�
void PlayerMoveAction(); //�÷��̾� ������ ����

void PlayerDraw();  //�÷��̾ �׸��°�
void PlayerErase(); //�÷��̾ �̵��ϸ� �ű⿡ �����ִ� �� �����͸� �׸���

void UndoSave(); //����Ҷ��� �� ����
void GameUndo(); //��ұ�� ����
/*********************************************************************************/

int main()
{
    exit_flag = 0;
    stage = 0;
    stage_clear_flag = 1;

    CursorView(HIDDEN);
    /* �ܼ� â�� Ÿ��Ʋ �ٿ� ǥ�� */
    SetConsoleTitle("PushPush");

    for (;;)
    {
        if (stage_clear_flag != 0)
        {
            system("cls");
            GameStageInit();
            GameDraw();
            PlayerDraw();
            GameStateDraw();
        }
        PlayerMoveAction();

        if (exit_flag != 0)
            break;
    }

    gotoxy(0, 24);
    CursorView(SHOW);

    return 0;
}

/*********************************************************************************/

//���ӿ� ���� �������
void GameStateDraw()
{
    TitleDraw();
    StageInfoDraw();
    PlayerMoveCountDraw();
    PlayerPushCountDraw();
    GameHelpDraw();
}

//�������
void TitleDraw()
{
    int x = SIZE_MAP_X * 2;
    int y = 0;
    SetColor(DARK_YELLOW);
    gotoxy(x, y + 0);
    printf("�� �� �� �� �� �� �� �� �� ��");
    gotoxy(x, y + 1);
    printf("�� P U S H  P U S H��");
    gotoxy(x, y + 2);
    printf("�� �� �� �� �� �� �� �� �� ��");
}

//�������� �������
void StageInfoDraw()
{
    int x = SIZE_MAP_X * 2;
    int y = 4;
    SetColor(RED);
    gotoxy(x + 2, y);
    printf("STAGE: %d", stage + 1);
}

//������ Ƚ�� ���
void PlayerMoveCountDraw()
{
    int x = SIZE_MAP_X * 2;
    int y = 6;
    SetColor(VIOLET);
    gotoxy(x + 2, y);
    printf("MOVE: %d", player_move_count);
}

//�ڽ��� �� Ƚ���� ���
void PlayerPushCountDraw()
{
    int x = SIZE_MAP_X * 2;
    int y = 8;
    SetColor(SKY_BLUE);
    gotoxy(x + 2, y);
    printf("PUSH: %d", player_push_count);
}
//���� ���
void GameHelpDraw()
{
    int x = SIZE_MAP_X * 2;
    int y = 10;

    SetColor(GRAY);
    gotoxy(x + 2, y++);
    printf("       UP");
    gotoxy(x + 2, y++);
    printf("       ��");
    gotoxy(x + 2, y++);
    printf("LEFT ��  �� RIGHT");
    gotoxy(x + 2, y++);
    printf("       ��");
    gotoxy(x + 2, y++);
    printf("      DOWN");
    gotoxy(x + 2, y++);
    printf("<ESC>: EXIT");
    gotoxy(x + 2, y++);
    printf("<BS>: UNDO");
    gotoxy(x + 2, y++);
    printf("<R>: RESTART");
}

/* ========================================================================== */

/* ========================================================================== */

//�� ��ü�� ��ȸ�ϸ� �׸��°�
void GameDraw()
{
    int loopx, loopy;

    for (loopy = 0; loopy < SIZE_MAP_Y; loopy++)
    {
        for (loopx = 0; loopx < SIZE_MAP_X; loopx++)
        {
            MapDraw(loopx, loopy);
        }
    }
}

//���� �� ������Ҹ� �׸��°�
void MapDraw(int x, int y)
{
    char data;
    gotoxy(x * 2, y);
    data = map[y][x];
    switch (data)
    {
    case '0':
        SetColor(BLACK);
        printf("  ");
        break; /* ����� */
    case '1':
        SetColor(RED);
        printf("��");
        break; /* ������ �ڽ� */
    case '2':
        SetColor(GREEN);
        printf("��");
        break; /* ��ó�� */
    case '3':
        SetColor(YELLOW);
        printf("��");
        break; /* ������ */
    case '4':
        SetColor(RED);
        printf("��");
        break; /* �ϼ��� ������ */
    }
}

/* ========================================================================== */

/* ========================================================================== */

//�������� �ʱ�ȭ
void GameStageInit()
{
    const char *pnt = stage_table[stage];

    GamePlayerPositionSet(pnt);
    pnt += 4; //x, y��ǥ �ǳʶ�

    GameStageSizeSet(pnt);

    player_x += stage_x;
    player_y += stage_y;

    GameStageToMap(pnt);

    stage_clear_flag = 0;
    player_move_count = 0;
    player_push_count = 0;
    undo_count = 0;
}

//�ʹ迭 ������ �ۼ�
void GameStageToMap(const char *pnt)
{
    int x = stage_x;
    int y = stage_y;
    char data;

    GameMapClear();

    for (;;)
    {
        data = *pnt++;
        if (data == 0)
            break; /* �������� �������� ���� üũ */
        if (data == 'n')
        { /* �ٹٲ� ó�� */
            x = stage_x;
            y++;
            continue;
        }

        map[y][x] = data;

        x++;
    }
}

//�ʹ迭�� 0�̶�� �ʱⰪ �ο�
void GameMapClear()
{
    int loopx, loopy;

    for (loopy = 0; loopy < SIZE_MAP_Y; loopy++)
    {
        for (loopx = 0; loopx < SIZE_MAP_X; loopx++)
        {
            map[loopy][loopx] = 0;
        }
    }
}

//�������� ������ ����
void GameStageSizeSet(const char *pnt)
{
    int max_x = 0; /* ���� ū ����ũ�� ã�� */
    int x = 0;     /* ���������� ���� ũ�� */
    int y = 0;     /* ���������� ���� ũ�� */
    char data;

    for (;;)
    {
        data = *pnt++;
        if (data == 0)
            break; /* �������� �������� ���� üũ */
        if (data == 'n')
        { /* �ٹٲ� ó�� */
            if (x > max_x)
                max_x = x;
            x = 0;
            y++;
            continue;
        }

        x++;
    }

    stage_x = (SIZE_MAP_X - max_x) / 2;
    stage_y = (SIZE_MAP_Y - y) / 2;
}

//�÷��̾� ��ǥ�� �ʱ�ȭ
void GamePlayerPositionSet(const char *pnt)
{
    player_x = GameGetPosition(pnt);     //x��ǥ xx
    player_y = GameGetPosition(pnt + 2); //y��ǥ yy
}

//�÷��̾� ��ǥ ���
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

//���������� Ŭ���� �ߴ��� �˻�
int StageClearCheck()
{
    char data;
    int loopx, loopy;
    for (loopy = 0; loopy < SIZE_MAP_Y; loopy++)
        for (loopx = 0; loopx < SIZE_MAP_X; loopx++)
        {
            data = map[loopy][loopx];
            if (data == '3')
                return 0;
        }
    return 1;
}

//�÷��̾� ������ ����
void PlayerMoveAction()
{
    char *Stage_clear1 = "S T A G E  C L E A R";
    char *Stage_clear2 = "Press space key to next stage.";

    int dx = 0, dy = 0;
    char key;
    char data;
    char nextData;
    int move_flag = 0;

    key = getch();

    if (key == ESC) //ESC�� ������ ����
    {
        exit_flag = 1;
        return;
    }
    else if (key == BACK_SPACE) //��ұ��
    {
        if (undo_count > 0)
        {
            GameUndo();
            return;
        }
    }
    else if (key == RESTART)
    {
        stage_clear_flag = 1;
        return;
    }
    if (key == -32)
        key = getch();

    switch (key)
    {
    case UP:
        dx = 0;
        dy = -1;
        break;
    case DOWN:
        dx = 0;
        dy = 1;
        break;
    case LEFT:
        dx = -1;
        dy = 0;
        break;
    case RIGHT:
        dx = 1;
        dy = 0;
        break;
    }

    if ((dx == 0) && (dy == 0))
        return; //���ܰ� ó��

    data = map[player_y + dy][player_x + dx];
    nextData = map[player_y + dy + dy][player_x + dx + dx];

    if (data == '1')
    {
        if (nextData == '0')
        {
            UndoSave();
            player_push_count++;
            PlayerPushCountDraw();
            map[player_y + dy + dy][player_x + dx + dx] = '1';
            map[player_y + dy][player_x + dx] = '0';
            MapDraw(player_x + dx + dx, player_y + dy + dy);
            move_flag = 1;
        }
        else if (nextData == '3')
        {
            UndoSave();
            player_push_count++;
            PlayerPushCountDraw();
            map[player_y + dy + dy][player_x + dx + dx] = '4';
            map[player_y + dy][player_x + dx] = '0';
            MapDraw(player_x + dx + dx, player_y + dy + dy);
            move_flag = 1;
        }
    }
    else if (data == '4')
    {
        if (nextData == '0')
        {
            UndoSave();
            player_push_count++;
            PlayerPushCountDraw();
            map[player_y + dy + dy][player_x + dx + dx] = '1';
            map[player_y + dy][player_x + dx] = '3';
            MapDraw(player_x + dx + dx, player_y + dy + dy);
            move_flag = 1;
        }
        else if (nextData == '3')
        {
            UndoSave();
            player_push_count++;
            PlayerPushCountDraw();
            map[player_y + dy + dy][player_x + dx + dx] = '4';
            map[player_y + dy][player_x + dx] = '3';
            MapDraw(player_x + dx + dx, player_y + dy + dy);
            move_flag = 1;
        }
    }
    else
    {
        UndoSave();
        if (data != '2')
            move_flag = 1;
    }

    if (move_flag != 0) //�������� ó��
    {
        player_move_count++;
        PlayerMoveCountDraw();
        PlayerErase();
        player_x += dx;
        player_y += dy;
        PlayerDraw();
        if (StageClearCheck() != 0) // �������� Ŭ��� Ȯ��
        {
            SetColor(YELLOW);
            gotoxy((SIZE_MAP_X * 2 - strlen(Stage_clear1)) / 2, 4);
            printf("%s", Stage_clear1);
            gotoxy((SIZE_MAP_X * 2 - strlen(Stage_clear2)) / 2, 6);
            printf("%s", Stage_clear2);
            for (;;)
                if ((key = getch()) == ' ')
                    break;

            if (++stage >= STAGE_MAX)
                stage = 0;
            stage_clear_flag = 1;
        }
    }
}

/* ========================================================================== */

/* ========================================================================== */

//�÷��̾ �׸��°�
void PlayerDraw()
{
    gotoxy(player_x * 2, player_y);
    SetColor(SKY_BLUE);
    printf("��");
}

//�÷��̾ �̵��ϸ� �ű⿡ �����ִ� �� �����͸� �׸���
void PlayerErase()
{
    MapDraw(player_x, player_y);
}

/* ========================================================================== */

//����Ҷ� �� ����
void UndoSave()
{
    int loop, loopx, loopy;
    if (undo_count >= UNDO_MAX) //������ �� �׿��ٸ�
    {
        for (loop = 1; loop < UNDO_MAX; loop++)
        {
            for (loopy = 0; loopy < SIZE_MAP_Y; ++loopy)
                for (loopx = 0; loopx < SIZE_MAP_X; ++loopx)
                    undo[loop - 1].map[loopy][loopx] = undo[loop].map[loopy][loopx];
            undo[loop - 1].player_x = undo[loop].player_x;
            undo[loop - 1].player_y = undo[loop].player_y;
            undo[loop - 1].player_move_count = undo[loop].player_move_count;
            undo[loop - 1].player_push_count = undo[loop].player_push_count;
        }
        undo_count--;
    }
    for (loopy = 0; loopy < SIZE_MAP_Y; ++loopy)
        for (loopx = 0; loopx < SIZE_MAP_X; ++loopx)
            undo[undo_count].map[loopy][loopx] = map[loopy][loopx];
    undo[undo_count].player_x = player_x;
    undo[undo_count].player_y = player_y;
    undo[undo_count].player_move_count = player_move_count;
    undo[undo_count].player_push_count = player_push_count;
    undo_count++;
}

//��ұ���
void GameUndo()
{
    int loopx, loopy;
    undo_count--;
    for (loopy = 0; loopy < SIZE_MAP_Y; ++loopy)
        for (loopx = 0; loopx < SIZE_MAP_X; ++loopx)
        {
            map[loopy][loopx] = undo[undo_count].map[loopy][loopx];
            MapDraw(loopx, loopy);
        }
    player_x = undo[undo_count].player_x;
    player_y = undo[undo_count].player_y;
    PlayerDraw();
    player_move_count = undo[undo_count].player_move_count;
    PlayerMoveCountDraw();
    player_push_count = undo[undo_count].player_push_count;
    PlayerPushCountDraw();
}

/* ========================================================================== */