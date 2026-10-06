// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45ea40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45ea40_0 {
    char padding_0[4];
    struct st_45ea40_0 *field_4;
} st_45ea40_0;

extern unsigned int g_4ad138;
extern char g_4b55f8;

void sub_45ea40(int idx)
{
    unsigned long v6;  // ldt
    unsigned long v7;  // gdt
    st_45ea40_0 *v16;  // eax
    unsigned long long v17;  // 4120
    unsigned short v8;  // fs
    unsigned long long v9;  // 4177
    unsigned int v10;  // ebx
    unsigned int v11;  // esi
    unsigned long long v12;  // 4181
    st_45ea40_0 *v13;  // eax
    st_45ea40_0 *v14;  // eax
    st_45ea40_0 *v15;  // eax
    unsigned int v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x10]
    char v3;  // [bp-0xc], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0x8]
    unsigned int v5;  // [bp-0x4]

    v5 = 0xffffffff;
    v4 = sub_497217;
    v9 = _ccall(v6, v7, v8, 0);
    v3 = *((int *)(unsigned int)v9);
    v2 = v10;
    v1 = v11;
    v0 = g_4ad138 ^ &edi;
    v12 = _ccall(v6, v7, v8, 0);
    *((unsigned int **)(unsigned int)v12) = &v3;
    v5 = 2;
    v13 = *((int *)(idx + 8935096));
    if (*((int *)(idx + 8935096)))
    {
        do
        {
            v14 = v13;
            sub_461450(idx);
            v13 = v14->field_4;
        } while (v14->field_4);
    }
    v15 = *((int *)(idx + 8935104));
    if (*((int *)(idx + 8935104)))
    {
        do
        {
            v16 = v15;
            sub_461450(idx);
            v15 = v16->field_4;
        } while (v16->field_4);
    }
    v3 = 1;
    sub_46cf1e(idx + 8935112, 1204, 32, sub_4026e0);
    if (*((int *)&(&g_4b55f8)[idx]))
        sub_46c941(*((int *)&(&g_4b55f8)[idx]));
    *((unsigned int *)&(&g_4b55f8)[idx]) = 0;
    v0 = 0xffffffff;
    sub_46cf1e(idx + 188, 1204, 0x1000, sub_4026e0);
    v17 = _ccall(v6, v7, v8, 0);
    *((unsigned int *)(unsigned int)v17) = idx + 8935112;
    return;
}
