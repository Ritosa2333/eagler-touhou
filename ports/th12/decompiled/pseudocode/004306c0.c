// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4306c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4306c0_0 {
    char padding_0[2364];
    unsigned int field_93c;
} st_4306c0_0;

extern int g_4b44fc;
extern int g_4b4500;
extern int g_4b4504;
extern unsigned int g_4ce8a4;

void sub_4306c0(void)
{
    unsigned int v3;  // ebx
    unsigned int v4;  // esi
    st_4306c0_0 *v5;  // edi
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    v1 = v3;
    v0 = v4;
    if (v5->field_93c == 1)
    {
        sub_461970(g_4b44fc);
        sub_461970(g_4b4500);
        sub_461970(g_4b4504);
        g_4b44fc = 0;
        g_4b4500 = 0;
        g_4b4504 = 0;
        v5->field_93c = 0;
    }
    if (g_4ce8a4)
        g_4ce8a4 = 0;
    return;
}
