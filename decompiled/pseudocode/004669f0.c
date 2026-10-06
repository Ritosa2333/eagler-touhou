// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4669f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4669f0_4 {
    char padding_0[8];
    unsigned int field_8;
    char padding_c[112];
    unsigned int field_7c;
    unsigned int field_80;
    unsigned int field_84;
    unsigned int field_88;
    HANDLE field_8c;
    struct st_4669f0_3 *field_90;
} st_4669f0_4;

typedef struct st_4669f0_5 {
    char padding_0[4];
    struct st_4669f0_2 *field_4;
    char padding_8[4];
    struct st_4669f0_4 *field_c;
} st_4669f0_5;

typedef struct st_4669f0_3 {
    char padding_0[16];
    unsigned int field_10;
    char padding_14[8];
    int field_1c;
} st_4669f0_3;

typedef struct st_4669f0_1 {
    unsigned int field_0;
} st_4669f0_1;

typedef struct st_4669f0_2 {
    struct st_4669f0_0 *field_0;
} st_4669f0_2;

typedef struct st_4669f0_0 {
    char padding_0[52];
    struct st_4669f0_1 *field_34;
} st_4669f0_0;

extern unsigned int g_4d4760;

int sub_4669f0(unsigned int a0)
{
    st_4669f0_5 *v3;  // edi
    unsigned int v4;  // ebx
    unsigned int v5;  // esi
    int v6;  // eax
    int v7;  // eax
    st_4669f0_4 *idx;  // esi
    st_4669f0_3 *v9;  // eax
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]

    if (!v3->field_4->field_0)
    {
        return -2147221008;
    }
    else if (v3->field_c)
    {
        v1 = v4;
        v0 = v5;
        memset(v3 + 6, 0, 16);
        if (!v3->field_4->field_0)
            return -2147221008;
        v6 = sub_466220();
        if (v6 < NULL)
            return v6;
        if (a0)
        {
            v7 = sub_465fe0(v3->field_4->field_0, 0);
            if (v7 < NULL)
                return v7;
        }
        idx = v3->field_c;
        if (idx->field_7c)
        {
            v9 = idx->field_90;
            idx->field_84 = idx->field_80;
            if (v9->field_1c > NULL)
            {
                idx->field_88 = v9->field_1c;
                return (*((int *)(*((int *)&v3->field_4->field_0->padding_0[0]) + 52)))(v3->field_4->field_0, 0);
            }
        }
        else if (idx->field_8c)
        {
            SetFilePointer(idx->field_8c, idx->field_90->field_10 + g_4d4760, NULL, FILE_BEGIN);
            idx->field_8 = idx->field_90->field_1c;
        }
        return (*((int *)(*((int *)&v3->field_4->field_0->padding_0[0]) + 52)))(v3->field_4->field_0, 0);
    }
    else
    {
        return -2147221008;
    }
}
