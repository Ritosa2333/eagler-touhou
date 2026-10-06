// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43dd50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b4524;

unsigned int * sub_43dd50(void)
{
    int v1;  // esi
    unsigned int *v2;  // esi
    int v3;  // ecx
    unsigned int *iter;  // eax
    int v5;  // ecx

    v2 = sub_46c9ea(2060, v1);
    if (v2)
    {
        sub_4027e0();
        v3 = 12;
        iter = v2 + 318;
        do
        {
            v5 = v3;
            *(iter) = *(iter) & 0xfffffffe;
            iter += 16;
            v3 = v5 - 1;
        } while (v5 - 1 >= 0);
        sub_477420(v2, 0, 2060);
        *(v2) = *(v2) | 2;
        g_4b4524 = v2;
    }
    else
    {
        v2 = NULL;
    }
    if (!sub_43db40())
        return v2;
    if (!v2)
        return NULL;
    sub_43dc30(v2);
    sub_46ca4f(v2);
    return NULL;
}
