// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x431b90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_431b90_0 {
    char padding_0[26];
    unsigned int field_1a;
    char field_1e;
} st_431b90_0;

extern int g_4b0c44;
extern unsigned int g_4b0ca8;
extern char g_4b0cb0;
extern char g_4b0cc4;

int sub_431b90(unsigned int a0)
{
    unsigned int v1;  // eax
    int v2;  // ebx
    unsigned int v11;  // ecx
    st_431b90_0 *idx;  // eax
    int v3;  // esi
    unsigned int v4;  // ecx
    int i;  // edx
    unsigned int v6;  // edi
    void* v7;  // eax
    void* j;  // esi
    void* iter;  // edi
    unsigned int v10;  // ecx
    unsigned int v0;  // [bp-0x10]

    v1 = g_4b0ca8;
    v2 = 0;
    v3 = g_4b0c44;
    v4 = g_4b0ca8 * 280 + a0 + 16;
    while (*((int *)v4) > g_4b0c44)
    {
        v4 += 28;
        if (v2 + 1 >= 10)
            return -0x1;
    }
    if (v2 >= 10)
        return -0x1;
    i = 9;
    if (v2 < 9)
    {
        v0 = v6;
        do
        {
            v7 = a0 + (i + v1 * 10) * 28;
            j = v7 - 12;
            iter = v7 + 16;
            v10 = 7;
            for (i -= 1; v10; j += 4)
            {
                v10 -= 1;
                *((int *)iter) = *((int *)j);
                iter += 4;
            }
            v1 = g_4b0ca8;
        } while (i > v2);
        v3 = g_4b0c44;
    }
    *((int *)(a0 + (v2 + v1 * 10) * 28 + 16)) = v3;
    *((char *)(a0 + (v2 + g_4b0ca8 * 10) * 28 + 21)) = g_4b0cc4;
    *((char *)(a0 + (v2 + g_4b0ca8 * 10) * 28 + 20)) = g_4b0cb0;
    sub_46d5b7(a0 + (v2 + g_4b0ca8 * 10) * 28 + 32);
    v11 = (v2 + g_4b0ca8 * 10) * 7;
    *((unsigned int *)(a0 + v11 * 4 + 22)) = 0x20202020;
    idx = a0 + v11 * 4 + 22;
    *((unsigned int *)&idx->padding_0[4]) = 0x20202020;
    idx->padding_0[8] = 0;
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        a0 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        a0 = nan;
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)(a0 + (v2 + g_4b0ca8 * 10) * 28 + 40)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    return v2;
}
