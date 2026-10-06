// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44d5b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b0e48;
extern unsigned int g_4b0e4c;
extern unsigned int g_4b0e50;
extern HDC g_4b0e5c;
extern HGDIOBJ g_4b0e60;
extern HGDIOBJ g_4b0e64;
extern unsigned int g_4b0e68;

char sub_44d5b0(void)
{
    if (!g_4b0e5c)
        return 0;
    SelectObject(g_4b0e5c, g_4b0e60);
    DeleteDC(g_4b0e5c);
    DeleteObject(g_4b0e64);
    g_4b0e4c = 0;
    g_4b0e50 = 0;
    g_4b0e5c = 0;
    g_4b0e64 = 0;
    g_4b0e60 = 0;
    g_4b0e68 = 0;
    g_4b0e48 = 0xffffffff;
    return 1;
}
