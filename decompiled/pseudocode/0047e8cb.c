// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e8cb; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e8cb_1 {
    char padding_0[4];
    unsigned int field_4;
} st_47e8cb_1;

typedef struct st_47e8cb_0 {
    char field_0;
} st_47e8cb_0;

extern st_47e8cb_0 *g_4b4318;
extern unsigned int g_4b4328;

int sub_47e8cb(void)
{
    unsigned int v3;  // ecx
    unsigned int v4;  // eax
    unsigned int v5;  // eax
    int v6;  // ecx, Other Possible Types: unsigned int
    unsigned int v7;  // eax
    unsigned int v8;  // eax
    unsigned int v9;  // eax
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    st_47e8cb_1 *v2;  // [bp+0x4]

    v1 = v3;
    v0 = v3;
    if (g_4b4318->field_0)
    {
        v4 = g_4b4318->field_0 - 65;
        g_4b4318 = g_4b4318 + 1;
        if (v4 <= 12)
        {
            v0 = 0;
            v1 &= 0xffff0000;
            if (!((char)~(g_4b4328 >> 1) & 1))
            {
                *((unsigned int *)&v2->padding_0[0]) = v0;
                v2->field_4 = v1;
                return;
            }
            v5 = v4 & 254;
            if (v5)
            {
                v6 = 2;
                v6 = 2;
                v7 = v5 - 2;
                if (v5 != 2)
                {
                    v8 = v7 - 2;
                    if (v7 != 2)
                    {
                        v9 = v8 - 2;
                        if (v8 == 2)
                        {
                            v6 = 3;
                        }
                        else if (v9 == 2)
                        {
                            v6 = 5;
                        }
                        else if (v9 == 6)
                        {
                            v6 = 6;
                        }
                        else
                        {
                            *((unsigned int *)&v2->padding_0[0]) = v0;
                            v2->field_4 = v1;
                            return;
                        }
                    }
                    else
                    {
                        v6 = 4;
                    }
                }
                else
                {
                    v6 = 2;
                }
            }
            else
            {
                v6 = 1;
            }
            sub_47e896(sub_47dc0b(v6));
            *((unsigned int *)&v2->padding_0[0]) = v0;
            v2->field_4 = v1;
            return;
        }
        v6 = 2;
    }
    else
    {
        v6 = 1;
    }
    sub_47e1ce(v6);
    return;
}
