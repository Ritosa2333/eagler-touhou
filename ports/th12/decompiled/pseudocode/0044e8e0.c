// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44e8e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44e8e0_2 {
    unsigned int field_0;
} st_44e8e0_2;

typedef struct st_44e8e0_1 {
    char padding_0[72];
    struct st_44e8e0_2 *field_48;
} st_44e8e0_1;

typedef struct st_44e8e0_3 {
    char padding_0[8];
    struct st_44e8e0_2 *field_8;
} st_44e8e0_3;

typedef struct st_44e8e0_0 {
    unsigned short field_0;
} st_44e8e0_0;

extern int g_4b0e48;
extern char g_4b0e54;
extern int g_4b0e58;
extern HDC g_4b0e5c;
extern st_44e8e0_0 *g_4b0e68;
extern HGDIOBJ g_4b453c;
extern HGDIOBJ g_4cc54c;
extern HGDIOBJ g_4ce550;
extern HGDIOBJ g_4ce554;

int sub_44e8e0(void)
{
    unsigned int v13;  // ebx
    unsigned int v14;  // esi
    st_44e8e0_1 **v23;  // eax
    st_44e8e0_3 **v24;  // edi
    unsigned int v15;  // eax
    HGDIOBJ v16;  // edi
    unsigned short *iter;  // edx
    int i;  // ecx
    unsigned int v19;  // edi
    HGDIOBJ v20;  // ebx
    st_44e8e0_1 **v21;  // eax
    st_44e8e0_1 **v22;  // eax
    unsigned int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x20]
    int v2;  // [bp-0x18], Other Possible Types: unsigned int
    unsigned int v3;  // [bp-0x14]
    int v4;  // [bp-0x10], Other Possible Types: unsigned int
    unsigned int v5;  // [bp-0xc]
    int v6;  // [bp-0x4]
    unsigned int *v7;  // [bp+0x0]
    unsigned int v9;  // [bp+0x8]
    int v10;  // [bp+0xc]
    unsigned int v11;  // [bp+0x10]
    st_44e8e0_1 **v12;  // [bp+0x14]

    v1 = v13;
    v0 = v14;
    if (!v15)
    {
        v16 = g_4ce554;
    }
    else if (v15 == 1)
    {
        v16 = g_4ce550;
    }
    else
    {
        v16 = g_4cc54c;
        if (v15 != 2)
            v16 = g_4b453c;
    }
    if (v10 < 0x11)
        v10 = 0x11;
    iter = &g_4b0e68->field_0;
    i = 0;
    if (*((int *)&g_4b0e54) > NULL)
    {
        do
        {
            *(iter) = ((v11 >> 20 & 15) * 16 | v11 >> 12 & 15) * 16 | v11 >> 4 & 15;
            i += 2;
            iter += 1;
        } while (i < *((int *)&g_4b0e54));
    }
    v19 = v10 * 2 + 6;
    v20 = SelectObject(g_4b0e5c, v16);
    sub_44dc20(v19);
    SetBkMode(g_4b0e5c, 1);
    v21 = v12;
    do
    {
        v22 = v21;
        v23 = (char *)v22 + 1;
        v21 = v23;
    } while (*((char *)v22));
    v2 = v23 - ((char *)v12 + 1);
    SetTextColor(g_4b0e5c, v11);
    TextOutA(g_4b0e5c, v9 * 2, 0, v12, v2);
    SelectObject(g_4b0e5c, v20);
    sub_44dc20(v19);
    sub_44d8b0(v19);
    SelectObject(g_4b0e5c, v20);
    v2 = 0;
    v3 = 0;
    v4 = (v7[2] - *(v7)) * 2 + 22;
    v5 = v9 * 2 + 2;
    if (v4 > 0x400)
        v4 = 0x400;
    *(v12)->field_48(v12, 0, &v9);
    D3DXLoadSurfaceFromMemory(v6, 0, v7, g_4b0e68, g_4b0e48, g_4b0e58, 0, &ebp, 4, 0);
    if (v24)
        *(v24)->field_8(v24);
    return;
}
