// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40ed80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40ed80_1 {
    char padding_0[8];
    unsigned int field_8;
} st_40ed80_1;

typedef struct st_40ed80_2 {
    char padding_0[100];
    struct st_40ed80_1 *field_64;
} st_40ed80_2;

typedef struct st_40ed80_0 {
    char padding_0[48];
    unsigned int field_30;
    unsigned int field_34;
    char padding_38[220];
    unsigned int field_114;
    char padding_118[220];
    unsigned int field_1f4;
} st_40ed80_0;

extern st_40ed80_0 *g_4b43d0;
extern st_40ed80_2 *g_4b43dc;

unsigned int sub_40ed80(unsigned int a0)
{
    int v0;  // edi
    int v1;  // esi
    int v2;  // ecx

    sub_436410(v0, v1, v2);
    sub_4097f0(v0, v1, v2);
    sub_406b20();
    sub_425b10();
    sub_43dd50();
    sub_4130e0(*((int *)(g_4b43d0->field_34 + g_4b43d0->field_114 * 4)));
    sub_41df40(*((int *)(g_4b43d0->field_34 + g_4b43d0->field_114 * 4)));
    g_4b43d0->field_1f4 = g_4b43dc->field_64->field_8;
    sub_464970(1);
    sub_464970(-0x1);
    g_4b43d0->field_30 = 1;
    return 0;
}
