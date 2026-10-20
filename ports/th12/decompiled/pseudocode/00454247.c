// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x454247; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_454247_0 {
    char padding_0[120];
    unsigned int field_78;
} st_454247_0;

extern char g_4ceacb;
extern st_454247_0 *g_4d4754;

int sub_454247(void)
{
    unsigned int v1;  // ebx

    if (g_4ceacb != 1)
        goto LABEL_0x454282;
    if (g_4d4754->field_78 != v1)
        goto LABEL_0x4543a4;
    sub_4664f0();
}
