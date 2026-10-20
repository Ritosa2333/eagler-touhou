// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c8fc; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
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

HBITMAP sub_46c8fc(BITMAPINFO *a0, enum DIB_USAGE a1, void* *a2, HANDLE a3, unsigned int a4, unsigned int a5)
{
    HDC v0;  // [bp+0x0]

    return CreateDIBSection(v0, a0, a1, a2, a3, a4);
}
