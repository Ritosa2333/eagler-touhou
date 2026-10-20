// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4588f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4588f0_1 {
    int field_0;
} st_4588f0_1;

typedef struct st_4588f0_0 {
    char padding_0[964];
    unsigned short field_3c4;
    char padding_3c6[46];
    struct st_4588f0_1 *field_3f4;
    char padding_3f8[156];
    struct st_4588f0_1 *field_494;
} st_4588f0_0;

extern char g_4b50c0;
extern unsigned int g_4ce8cc;

int sub_4588f0(void)
{
    unsigned int v1;  // eax
    unsigned int v2;  // ebx
    st_4588f0_0 *v3;  // ecx
    st_4588f0_0 *iter;  // esi
    unsigned int v5;  // ecx
    unsigned int v6;  // ebx
    int v7;  // eax
    unsigned short v8;  // di

    v2 = v1;
    iter = v3;
    if (!v2)
        return;
    v5 = g_4ce8cc;
    do
    {
        v6 = v2;
        if (iter->field_3f4)
        {
            v7 = iter->field_3f4->field_0;
            if (v7 >= 0 && *((int *)&(&g_4b50c0)[4 * v7 + v5]) && *((int *)(*((int *)&(&g_4b50c0)[4 * v7 + v5]) + 288)))
            {
                if (iter->field_494)
                {
                    iter->field_494();
                    v5 = g_4ce8cc;
                }
                iter->field_3c4 = v8;
            }
        }
    } while ((iter += 1204, v2 = v6 - 1, v6 != 1));
    return;
}
