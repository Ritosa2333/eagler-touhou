// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43f1e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43f1e0_0 {
    char padding_0[1457];
    char field_5b1;
} st_43f1e0_0;

extern st_43f1e0_0 *g_4b451c;

unsigned int sub_43f1e0(void)
{
    unsigned int v1;  // esi
    unsigned int v2;  // edi
    unsigned int i;  // edi
    unsigned int iter;  // eax
    unsigned int v5;  // edx
    unsigned int j;  // edx
    unsigned int v7;  // ecx
    unsigned int k;  // ecx

    memset(&g_4b451c[0x55].padding_0[1456], 0x11, 16);
    v1 = &g_4b451c->field_5b1;
    v2 = 6;
    do
    {
        i = v2;
        iter = v1;
        v5 = 4;
        do
        {
            j = v5;
            v7 = 6;
            do
            {
                k = v7;
                *((char *)iter) = 1;
                iter += 8;
                v7 = k - 1;
            } while (k != 1);
            v5 = j - 1;
        } while (j != 1);
        v1 += 17908;
        v2 = i - 1;
    } while (i != 1);
    return iter;
}
