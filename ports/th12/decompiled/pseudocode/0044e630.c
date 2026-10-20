// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44e630; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern HGDIOBJ g_4b453c;
extern HGDIOBJ g_4cc54c;
extern HGDIOBJ g_4ce550;
extern HGDIOBJ g_4ce554;

int sub_44e630(void)
{
    int v0;  // [bp-0x4]

    sub_44d5b0(v0);
    DeleteObject(g_4ce554);
    DeleteObject(g_4cc54c);
    DeleteObject(g_4ce550);
    return DeleteObject(g_4b453c);
}
