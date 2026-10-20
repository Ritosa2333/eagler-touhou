// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x430840; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_430840_0 {
    char padding_0[2364];
    unsigned int field_93c;
} st_430840_0;

extern int g_4b44fc;
extern int g_4b4500;
extern int g_4b4504;
extern unsigned int g_4ce8a4;

void sub_430840(void)
{
    unsigned int v2;  // edi
    st_430840_0 *v3;  // esi
    int v4;  // ebx
    unsigned int v0;  // [bp-0x4]

    v0 = v2;
    if (v3->field_93c == 1)
    {
        sub_461970(g_4b44fc, v4);
        sub_461970(g_4b4500);
        sub_461970(g_4b4504);
        g_4b44fc = 0;
        g_4b4500 = 0;
        g_4b4504 = 0;
        v3->field_93c = 2;
    }
    if (g_4ce8a4)
        g_4ce8a4 = 0;
    return;
}
