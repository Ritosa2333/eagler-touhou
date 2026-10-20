// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41cad0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b43e0;

void sub_41cad0(void)
{
    int v1;  // esi
    unsigned int *v2;  // esi

    v2 = sub_46c9ea(140, v1);
    if (v2)
    {
        sub_477420(v2, 0, 140);
        *(v2) = *(v2) | 2;
        g_4b43e0 = v2;
        sub_41cb01();
    }
    else
    {
        sub_41cb01();
    }
    return;
}
