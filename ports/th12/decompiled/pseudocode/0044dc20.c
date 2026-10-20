// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44dc20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44dc20_0 {
    unsigned short field_0;
} st_44dc20_0;

extern unsigned int g_4b0e48;
extern unsigned int g_4b0e58;
extern st_44dc20_0 *g_4b0e68;

char sub_44dc20(unsigned int a0)
{
    unsigned int v0;  // eax
    unsigned int v1;  // ecx
    unsigned short *iter;  // edx
    unsigned int i;  // ecx
    unsigned int v4;  // esi
    unsigned int v5;  // esi
    unsigned int v6;  // ecx
    unsigned int v7;  // eax
    unsigned int j;  // esi

    v0 = a0 * g_4b0e58;
    v1 = g_4b0e48 - 21;
    iter = &g_4b0e68->field_0;
    if (g_4b0e48 == 21)
    {
        j = 3;
        if (v0 <= 3)
            return 1;
        do
        {
            *(j + (char *)g_4b0e68) = ~(*(j + (char *)g_4b0e68));
            j += 4;
        } while (j < v0);
        return 1;
    }
    else if (v1 != 4)
    {
        if (v1 != 5)
            return 0;
        i = 1;
        if (v0 <= 1)
            return 1;
        do
        {
            *(i + (char *)g_4b0e68) = *(i + (char *)g_4b0e68) ^ 240;
            i += 2;
        } while (i < v0);
        return 1;
    }
    else
    {
        if (v0 > 0)
        {
            v4 = (v0 - 1 >> 1) + 1;
            do
            {
                v5 = v4;
                v6 = *(iter);
                v7 = (~(v6) ^ v6) & 0x7fff ^ ~(v6);
                *(iter) = v7;
                if (!(v7 & 0x8000))
                    *(iter) = v7 & 0x8000;
            } while ((iter += 2, v4 = v5 - 1, v5 != 1));
            return 1;
        }
        else
        {
            return 1;
        }
    }
}
