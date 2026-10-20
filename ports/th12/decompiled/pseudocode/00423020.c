// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x423020; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_423020_3 {
    struct st_423020_4 *field_0;
    char padding_4[16];
    unsigned int field_14;
} st_423020_3;

typedef struct st_423020_4 {
    struct st_423020_0 *field_0;
} st_423020_4;

typedef struct st_423020_1 {
    unsigned int field_0;
} st_423020_1;

typedef struct st_423020_0 {
    char padding_0[36];
    struct st_423020_1 *field_24;
    char padding_28[32];
    struct st_423020_1 *field_48;
} st_423020_0;

extern unsigned int g_4cf508;
extern st_423020_3 g_4d0e70;
extern st_423020_3 g_4d1410;

void sub_423020(unsigned int a0)
{
    st_423020_3 *iter;  // esi
    unsigned int v3;  // eax
    unsigned int v4;  // eax
    unsigned int v0;  // [bp-0x4]

    v0 = a0;
    g_4cf508 = 0xffffffff;
    iter = &g_4d0e70.field_0;
    do
    {
        v3 = iter->field_0;
        iter->field_14 = 0;
        if (v3)
        {
            (*((int *)(*((int *)v3) + 36)))(v3, &v0);
            v4 = iter->field_0;
            iter->field_14 = v0 & 1;
            (*((int *)(*((int *)v4) + 72)))(v4);
        }
    } while ((iter += 24, iter < &g_4d1410.field_0));
    return;
}
