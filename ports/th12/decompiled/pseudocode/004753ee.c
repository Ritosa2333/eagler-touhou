// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4753ee; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4753ee_0 {
    unsigned int field_0;
    unsigned int field_4;
} st_4753ee_0;

extern int g_4adac8;
extern int g_4b4108;

st_4753ee_0 * sub_4753ee(void)
{
    unsigned int v2;  // edi
    enum WIN32_ERROR v3;  // eax
    unsigned int *v4;  // eax
    st_4753ee_0 *v5;  // esi
    unsigned int *v6;  // eax
    unsigned int ch;  // eax
    unsigned int v0;  // [bp-0x8]

    v0 = v2;
    v3 = GetLastError();
    v4 = sub_475279(g_4adac8);
    v5 = v4(g_4adac8);
    if (!v5)
    {
        v5 = sub_477813(1, 532);
        if (v5)
        {
            v6 = sub_4751de(g_4b4108, g_4adac8, v5);
            if (v6(g_4adac8, v5))
            {
                sub_475307(v5, 0);
                ch = GetCurrentThreadId();
                v5->field_4 = 0xffffffff;
                v5->field_0 = ch;
            }
            else
            {
                sub_46c941(v5);
                v5 = NULL;
            }
        }
    }
    SetLastError(v3);
    return v5;
}
