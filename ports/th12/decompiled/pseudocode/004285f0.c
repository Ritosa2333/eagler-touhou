// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4285f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4285f0_2 {
    struct st_4285f0_0 *field_0;
    char padding_4[4];
    struct st_4285f0_2 *field_8;
    unsigned int field_c;
    char padding_10[109];
    char field_7d;
} st_4285f0_2;

typedef struct st_4285f0_0 {
    char padding_0[24];
    struct st_4285f0_1 *field_18;
} st_4285f0_0;

typedef struct st_4285f0_1 {
    unsigned int field_0;
} st_4285f0_1;

typedef struct st_4285f0_3 {
    char padding_0[24];
    struct st_4285f0_2 *field_18;
    char padding_1c[1108];
    unsigned int field_470;
    unsigned int field_474;
    unsigned int field_478;
    unsigned int field_47c;
    unsigned int field_480;
    unsigned int field_484;
} st_4285f0_3;

extern st_4285f0_3 *g_4b44f4;

unsigned int sub_4285f0(unsigned int a0)
{
    st_4285f0_2 *v0;  // ecx
    unsigned int *idx;  // edi
    unsigned int *index;  // esi
    unsigned int v3;  // ebx
    st_4285f0_2 *v4;  // ecx

    v0 = g_4b44f4->field_18;
    g_4b44f4->field_470 = *(idx);
    g_4b44f4->field_474 = idx[1];
    g_4b44f4->field_478 = idx[2];
    g_4b44f4->field_47c = *(index);
    g_4b44f4->field_480 = index[1];
    v3 = 0;
    g_4b44f4->field_484 = index[2];
    if (!v0)
        return 0;
    do
    {
        v4 = v0;
        if (v4->field_c != 1 && v4->field_7d)
            v3 += v4->field_0->field_18(idx, index, a0, 1);
    } while ((v0 = (st_4285f0_2 *)v4->field_8, v4->field_8));
    return v3;
}
