// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x462890; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_462890_2 {
    char padding_0[8];
    struct st_462890_1 *field_8;
    char padding_c[12];
    struct st_462890_2 *field_18;
    struct st_462890_3 *field_1c;
} st_462890_2;

typedef struct st_462890_1 {
    char padding_0[4];
    struct st_462890_2 *field_4;
} st_462890_1;

typedef struct st_462890_3 {
    char padding_0[4];
    struct st_462890_2 *field_4;
} st_462890_3;

typedef struct st_462890_0 {
    char padding_0[4];
    char field_4;
    char padding_5[3];
    unsigned int field_8;
} st_462890_0;


void sub_462890(st_462890_0 *a0, st_462890_2 *a1)
{
    st_462890_2 *v2;  // eax
    st_462890_2 *idx;  // eax
    st_462890_2 *v4;  // eax
    unsigned int v5;  // esi
    unsigned int v6;  // 4121
    unsigned int v0;  // [bp-0x8]

    if (!a0)
        return;
    v2 = &a1->padding_c[8];
    if (v2)
    {
        do
        {
            idx = v2;
            if (*((int *)&idx->padding_0[0]) == a0)
                goto LABEL_4628bf;
        } while ((v2 = (st_462890_2 *)*((int *)&idx->padding_0[4]), *((int *)&idx->padding_0[4])));
    }
    v4 = &a1[1].field_18;
    if (!v4)
        return;
    while (1)
    {
        idx = v4;
        if (*((int *)&idx->padding_0[0]) == a0)
            break;
        v4 = *((int *)&idx->padding_0[4]);
        if (!*((int *)&idx->padding_0[4]))
            return;
    }
LABEL_4628bf:
    v0 = v5;
    if (!idx->field_8)
        return;
    if (*((int *)&idx->padding_0[4]))
        *((struct st_462890_1 **)(*((int *)&idx->padding_0[4]) + 8)) = idx->field_8;
    if (idx->field_8)
        idx->field_8->field_4 = *((int *)&idx->padding_0[4]);
    *((st_462890_2 **)&idx->padding_0[4]) = NULL;
    idx->field_8 = NULL;
    v6 = a0->field_4 & 1;
    a0->field_8 = 0;
    if ((char)v6)
    {
        memset(&a0->field_8, 0, 12);
        sub_46ca4f(a0);
    }
    return;
}
