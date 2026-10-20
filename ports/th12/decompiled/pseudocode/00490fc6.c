// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x490fc6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_490fc6_1 {
    char padding_0[68];
    char field_44;
    char padding_45[63];
    char field_84;
} st_490fc6_1;

typedef struct st_490fc6_0 {
    struct st_490fc6_1 *field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
} st_490fc6_0;

extern st_490fc6_0 g_4d6320;

unsigned int sub_490fc6(int a0)
{
    int v2;  // edi
    unsigned int v3;  // eax
    unsigned int v4;  // eax
    int v0;  // [bp-0x8]
    char v1;  // [bp+0x0]

    if (sub_48cac6(a0, v2, v0, &v1) != 0xffffffff && ((a0 != 1 || !(g_4d6320.field_0->field_84 & 1)) && (a0 != 2 || !(g_4d6320.field_0->field_44 & 1)) || (v3 = sub_48cac6(2), sub_48cac6(1) != sub_48cac6(2))) && !CloseHandle(sub_48cac6(a0)))
        v4 = (unsigned int)GetLastError();
    else
        v4 = 0;
    sub_48ca40(a0);
    (&g_4d6320.field_0)[a0 >> 5]->padding_0[4 + 64 * (a0 & 31)] = 0;
    if (!v4)
        return 0;
    sub_46e995(v4);
    return 0xffffffff;
}
