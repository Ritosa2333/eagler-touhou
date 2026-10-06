// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x483e6d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_483e6d_0 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[176];
    int field_d4;
} st_483e6d_0;

extern int g_4adea8;

unsigned int sub_483e6d(st_483e6d_0 *a0)
{
    unsigned int v1;  // edi
    int v2;  // esi
    st_483e6d_0 *v3;  // edi
    unsigned int v0;  // [bp-0x10]

    v0 = v1;
    if (a0->field_20)
    {
        v2 = sub_477813(1, 184);
        if (!v2)
            return 1;
        if (sub_4838bd())
        {
            sub_483cd8(v2);
            sub_46c941(v2);
        }
        else
        {
            *((unsigned int *)(v2 + 180)) = 1;
            goto LABEL_483ec5;
        }
        return 1;
    }
    else
    {
        v2 = &g_4adea8;
LABEL_483ec5:
        v3 = &a0->field_d4;
        if (*((int *)&v3->padding_0[0]) != &g_4adea8)
            InterlockedDecrement(*((int *)&v3->padding_0[0]) + 180);
        *((int *)&v3->padding_0[0]) = v2;
        return 0;
    }
}
