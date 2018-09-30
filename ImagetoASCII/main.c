#define _CRT_SECURE_NO_WARNIGNS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push, 1)               //구조체를 1바이트 크기로 정렬

typedef struct _BITMAPFILEHEADER    //BMP 파일 헤더 구조체
{
    unsigned short bfType;          //BMP 파일 매직넘버
    unsigned int   bfSize;          //파일크기
    unsigned short bfReserved1;     //예약
    unsigned short bfReserved2;     //예약
    unsigned int   bfOffBits;       //비트맵 데이터의 시작위치
} BITMAPFILEHEADER;

typedef struct _BITMAPINFOHEADER{   //BMP 정보 헤더 구조체
    unsigned int   biSize;          //현재 구조체의 크기
    int            biWidth;         //BMP이미지의 가로크기
    int            biHeight;        //BMP이미지의 세로크기
    unsigned short biPlanes;        //사용하는 색상판의 수
    unsigned short biBitCount;      //픽셀 하나를 표현하는 비트 수
    unsigned int   biCompression;   //압축 방식
    unsigned int   biSizeImage;     //BMP이미지의 픽셀 데이터 크기
    int            biXPelsPerMeter; //그림의 가로 해상도(미터당 픽셀)
    int            biYPelsPerMeter; //그림의 세로 해상도(미터당 픽셀)
    unsigned int   biClrUsed;       //색상 테이블에서 실제 사용되는 색상 수
    unsigned int   biClrImportant;  //BMP를 표현하기 위해 필요한 색상 인덱스 수
} BITMAPINFOHEADER;

typedef struct _RGBTRIPLE           //24비트 BMP이미지의 픽셀 구조체
{
    unsigned char rgbtBlue;         //파랑
    unsigned char rgbtGreen;        //초록
    unsigned char rgbtRed;          //빨강
} RGBTRIPLE;

#pragma pack(pop)

#define PIXEL_SIZE  3   //픽셀 한 개의 크기 3바이트(24비트)
#define PIXEL_ALIGN 4   //픽셀 데이터 가로 한 줄은 4의 배수 크기로 저장됨

char path[] = "c:\\Users\\sestr\\Desktop\\";
int main()
{
    FILE *fpBmp;                    //BMP 파일 포인터
    FILE *fpTxt;                    //TXT 파일 포인터
    BITMAPFILEHEADER fileHeader;    //BMP파일 헤더 구조체 변수
    BITMAPINFOHEADER infoHeader;    //BMP정보 헤더 구조체 변수

    unsigned char *image;   //픽셀 데이터 포인터
    int size;               //픽셀 데이터 크기
    int width, height;      //BMP이미지의 가로, 세로 크기
    int padding;            //픽셀 데이터의 가로 크기가 4의 배수가 아닐 때 남는 공간의 크기

    char ascii[] = { '#', '#', '@', '%', '=', '+', '*', ':', '-', '.', ' '};
    //11개 인덱스가 높을 수록 밝아지는 것을 표현

    char fileName[20];
    char *filePath;
    puts("바탕화면에 있는 파일 이름을 입력해 주십시오(확장자 포함)");
    scanf("%s", fileName);
    filePath = strcat(path, fileName);

    fpBmp = fopen(filePath, "rb");

    if(fpBmp == NULL) return 0;

    if(fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return 0;
    }
    if(fileHeader.bfType != 'MB')
    {
        fclose(fpBmp);
        return 0;
    }
    
    if(fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return 0;
    }
    if(infoHeader.biBitCount != 24)
    {
        fclose(fpBmp);
        return 0;
    }

    size = infoHeader.biSizeImage;
    width = infoHeader.biWidth;
    height = infoHeader.biHeight;
    padding = (PIXEL_ALIGN - ((width * PIXEL_SIZE) % PIXEL_ALIGN)) % PIXEL_ALIGN;

    if(size == 0)
    {
        size = (width * PIXEL_SIZE + padding) * height;
    }

    image = malloc(size);

    fseek(fpBmp, fileHeader.bfOffBits, SEEK_SET);

    if(fread(image, size, 1, fpBmp) < 1)
    {
        fclose(fpBmp);
        return 0;
    }

    fclose(fpBmp);
    
    fpTxt = fopen(strcat(path,"ascii.txt"), "w");

    if(fpTxt == NULL)
    {
        free(image);
        return 0;
    }

    for(int y = height - 1; y >= 0; y--)
    {
        for(int x = 0; x < width; x++)
        {
            int index = (x * PIXEL_SIZE) + (y * (width * PIXEL_SIZE)) + (padding * y);

            RGBTRIPLE *pixel = (RGBTRIPLE *)&image[index];

            unsigned char blue = pixel->rgbtBlue;
            unsigned char green = pixel->rgbtGreen;
            unsigned char red = pixel->rgbtRed;

            unsigned char gray = (red + green + blue) / PIXEL_SIZE;

            char c = ascii[gray * sizeof(ascii) / 256];

            fprintf(fpTxt, "%c%c", c, c);
        }

        fprintf(fpTxt, "\n");
    }

    fclose(fpTxt);
    free(image);

    return 0;
}