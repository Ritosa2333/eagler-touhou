// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44e660; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44e660_2 {
    Byte field_0;
    char field_1;
} st_44e660_2;

typedef struct st_44e660_1 {
    unsigned int field_0;
} st_44e660_1;

typedef struct st_44e660_0 {
    char padding_0[72];
    struct st_44e660_1 *field_48;
} st_44e660_0;

typedef struct st_44e660_3 {
    char padding_0[8];
    struct st_44e660_1 *field_8;
} st_44e660_3;

extern int g_4b0e48;
extern int g_4b0e54;
extern int g_4b0e58;
extern HDC g_4b0e5c;
extern int g_4b0e68;
extern HGDIOBJ g_4b453c;
extern HGDIOBJ g_4cc54c;
extern HGDIOBJ g_4ce550;
extern HGDIOBJ g_4ce554;

int sub_44e660(void)
{
    unsigned int v17;  // esi
    unsigned int v18;  // edi
    unsigned int v27;  // edx
    st_44e660_2 *iter;  // ebx
    int v29;  // edi
    int v30;  // edi
    HGDIOBJ v31;  // ebx
    st_44e660_3 **v32;  // eax
    unsigned int v19;  // eax
    HGDIOBJ v20;  // ebp
    st_44e660_2 *v21;  // ecx
    st_44e660_2 *v22;  // ebp
    st_44e660_2 *v23;  // ebp
    st_44e660_2 *v24;  // ebp
    st_44e660_2 *v25;  // ebp
    unsigned int v26;  // ebp
    unsigned int v0;  // [bp-0x34]
    unsigned int v1;  // [bp-0x30]
    unsigned int v2;  // [bp-0x24]
    unsigned int i;  // [bp-0x24]
    HGDIOBJ v4;  // [bp-0x20], Other Possible Types: unsigned int
    HGDIOBJ v5;  // [bp-0x1c], Other Possible Types: unsigned int
    int v6;  // [bp-0x18], Other Possible Types: unsigned int
    unsigned int v7;  // [bp-0x14]
    int v8;  // [bp-0x10]
    unsigned int *v9;  // [bp-0x8]
    unsigned int v10;  // [bp-0x4]
    unsigned int v11;  // [bp+0x4]
    st_44e660_0 **v12;  // [bp+0x8]
    unsigned int v13;  // [bp+0xc]
    unsigned int v14;  // [bp+0x14], Other Possible Types: char
    char v15;  // [bp+0x15]
    char v16;  // [bp+0x16]

    v1 = v17;
    v0 = v18;
    if (!v19)
    {
        v20 = g_4ce554;
    }
    else if (v19 == 1)
    {
        v20 = g_4ce550;
    }
    else
    {
        v20 = g_4cc54c;
        if (v19 != 2)
            v20 = g_4b453c;
    }
    if (v12 < 0x11)
        v12 = 0x11;
    sub_477420(g_4b0e68, 0, g_4b0e54);
    v5 = SelectObject(g_4b0e5c, v20);
    sub_44dc20(v12 * 2 + 6);
    SetBkMode(g_4b0e5c, 1);
    v22 = v21;
    v23 = v22;
    do
    {
        v24 = v23;
        v25 = &v24->field_1;
        v23 = v25;
    } while (v24->field_0);
    v26 = v25 - &v22->field_1;
    if (!v14)
    {
        SetTextColor(g_4b0e5c, v13);
        v13 = v27 * 2;
        TextOutA(g_4b0e5c, v13 + 2, 4, v21, v26);
        TextOutA(g_4b0e5c, v13 - 2, 4, v21, v26);
        TextOutA(g_4b0e5c, v13 + 2, 0, v21, v26);
        TextOutA(g_4b0e5c, v13 - 2, 0, v21, v26);
        SetTextColor(g_4b0e5c, v12);
        TextOutA(g_4b0e5c, v13, 2, v21, v26);
    }
    else
    {
        v14 = 0;
        v15 = 0;
        v16 = 0;
        if (v26 > NULL)
        {
            iter = &v21->field_1;
            v5 = v14 * 2;
            v29 = v27 * 2 - 2;
            v2 = (v26 - 1 >> 1) + 1;
            do
            {
                i = v2;
                SetTextColor(g_4b0e5c, v13);
                v14 = *(&iter->field_0 - 1);
                v15 = iter->field_0;
                TextOutA(g_4b0e5c, v29 + 4, 4, &v14, 2);
                TextOutA(g_4b0e5c, v29, 4, &v14, 2);
                TextOutA(g_4b0e5c, v29 + 4, 0, &v14, 2);
                TextOutA(g_4b0e5c, v29, 0, &v14, 2);
                SetTextColor(g_4b0e5c, v12);
                TextOutA(g_4b0e5c, v29 + 2, 2, &v14, 2);
                v29 += v5;
                iter += 1;
                v2 = i - 1;
            } while (i != 1);
        }
    }
    SelectObject(g_4b0e5c, v4);
    v30 = v11 * 2 + 6;
    sub_44dc20(v30);
    sub_44d8b0(v30);
    SelectObject(g_4b0e5c, v31);
    v4 = 0;
    v5 = 0;
    v6 = (v9[2] - *(v9)) * 2 + 22;
    v7 = v10 * 2 + 2;
    if (v6 > 0x400)
        v6 = 0x400;
    *(v12)->field_48(v12, 0, &v10);
    D3DXLoadSurfaceFromMemory(v8, 0, v9, g_4b0e68, g_4b0e48, g_4b0e58, 0, &ebp, 4, 0);
    v32 = v12 * 2 + 6;
    if (v32)
        *(v32)->field_8(v32);
    return;
}
