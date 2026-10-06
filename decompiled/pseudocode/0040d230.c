// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40d230; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40d230_1 {
    char padding_0[120];
    int field_78;
    char field_7c;
} st_40d230_1;

typedef struct st_40d230_0 {
    char padding_0[100];
    char field_64;
    char padding_65[3];
    unsigned int field_68;
    char padding_6c[1322];
    unsigned short field_596;
    char padding_598[1220];
    char field_a5c;
} st_40d230_0;

extern st_40d230_0 *g_4b43c8;
extern st_40d230_1 *g_4b43cc;

void sub_40d230(unsigned int a0)
{
    unsigned int v2;  // esi
    unsigned int v3;  // edi
    unsigned int v4;  // edi
    unsigned int v5;  // ebx
    unsigned int v0;  // [bp-0x4]

    v0 = a0;
    v2 = &g_4b43c8->field_64;
    if (g_4b43cc->field_7c & 1 && g_4b43cc->field_78 >= 96 && g_4b43cc->field_78 <= 99)
        return;
    v3 = 2000;
    do
    {
        v4 = v3;
        if (*((short *)(v2 + 1330)) && *((short *)(v2 + 1330)) != 3 && (!v5 || !*((int *)(v2 + 4))))
            sub_40c8b0();
    } while ((v2 += 2552, v3 = v4 - 1, v4 != 1));
    return;
}
