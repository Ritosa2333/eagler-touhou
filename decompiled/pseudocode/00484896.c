// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x484896; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_484896_0 {
    char padding_0[4];
    unsigned int field_4;
} st_484896_0;

typedef struct st_484896_1 {
    char padding_0[108];
    struct st_484896_0 *field_6c;
    unsigned int field_70;
} st_484896_1;

extern unsigned int g_4ad9d4;
extern char g_4adab8;

int sub_484896(void)
{
    st_484896_1 *v2;  // ecx
    st_484896_0 *v3;  // eax
    unsigned int v0;  // [bp+0x0]

    v2 = sub_475467();
    v3 = v2->field_6c;
    if (v3 != *((int *)&g_4adab8) && !(g_4ad9d4 & v2->field_70))
        v3 = sub_474181(v0);
    return v3->field_4;
}
