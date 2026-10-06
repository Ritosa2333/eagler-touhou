// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43c8c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43c8c0_0 {
    char padding_0[487];
    unsigned int field_1e7;
} st_43c8c0_0;

extern unsigned int g_4d4f30;
extern char g_4d4f34;

int sub_43c8c0(void)
{
    st_43c8c0_0 *v1;  // eax

    g_4d4f30 = *((int *)&(v1->padding_0)[1]);
    g_4d4f34 = 0;
    return;
}
