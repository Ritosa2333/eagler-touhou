// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x454df0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_454df0_0 {
    unsigned short field_0;
    char padding_2[282];
    unsigned int field_11c;
    char padding_120[4];
    unsigned int field_124;
} st_454df0_0;

typedef struct st_454df0_2 {
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
    struct st_454df0_0 *field_3f8;
    char padding_3fc[40];
    unsigned int field_424;
    unsigned int field_428;
    unsigned int field_42c;
    char padding_430[76];
    unsigned int field_47c;
} st_454df0_2;

typedef struct st_454df0_3 {
    char padding_0[160];
    unsigned int field_a0;
} st_454df0_3;

extern char g_4b2ed0;
extern st_454df0_3 *g_4ce8cc;

int sub_454df0(void)
{
    unsigned int v3;  // esi
    st_454df0_0 *v4;  // edi
    st_454df0_2 *idx;  // eax
    unsigned int *index;  // ebx
    unsigned short v7;  // cx
    unsigned int v8;  // eax
    unsigned int v9;  // eax
    st_454df0_2 *v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int idx1;  // [bp+0x4]

    v1 = v3;
    if (*((int *)(v4->field_11c + idx1 * 4)) && !v4->field_124)
    {
        sub_402520();
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_424 = *(index);
        idx->field_428 = index[1];
        idx->field_42c = index[2];
        idx->field_3ea = idx1;
        v7 = v4->field_0;
        idx->field_47c = idx->field_47c & 0xfffff3ff;
        idx->field_3e6 = v7;
        idx->field_3f8 = v4;
        v8 = *((int *)(v4->field_11c + idx1 * 4));
        idx->field_3ec = v8;
        idx->field_3f0 = v8;
        v9 = idx->field_78;
        if (!((char)idx->field_78 & 1))
        {
            if (/* unsupported instruction */)
                idx->field_70 = /* unsupported instruction */;
            else
                idx->field_70 = nan;
            idx->field_6c = 0;
            idx->field_68 = 0xfff0bdc1;
            *((char **)&idx->padding_74[0]) = &g_4b2ed0;
            idx->field_78 = v9 | 1;
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
