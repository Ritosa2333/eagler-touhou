// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43f0a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43f0a0_1 {
    unsigned int field_0;
} st_43f0a0_1;

typedef struct st_43f0a0_0 {
    char padding_0[964];
    unsigned short field_3c4;
    char padding_3c6[206];
    struct st_43f0a0_1 *field_494;
} st_43f0a0_0;

extern unsigned int g_4ce8cc;

int sub_43f0a0(void)
{
    unsigned int v2;  // esi
    unsigned int v3;  // ecx
    unsigned int v4;  // edi
    unsigned int v5;  // eax
    st_43f0a0_0 *v6;  // esi
    unsigned short v7;  // bx
    unsigned int v0;  // [bp-0x4]

    v0 = v2;
    if (!sub_461920(v3, g_4ce8cc, *((int *)(v4 + v5 * 4 + 712))))
        *((unsigned int *)(v4 + v5 * 4 + 712)) = 0;
    v6 = sub_462020();
    if (v6->field_494)
        v6->field_494();
    v6->field_3c4 = v7;
    sub_455630(v6);
    return;
}
