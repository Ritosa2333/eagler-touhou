// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47eedc; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47eedc_0 {
    char field_0;
} st_47eedc_0;

extern st_47eedc_0 *g_4b4318;
extern unsigned int g_4b4328;

int sub_47eedc(void)
{
    unsigned int v4;  // eax
    int v5;  // eax
    unsigned int v6;  // edx
    unsigned int *v7;  // eax
    unsigned int *v8;  // ecx
    unsigned int *v9;  // eax
    char *v10;  // eax
    unsigned int v11;  // edx
    unsigned int v0[2];  // [bp-0x14]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]
    unsigned int *v3;  // [bp+0x4]

    v4 = g_4b4318->field_0;
    if (v4 != 88)
    {
        if (v4 != 90)
        {
            sub_47eaac(&v1);
            if (!(char)v2 && g_4b4318->field_0)
            {
                if (g_4b4318->field_0 != 64)
                {
                    if (g_4b4318->field_0 != 90)
                    {
                        sub_47e1ce(2);
                        return;
                    }
                    g_4b4318 = g_4b4318 + 1;
                    v5 = ",...";
                    if (!((char)~(g_4b4328 >> 18) & 1))
                        v5 = ",<ellipsis>";
                    v7 = sub_47ec92(&v1, v6, v0, v5);
                    v8 = v3;
                    *(v8) = *(v7);
                    v8[1] = v7[1];
                    return;
                }
                g_4b4318 = g_4b4318 + 1;
            }
            v9 = v3;
            *(v9) = v1;
            v9[1] = v2;
            return;
        }
        g_4b4318 = g_4b4318 + 1;
        v10 = "...";
        if (!((char)~(g_4b4328 >> 18) & 1))
            v10 = "<ellipsis>";
    }
    else
    {
        g_4b4318 = g_4b4318 + 1;
        v10 = "void";
    }
    sub_47e59a(v3, v11, v10);
    return;
}
