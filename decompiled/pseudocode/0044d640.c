// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44d640; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct BITMAPINFOHEADER {
    unsigned int biSize;
    int biWidth;
    int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
} BITMAPINFOHEADER;

typedef struct RGBQUAD {
    Byte rgbBlue;
    Byte rgbGreen;
    Byte rgbRed;
    Byte rgbReserved;
} RGBQUAD;

typedef struct BITMAPINFO {
    BITMAPINFOHEADER bmiHeader;
    struct RGBQUAD *bmiColors;
} BITMAPINFO;

extern unsigned int g_4aeb20[4];
extern unsigned int g_4b0e48;
extern unsigned int g_4b0e4c;
extern unsigned int g_4b0e50;
extern unsigned int g_4b0e54;
extern unsigned int g_4b0e58;
extern unsigned int g_4b0e5c;
extern unsigned int g_4b0e60;
extern unsigned int g_4b0e64;
extern unsigned int g_4b0e68;

char sub_44d640(unsigned int a0)
{
    unsigned int v7;  // ecx
    unsigned int index;  // eax
    HDC v17;  // edi
    HGDIOBJ v18;  // eax
    void* idx;  // ecx
    unsigned int v10;  // eax
    int v11;  // eax
    unsigned int v12;  // esi
    unsigned int v13;  // esi
    unsigned int v14;  // ebx
    HBITMAP v15;  // ebx
    int v16;  // edi
    unsigned int v0;  // [bp-0x78]
    unsigned int v1;  // [bp-0x74]
    int v2;  // [bp-0x70]
    BITMAPINFO v3;  // [bp-0x6c], Other Possible Types: int, char
    unsigned int v4;  // [bp-0x40]
    unsigned int v5;  // [bp-0x3c]
    unsigned int v6;  // [bp-0x38]

    sub_44d5b0(v2);
    sub_477420(&v3, 0, 108);
    v7 = g_4aeb20;
    index = 0;
    if (g_4aeb20 != 0xffffffff)
    {
        do
        {
        } while (v7 != a0 && (index += 1, v7 = g_4aeb20[6 * index], g_4aeb20[6 * index] != 0xffffffff));
    }
    if (a0 == 0xffffffff)
        return 0;
    idx = &g_4aeb20[6 * index];
    if (!idx)
        return 0;
    v10 = (int)idx[4] * 0x400;
    v11 = ((int)(v10 + ((int)(v10) >> 31 & 7)) >> 3) + 3;
    v1 = v12;
    v13 = ((int)(v11 + (v11 >> 31 & 3)) >> 2) * 4;
    v3 = (int)_INSERT(CONCAT(v3, 0), 12, 1);
    *((unsigned int *)&v3) = 108;
    *((unsigned int *)&(&v3)[4]) = 0x400;
    *((unsigned int *)&(&v3)[8]) = 0xffffffbf;
    *((short *)&(&v3)[14]) = (short)idx[4];
    v3.bmiHeader.biSizeImage = v13 * 64;
    if (a0 != 24 && a0 != 22)
    {
        *((int *)&(&v3)[40]) = (int)idx[12];
        v3.bmiHeader.biCompression = 3;
        v4 = (int)idx[16];
        v5 = (int)idx[20];
        v6 = (int)idx[8];
    }
    v0 = v14;
    v15 = CreateDIBSection(NULL, &v3, DIB_RGB_COLORS, &a0, NULL, 0);
    if (v15)
    {
        sub_477420(a0, 0, *(&v3.bmiHeader.biSizeImage), v16);
        v17 = CreateCompatibleDC(NULL);
        v18 = SelectObject(v17, v15);
        g_4b0e5c = v17;
        g_4b0e64 = v15;
        g_4b0e58 = v13;
        g_4b0e60 = v18;
        g_4b0e48 = a0;
        g_4b0e68 = a0;
        g_4b0e54 = *(&v3.bmiHeader.biSizeImage);
        g_4b0e4c = 0x400;
        g_4b0e50 = 64;
        return 1;
    }
    return 0;
}
