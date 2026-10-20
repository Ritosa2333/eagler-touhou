// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43bae0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43bae0_0 {
    char padding_0[16];
    unsigned int field_10;
    char padding_14[444];
    int field_1d0;
} st_43bae0_0;

extern unsigned int g_4b44e8;
extern unsigned int g_4d48b8;

int sub_43bae0(void)
{
    st_43bae0_0 *v1;  // eax

    if (g_4b44e8 && v1->field_10 == 1 && g_4d48b8 & 0x200 && (int)(v1->field_1d0 % 6))
        return;
    return;
}
