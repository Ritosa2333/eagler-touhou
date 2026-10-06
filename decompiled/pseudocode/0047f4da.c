// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47f4da; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47f4da_0 {
    char field_0;
} st_47f4da_0;

extern st_47f4da_0 *g_4b4318;

int sub_47f4da(void)
{
    unsigned int v6;  // edx
    char v7;  // 4105
    char v8;  // 4105
    char *iter;  // eax
    char v10;  // cl
    int v0;  // [bp-0x18]
    char v1;  // [bp-0x14]
    char v2;  // [bp-0xc], Other Possible Types: unsigned int
    unsigned int v3;  // [bp-0x8]
    unsigned int *v4;  // [bp+0x4]
    char *v5;  // [bp+0x8]

    sub_47e59a(&v2, v6, v5);
    v7 = g_4b4318->field_0;
    g_4b4318 = g_4b4318 + 1;
    if (v7 == 64 && !(v8 = g_4b4318->field_0, g_4b4318 = g_4b4318 + 1, v8 != 95))
    {
        g_4b4318 = g_4b4318 + 1;
        sub_47ecb6(&v1, 0);
        sub_47ecb6(&v1, 0);
        iter = &g_4b4318->field_0;
        v10 = g_4b4318->field_0;
        if (g_4b4318->field_0)
        {
            do
            {
            } while (v10 != 64 && (iter += 1, g_4b4318 = (st_47f4da_0 *)iter, v10 = *(iter), *(iter)));
            if (*(iter))
            {
                g_4b4318 = iter + 1;
                *(v4) = v2;
                v4[1] = v3;
                return;
            }
        }
        g_4b4318 = iter - 1;
        v0 = 1;
    }
    else
    {
        v0 = 2;
    }
    sub_47e1ce(v0);
    return;
}
