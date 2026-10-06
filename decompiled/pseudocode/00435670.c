// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x435670; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_435670_2 {
    char padding_0[12];
    unsigned int field_c;
} st_435670_2;

typedef struct st_435670_3 {
    struct st_435670_4 *field_0;
    char padding_4[4];
    struct st_435670_2 *field_8;
    char padding_c[8];
    unsigned int field_14;
} st_435670_3;

typedef struct st_435670_4 {
    struct st_435670_0 *field_0;
} st_435670_4;

typedef struct st_435670_1 {
    unsigned int field_0;
} st_435670_1;

typedef struct st_435670_0 {
    char padding_0[48];
    struct st_435670_1 *field_30;
} st_435670_0;

extern st_435670_3 g_4d0e70;
extern st_435670_3 g_4d1410;

void sub_435670(unsigned int a0)
{
    st_435670_3 *iter;  // esi
    unsigned int v1;  // eax

    iter = &g_4d0e70.field_0;
    do
    {
        v1 = iter->field_0;
        if (v1 && iter->field_14)
            (*((int *)(*((int *)v1) + 48)))(v1, 0, 0, iter->field_8->field_c);
    } while ((iter += 24, iter < &g_4d1410.field_0));
    return;
}
