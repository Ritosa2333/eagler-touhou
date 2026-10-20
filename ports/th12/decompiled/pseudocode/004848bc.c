// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4848bc; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4848bc_1 {
    char padding_0[108];
    struct st_4848bc_0 *field_6c;
    unsigned int field_70;
} st_4848bc_1;

typedef struct st_4848bc_0 {
    char padding_0[8];
    unsigned int field_8;
} st_4848bc_0;

extern unsigned int g_4ad9d4;
extern char g_4adab8;

int sub_4848bc(void)
{
    st_4848bc_1 *v1;  // ecx
    st_4848bc_0 *v2;  // eax

    v1 = sub_475467();
    v2 = v1->field_6c;
    if (v2 != *((int *)&g_4adab8) && !(g_4ad9d4 & v1->field_70))
        v2 = sub_474181();
    return v2->field_8;
}
