#define _CRT_SECURE_NO_WARNIGNS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push, 1)               //����ü�� 1����Ʈ ũ��� ����

typedef struct _BITMAPFILEHEADER    //BMP ���� ��� ����ü
{
    unsigned short bfType;          //BMP ���� �����ѹ�
    unsigned int   bfSize;          //����ũ��
    unsigned short bfReserved1;     //����
    unsigned short bfReserved2;     //����
    unsigned int   bfOffBits;       //��Ʈ�� �������� ������ġ
} BITMAPFILEHEADER;

typedef struct _BITMAPINFOHEADER{   //BMP ���� ��� ����ü
    unsigned int   biSize;          //���� ����ü�� ũ��
    int            biWidth;         //BMP�̹����� ����ũ��
    int            biHeight;        //BMP�̹����� ����ũ��
    unsigned short biPlanes;        //����ϴ� �������� ��
    unsigned short biBitCount;      //�ȼ� �ϳ��� ǥ���ϴ� ��Ʈ ��
    unsigned int   biCompression;   //���� ���
    unsigned int   biSizeImage;     //BMP�̹����� �ȼ� ������ ũ��
    int            biXPelsPerMeter; //�׸��� ���� �ػ�(���ʹ� �ȼ�)
    int            biYPelsPerMeter; //�׸��� ���� �ػ�(���ʹ� �ȼ�)
    unsigned int   biClrUsed;       //���� ���̺��� ���� ���Ǵ� ���� ��
    unsigned int   biClrImportant;  //BMP�� ǥ���ϱ� ���� �ʿ��� ���� �ε��� ��
} BITMAPINFOHEADER;

typedef struct _RGBTRIPLE           //24��Ʈ BMP�̹����� �ȼ� ����ü
{
    unsigned char rgbtBlue;         //�Ķ�
    unsigned char rgbtGreen;        //�ʷ�
    unsigned char rgbtRed;          //����
} RGBTRIPLE;

#pragma pack(pop)

#define PIXEL_SIZE  3   //�ȼ� �� ���� ũ�� 3����Ʈ(24��Ʈ)
#define PIXEL_ALIGN 4   //�ȼ� ������ ���� �� ���� 4�� ��� ũ��� �����

char path[] = "c:\\Users\\sestr\\Desktop\\";
int main()
{
    FILE *fpBmp;                    //BMP ���� ������
    FILE *fpTxt;                    //TXT ���� ������
    BITMAPFILEHEADER fileHeader;    //BMP���� ��� ����ü ����
    BITMAPINFOHEADER infoHeader;    //BMP���� ��� ����ü ����

    unsigned char *image;   //�ȼ� ������ ������
    int size;               //�ȼ� ������ ũ��
    int width, height;      //BMP�̹����� ����, ���� ũ��
    int padding;            //�ȼ� �������� ���� ũ�Ⱑ 4�� ����� �ƴ� �� ���� ������ ũ��

    char ascii[] = { '#', '#', '@', '%', '=', '+', '*', ':', '-', '.', ' '};
    //11�� �ε����� ���� ���� ������� ���� ǥ��

    char fileName[20];
    char *filePath;
    puts("����ȭ�鿡 �ִ� ���� �̸��� �Է��� �ֽʽÿ�(Ȯ���� ����)");
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