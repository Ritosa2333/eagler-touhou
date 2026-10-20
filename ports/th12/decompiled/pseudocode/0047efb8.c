// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47efb8; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47efb8_1 {
    unsigned int field_0;
    unsigned int field_4;
} st_47efb8_1;

typedef struct st_47efb8_0 {
    char field_0;
} st_47efb8_0;

extern st_47efb8_0 *g_4b4318;

int sub_47efb8(void)
{
    unsigned int v6;  // edx
    char v0;  // [bp-0x14]
    char v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]
    st_47efb8_1 *v3;  // [bp+0x4]

    if (g_4b4318->field_0)
    {
        if (g_4b4318->field_0 == 90)
        {
            g_4b4318 = g_4b4318 + 1;
            v3->field_0 = 0;
            v3->field_4 = v2 & 0xffff0000;
            return;
        }
        sub_47ec4a(&v0, " throw(", sub_47eedc(&v1, v3, 41));
    }
    else
    {
        sub_47e79b(sub_47e59a(" throw(", &v0, 1, v3, 41), v6, " throw(", &v0);
    }
    sub_47ec6e();
    return;
}
