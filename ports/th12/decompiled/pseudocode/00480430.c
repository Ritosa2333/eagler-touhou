// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x480430; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_480430_0 {
    char field_0;
} st_480430_0;

extern st_480430_0 *g_4b4318;

int sub_480430(void)
{
    void* idx;  // esi
    int v6;  // edi
    int v7;  // esi
    int v8;  // ebx
    unsigned int v11;  // edx
    int v0;  // [bp-0x3c]
    char v1;  // [bp-0x1c]
    unsigned int v2[2];  // [bp-0x14]
    char v3;  // [bp-0xc]
    void* v4;  // [bp+0x4]

    idx = v4;
    *((char *)&idx[4]) = 0;
    *((unsigned int *)&idx[4]) = (int)idx[4] & 0xffff00ff;
    *((unsigned int *)idx) = 0;
    sub_47dde0(sub_48023a(&v3, 1, 0, v6, v7, v8));
    if (!(char)idx[4] && g_4b4318->field_0)
    {
        if (g_4b4318->field_0 != 64)
        {
            sub_47ec92(sub_4814b0(&v1, v2, "::", &v3, idx), v11, v2, "::");
            sub_47dde0(sub_47e9ae());
        }
        else
        {
            g_4b4318 = g_4b4318 + 1;
            return;
        }
    }
    if (g_4b4318->field_0 == 64)
    {
        g_4b4318 = g_4b4318 + 1;
        return;
    }
    if (g_4b4318->field_0)
    {
        v0 = 2;
    }
    else if (!*((int *)idx))
    {
        v0 = 1;
    }
    else
    {
        sub_47ec92(sub_47e1ce(1, v2, "::", &v1, idx), v11, 0x1, v2);
        sub_47dde0(sub_47e9ae());
        return;
    }
    sub_47e2f7(v0);
    return;
}
