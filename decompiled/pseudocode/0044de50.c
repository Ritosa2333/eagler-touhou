// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44de50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44de50_0 {
    char padding_0[260];
    unsigned int field_104;
    unsigned int field_108;
    char padding_10c[8];
    HDC field_114;
} st_44de50_0;

int sub_44de50(void)
{
    unsigned int v20;  // ebp
    LPArray v21;  // edi
    unsigned int v30;  // eax
    unsigned int v22;  // ebx
    unsigned int v23;  // esi
    st_44de50_0 *idx;  // eax
    HDC v25;  // esi
    unsigned int v26;  // eax
    unsigned int v27;  // eax
    unsigned int v28;  // ecx
    unsigned int v29;  // edx
    unsigned int v0;  // [bp-0x94]
    unsigned int v1;  // [bp-0x90]
    unsigned int v2;  // [bp-0x88]
    unsigned int v3;  // [bp-0x84]
    unsigned int v4;  // [bp-0x80]
    unsigned int v5;  // [bp-0x7c]
    HGDIOBJ v6;  // [bp-0x78]
    unsigned int v7;  // [bp-0x74]
    int v8;  // [bp-0x70], Other Possible Types: int (32 bits)[4]
    int v9;  // [bp-0x60], Other Possible Types: int (32 bits)[4]
    int v10;  // [bp-0x50], Other Possible Types: int (32 bits)[4]
    int v11[4];  // [bp-0x40]
    int v12[4];  // [bp-0x30]
    int v13[4];  // [bp-0x20]
    int v14[4];  // [bp-0x10]
    unsigned int v15;  // [bp+0x4]
    unsigned int v16;  // [bp+0x8]
    HGDIOBJ v17;  // [bp+0xc]
    unsigned int v18;  // [bp+0x10]
    unsigned int v19;  // [bp+0x14]

    v20 = v16;
    if (!v21)
        return;
    v1 = v22;
    v0 = v23;
    v25 = idx->field_114;
    v3 = idx->field_104;
    v4 = idx->field_108;
    v6 = SelectObject(v25, v17);
    SetBkMode(v25, 1);
    if (v19 != 0xffffffff)
    {
        SetTextColor(v25, v19);
        v26 = v15;
        *((unsigned int *)&v8) = v26;
        v2 = v26 + v3;
        *((unsigned int *)&(&v8)[8]) = v2 - 2;
        v8[3] = v4 + v20 - 2;
        v8[1] = v20;
        DrawTextA(v25, v21, -0x1, v8, DT_TOP);
        v5 = v4 + v20;
        *((unsigned int *)&(&v10)[12]) = v5;
        *(v11) = v15 + 1;
        v11[1] = v20 + 2;
        v11[2] = v2 + 1;
        v27 = v15 + 2;
        v7 = v5 + 2;
        v11[3] = v7;
        v12[3] = v5;
        *(v10) = v27;
        *(v12) = v27;
        *(v14) = v27;
        *(v13) = v15;
        v14[1] = v20 + 2;
        v13[1] = v20 + 2;
        v28 = v2 + 2;
        v13[2] = v2;
        v29 = v7;
        *((unsigned int *)&(&v10)[4]) = v20;
        v10[2] = v28;
        v12[1] = v20;
        v12[2] = v28;
        v13[3] = v29;
        v14[2] = v28;
        v14[3] = v29;
        DrawTextA(v25, v21, -0x1, v10, DT_TOP);
        DrawTextA(v25, v21, -0x1, v11, DT_TOP);
        DrawTextA(v25, v21, -0x1, v12, DT_TOP);
        DrawTextA(v25, v21, -0x1, v13, DT_TOP);
        DrawTextA(v25, v21, -0x1, v14, DT_TOP);
    }
    SetTextColor(v25, v18);
    v30 = v15;
    *((unsigned int *)&v9) = v30 + 1;
    *((unsigned int *)&(&v9)[4]) = v20 + 1;
    v9[2] = v3 + v30 + 1;
    v9[3] = v4 + v20 + 1;
    DrawTextA(v25, v21, -0x1, v9, DT_TOP);
    SelectObject(v25, v6);
    return;
}
