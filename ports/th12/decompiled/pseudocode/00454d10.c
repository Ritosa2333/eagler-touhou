// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x454d10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_454d10_0 {
    unsigned short field_0;
    char padding_2[282];
    unsigned int field_11c;
    char padding_120[4];
    unsigned int field_124;
} st_454d10_0;

typedef struct st_454d10_2 {
    char padding_0[104];
    unsigned int field_68;
    unsigned int field_6c;
    unsigned int field_70;
    char padding_74[4];
    unsigned int field_78;
    char padding_7c[874];
    unsigned short field_3e6;
    char padding_3e8[2];
    unsigned short field_3ea;
    unsigned int field_3ec;
    unsigned int field_3f0;
    char padding_3f4[4];
    struct st_454d10_0 *field_3f8;
    char padding_3fc[128];
    unsigned int field_47c;
} st_454d10_2;

typedef struct st_454d10_3 {
    char padding_0[160];
    unsigned int field_a0;
} st_454d10_3;

extern char g_4b2ed0;
extern st_454d10_3 *g_4ce8cc;

void sub_454d10(st_454d10_0 *a0, unsigned int a1, st_454d10_2 *idx, unsigned int index)
{
    unsigned int v5;  // ebx
    unsigned int v6;  // esi
    unsigned int v7;  // edi
    unsigned short v8;  // cx
    unsigned int v9;  // eax
    unsigned int v10;  // eax
    st_454d10_2 *v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    st_454d10_0 *v4;  // [bp-0x8]

    v4 = a0;
    v3 = v5;
    v2 = v6;
    v1 = v7;
    if (*((int *)(a0->field_11c + index * 4)) && !a0->field_124)
    {
        sub_402520();
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_3ea = index;
        v8 = a0->field_0;
        idx->field_47c = idx->field_47c & 0xfffff3ff;
        idx->field_3e6 = v8;
        idx->field_3f8 = a0;
        v9 = *((int *)(a0->field_11c + index * 4));
        idx->field_3ec = v9;
        idx->field_3f0 = v9;
        v10 = idx->field_78;
        if (!((char)idx->field_78 & 1))
        {
            if (/* unsupported instruction */)
                idx->field_70 = /* unsupported instruction */;
            else
                idx->field_70 = nan;
            idx->field_6c = 0;
            idx->field_68 = 0xfff0bdc1;
            *((char **)&idx->padding_74[0]) = &g_4b2ed0;
            idx->field_78 = v10 | 1;
        }
        if (/* unsupported instruction */)
        {
            idx->field_70 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx->field_70 = nan;
            /* unsupported instruction */
        }
        idx->field_6c = 0;
        idx->field_68 = 0xffffffff;
        idx->field_47c = idx->field_47c & 0xfffffffe;
        v0 = idx;
        sub_455630();
        g_4ce8cc->field_a0 = g_4ce8cc->field_a0 + 1;
        return;
    }
    sub_477420(idx, 0, 1204);
    return;
}
