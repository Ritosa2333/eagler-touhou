// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x406ce0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_406ce0_0 {
    char padding_0[60];
    unsigned int field_3c;
} st_406ce0_0;

extern unsigned int g_406db0[4];
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;

int sub_406ce0(void)
{
    st_406ce0_0 *v1;  // eax
    int v2;  // edi

    if (!v1->field_3c)
        return;
    if (v1->field_3c == 1 && g_4b0c94 + g_4b0c90 * 2 <= 5)
        goto *((void *)(g_406db0[2 * g_4b0c90 + g_4b0c94]));
    sub_464a80(v2);
    return;
}
