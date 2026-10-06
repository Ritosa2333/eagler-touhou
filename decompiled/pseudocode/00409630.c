// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x409630; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_409630_0 {
    char padding_0[16];
    unsigned int field_10;
} st_409630_0;

extern st_409630_0 *g_4b43c8;

void sub_409630(void)
{
    unsigned int v3;  // esi
    int v0;  // [bp-0x8]
    int v1;  // [bp-0x4]

    sub_461be0(v0, v1);
    v3 = g_4b43c8 + 5;
    sub_477420(v3, 0, 5106552);
    g_4b43c8->field_10 = v3;
    *((unsigned short *)&g_4b43c8[255271].padding_0[10]) = 5;
    return;
}
