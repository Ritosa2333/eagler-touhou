// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x412be0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_412be0_2 {
    char padding_0[4804];
    struct st_412be0_1 *field_12c4;
    struct st_412be0_0 *field_12c8;
} st_412be0_2;

typedef struct st_412be0_0 {
    char padding_0[4];
    struct st_412be0_1 *field_4;
} st_412be0_0;

typedef struct st_412be0_1 {
    char padding_0[8];
    struct st_412be0_0 *field_8;
} st_412be0_1;

typedef struct st_412be0_7 {
    char padding_0[104];
    struct st_412be0_1 *field_68;
    struct st_412be0_0 *field_6c;
    unsigned int field_70;
} st_412be0_7;

extern st_412be0_7 *g_4b43dc;

void sub_412be0(unsigned int a0, st_412be0_2 *index)
{
    st_412be0_2 *idx;  // eax

    idx = &index->padding_0[4800];
    if (g_4b43dc->field_68 == idx)
        g_4b43dc->field_68 = index->field_12c4;
    if (g_4b43dc->field_6c == idx)
        g_4b43dc->field_6c = index->field_12c8;
    if (*((int *)&idx->padding_0[4]))
        *((int *)(*((int *)&idx->padding_0[4]) + 8)) = *((int *)&idx->padding_0[8]);
    if (*((int *)&idx->padding_0[8]))
        *((int *)(*((int *)&idx->padding_0[8]) + 4)) = *((int *)&idx->padding_0[4]);
    *((st_412be0_1 **)&idx->padding_0[4]) = NULL;
    *((st_412be0_0 **)&idx->padding_0[8]) = NULL;
    g_4b43dc->field_70 = g_4b43dc->field_70 - 1;
    return;
}
