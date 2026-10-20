// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x474376; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_474376_1 {
    unsigned int field_0;
} st_474376_1;

typedef struct st_474376_0 {
    struct st_474376_1 *field_0;
    int field_4;
} st_474376_0;

extern unsigned int g_4aaca0;
extern int g_4ad4b0;
extern unsigned int g_4ad9e0;

int sub_474376(unsigned int a0, st_474376_0 *a1)
{
    unsigned int *v1;  // eax
    unsigned int v0;  // [bp-0x4]

    sub_46fcec(&g_4aaca0, 8);
    if (!a1)
        return sub_46fd31();
    sub_46ec9a(13);
    v0 = 0;
    if (a1->field_4 && !InterlockedDecrement(a1->field_4) && a1->field_4 != &g_4ad4b0)
        sub_46c941(a1->field_4);
    v0 = 0xfffffffe;
    sub_474361();
    if (a1->field_0)
    {
        sub_46ec9a(12);
        v0 = 1;
        sub_474084(a1->field_0);
        v1 = &a1->field_0->field_0;
        if (v1 && !*(v1) && v1 != &g_4ad9e0)
            sub_473eac(v1);
        v0 = 0xfffffffe;
        sub_47436d();
    }
    a1->field_0 = 0xbaadf00d;
    a1->field_4 = -0x45520ff3;
    sub_46c941(a1);
    return sub_46fd31();
}
