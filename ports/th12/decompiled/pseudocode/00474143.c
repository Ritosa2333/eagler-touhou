// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x474143; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_474143_0 {
    unsigned int field_0;
} st_474143_0;

extern unsigned int g_4ad9e0;

int sub_474143(void)
{
    unsigned int *v1;  // edi
    st_474143_0 **v2;  // eax
    unsigned int *v3;  // esi

    if (!v1)
    {
        return;
    }
    else if (v2)
    {
        v3 = &*(v2)->field_0;
        if (v3 == v1)
            return;
        *(v2) = v1;
        sub_473ff5(v1);
        if (!v3)
            return;
        sub_474084(v3);
        if (*(v3))
            return;
        if (v3 == &g_4ad9e0)
            return;
        sub_473eac(v3);
        return;
    }
    else
    {
        return;
    }
}
