// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x453f10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_453f10_3 {
    struct st_453f10_4 *field_0;
    char padding_4[16];
    unsigned int field_14;
} st_453f10_3;

typedef struct st_453f10_4 {
    struct st_453f10_0 *field_0;
} st_453f10_4;

typedef struct st_453f10_1 {
    unsigned int field_0;
} st_453f10_1;

typedef struct st_453f10_0 {
    char padding_0[36];
    struct st_453f10_1 *field_24;
    char padding_28[32];
    struct st_453f10_1 *field_48;
} st_453f10_0;

extern unsigned int g_4cf508;
extern unsigned int g_4cf538;
extern st_453f10_3 g_4d0e70;
extern st_453f10_3 g_4d1410;

int sub_453f10(void)
{
    int v2;  // eax
    st_453f10_3 *iter;  // esi
    unsigned int v4;  // eax
    unsigned int v5;  // eax
    int idx;  // edx
    unsigned int *v7;  // eax
    char v0;  // [bp-0x4], Other Possible Types: unsigned int

    if (v2 < 0)
    {
        iter = &g_4d0e70.field_0;
        do
        {
            v4 = iter->field_0;
            iter->field_14 = 0;
            if (v4)
            {
                (*((int *)(*((int *)v4) + 36)))(v4, &v0);
                v5 = iter->field_0;
                iter->field_14 = v0 & 1;
                (*((int *)(*((int *)v5) + 72)))(v5);
            }
        } while ((iter += 24, iter < &g_4d1410.field_0));
        return;
    }
    else
    {
        idx = 0;
        v7 = &g_4cf508;
        while (1)
        {
            if (*(v7) >= 0)
            {
                if (*(v7) == v2)
                    break;
                v7 += 1;
                idx += 1;
                if (v7 >= &g_4cf538)
                    return;
            }
            else if (idx < 12)
            {
                (&g_4cf508)[idx] = v2;
                break;
            }
            else
            {
                return;
            }
        }
        (&g_4cf538)[idx] = 0xffffffff;
        return;
    }
}
