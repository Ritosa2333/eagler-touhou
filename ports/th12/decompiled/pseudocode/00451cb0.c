// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x451cb0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_451cb0_0 {
    char padding_0[24];
    unsigned int field_18;
} st_451cb0_0;

typedef struct st_451cb0_2 {
    unsigned int field_0;
} st_451cb0_2;

typedef struct st_451cb0_1 {
    char padding_0[24];
    struct st_451cb0_2 *field_18;
} st_451cb0_1;

typedef struct st_451cb0_3 {
    struct st_451cb0_1 *field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
} st_451cb0_3;

extern unsigned int g_4ce8c8;
extern st_451cb0_3 g_4ce90c;

unsigned int sub_451cb0(st_451cb0_0 *a0)
{
    char v0;  // [bp-0x18]

    if (!((char)a0->field_18 & 3))
    {
        return 1;
    }
    else if ((*((int *)(*((int *)&(&g_4ce90c.field_0)[g_4ce8c8]->padding_0[0]) + 24)))((&g_4ce90c.field_0)[g_4ce8c8], 4, &v0, 24, 16, a0->field_18, 2, 0xfffffc18, 1000) < 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
