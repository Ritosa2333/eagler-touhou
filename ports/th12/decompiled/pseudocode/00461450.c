// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x461450; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_461450_6 {
    unsigned int field_0;
} st_461450_6;

typedef struct st_461450_0 {
    char padding_0[4];
    struct st_461450_1 *field_4;
} st_461450_0;

typedef struct st_461450_3 {
    char padding_0[4];
    struct st_461450_4 *field_4;
} st_461450_3;

typedef struct st_461450_2 {
    char padding_0[60];
    unsigned int field_3c;
    char padding_40[1084];
    unsigned int field_47c;
} st_461450_2;

typedef struct st_461450_5 {
    char padding_0[8];
    struct st_461450_1 *field_8;
    struct st_461450_0 *field_c;
    char padding_10[4];
    struct st_461450_4 *field_14;
    struct st_461450_0 *field_18;
    char padding_1c[1116];
    int field_478;
    char padding_47c[20];
    struct st_461450_6 *field_490;
} st_461450_5;

typedef struct st_461450_1 {
    char padding_0[8];
    struct st_461450_0 *field_8;
} st_461450_1;

typedef struct st_461450_4 {
    struct st_461450_2 *field_0;
    struct st_461450_4 *field_4;
    struct st_461450_3 *field_8;
} st_461450_4;

extern char g_4b40bc;

unsigned int sub_461450(st_461450_5 *idx)
{
    st_461450_5 *idx1;  // esi
    st_461450_5 *index;  // edi
    st_461450_4 *v2;  // eax
    st_461450_4 *v3;  // eax
    unsigned int v5;  // edx

    index = &idx1->padding_0[4];
    if (index == *((int *)&idx[7623].padding_1c[916]))
        *((struct st_461450_0 **)&idx[7623].padding_1c[916]) = idx1->field_c;
    if (index == *((int *)&idx[7623].padding_1c[912]))
        *((struct st_461450_1 **)&idx[7623].padding_1c[912]) = idx1->field_8;
    if (index == *((int *)&idx[7623].padding_1c[924]))
        *((struct st_461450_0 **)&idx[7623].padding_1c[924]) = idx1->field_c;
    if (index == *((int *)&idx[7623].padding_1c[920]))
        *((struct st_461450_1 **)&idx[7623].padding_1c[920]) = idx1->field_8;
    if (idx1->field_490)
        idx1->field_490();
    if (*((int *)&index->padding_0[4]))
        *((struct st_461450_1 **)(*((int *)&index->padding_0[4]) + 8)) = index->field_8;
    if (index->field_8)
        *((int *)&index->field_8->padding_0[4]) = *((int *)&index->padding_0[4]);
    *((st_461450_1 **)&index->padding_0[4]) = NULL;
    index->field_8 = NULL;
    if (!idx1->field_18)
    {
        v2 = idx1->field_14;
        if (idx1->field_14)
        {
            do
            {
                v3 = v2;
                v3->field_0->field_3c = 0;
                v3->field_0->field_47c = v3->field_0->field_47c | 0x10000000;
                v2 = v3->field_4;
            } while (v3->field_4);
        }
    }
    if (idx1->field_14)
        idx1->field_14->field_8 = idx1->field_18;
    if (idx1->field_18)
        idx1->field_18->field_4 = idx1->field_14;
    idx1->field_14 = NULL;
    idx1->field_18 = NULL;
    if (idx1 >= &idx->padding_1c[160] && idx1 < &idx[4207].field_490)
    {
        v5 = 456607819 * (idx1 - idx - 188) >> 39;
        *((char *)(&idx->padding_0[v5 + (v5 >> 31)] + &g_4b40bc)) = 0;
        if (idx1->field_478)
            sub_46c941(idx1->field_478);
        idx1->field_478 = 0;
        return 0;
    }
    if (idx1->field_478)
        sub_46c941(idx1->field_478);
    idx1->field_478 = 0;
    sub_46ca4f(idx1);
    return 0;
}
