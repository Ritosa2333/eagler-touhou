// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47c20c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4ade70;

void sub_47c20c(void)
{
    unsigned int v2;  // edi
    unsigned int i;  // edi
    unsigned int v4;  // edi
    unsigned int v0;  // [bp-0x8]

    v0 = v2;
    i = 0;
    do
    {
        v4 = i + 4;
        *((unsigned int *)&(&g_4ade70)[i]) = sub_475163(*((int *)&(&g_4ade70)[i]));
        i = v4;
    } while (i < 40);
    return;
}
