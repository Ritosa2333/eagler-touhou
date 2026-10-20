// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x466c90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_466c90_1 {
    char padding_0[8];
    unsigned int field_8;
    char padding_c[112];
    unsigned int field_7c;
    unsigned int field_80;
    unsigned int field_84;
    unsigned int field_88;
    HANDLE field_8c;
    struct st_466c90_0 *field_90;
} st_466c90_1;

typedef struct st_466c90_0 {
    char padding_0[16];
    unsigned int field_10;
    char padding_14[4];
    int field_18;
    int field_1c;
} st_466c90_0;

extern unsigned int g_4d4760;

unsigned int sub_466c90(char a0)
{
    st_466c90_1 *idx;  // esi
    unsigned int v2;  // edx
    st_466c90_0 *index;  // ecx
    char v4;  // bl
    HANDLE v5;  // eax
    char v6;  // bl
    unsigned int v7;  // edi

    if (idx->field_7c)
    {
        v2 = idx->field_80;
        index = idx->field_90;
        idx->field_84 = v2;
        if (index->field_1c > 0)
            idx->field_88 = index->field_1c;
        if (!v4)
            return 0;
        if (index->field_18 <= 0)
            return 0;
        idx->field_84 = index->field_18 + v2;
        return 0;
    }
    else
    {
        v5 = idx->field_8c;
        if (!v5)
            return 2147746288;
        if (v6 && idx->field_90->field_18 > 0)
        {
            SetFilePointer(v5, idx->field_90->field_10 + idx->field_90->field_18 + g_4d4760, NULL, FILE_BEGIN);
            idx->field_8 = idx->field_90->field_1c - idx->field_90->field_18;
            return 0;
        }
        if (!v7)
        {
            SetFilePointer(v5, idx->field_90->field_10 + g_4d4760, NULL, FILE_BEGIN);
            idx->field_8 = idx->field_90->field_1c;
        }
        else
        {
            SetFilePointer(v5, g_4d4760 + v7, NULL, FILE_BEGIN);
            idx->field_8 = idx->field_90->field_1c + idx->field_90->field_10 - v7;
        }
        return 0;
    }
}
