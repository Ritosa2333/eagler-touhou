// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47dc29; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47dc29_1 {
    struct st_47dc29_2 *field_0;
    char padding_4[4];
    struct st_47dc29_0 *field_8;
    struct st_47dc29_0 *field_c;
    unsigned int field_10;
} st_47dc29_1;

typedef struct st_47dc29_0 {
    struct st_47dc29_0 *field_0;
} st_47dc29_0;

typedef struct st_47dc29_2 {
    unsigned int field_0;
} st_47dc29_2;

extern void g_4b42f8;

unsigned int sub_47dc29(st_47dc29_1 *idx, int a1, unsigned int a2, unsigned int a3)
{
    unsigned int v2;  // edi
    unsigned int v3;  // edi
    unsigned int v4;  // ebx
    st_47dc29_0 **v5;  // eax
    unsigned int v6;  // eax
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]

    v1 = v2;
    v3 = a2 + 7 & 0xfffffff8;
    if (a3)
        return idx->field_0(v3);
    if (!v3)
        v3 = 8;
    v0 = v4;
    if (idx->field_10 >= v3)
    {
        idx->field_10 = idx->field_10 - v3;
        goto LABEL_47dca0;
    }
    else if (v3 <= 0x1000)
    {
        v5 = sub_47dc29(&g_4b42f8, a1, 4100, 1);
        if (v5)
            *(v5) = NULL;
        else
            v5 = NULL;
        if (!v5)
            goto LABEL_47dc97;
        if (idx->field_c)
            idx->field_c->field_0 = v5;
        else
            idx->field_8 = v5;
        idx->field_c = v5;
        idx->field_10 = 0x1000 - v3;
LABEL_47dca0:
        v6 = (char *)idx->field_c + idx->field_10 + 4;
    }
    else
    {
LABEL_47dc97:
        v6 = 0;
    }
    return v6;
}
