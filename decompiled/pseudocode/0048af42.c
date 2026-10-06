// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48af42; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern void* g_4b3d80;
extern unsigned int g_4b3d88;
extern unsigned int g_4d6428;

unsigned int sub_48af42(unsigned int a0)
{
    void* iter;  // esi
    unsigned int v2;  // edi
    unsigned int v3;  // edi
    unsigned int v0;  // [bp-0xc]

    iter = g_4b3d80;
    if (!g_4d6428)
        return 0;
    v0 = v2;
    if (g_4b3d80 || g_4b3d88 && !sub_48e343() && !(iter = g_4b3d80, !g_4b3d80))
    {
        if (a0)
        {
            for (v3 = sub_475860(a0); *((int *)iter); iter += 4)
            {
                if (sub_475860(*((int *)iter)) > v3 && *((char *)(*((int *)iter) + v3)) == 61 && !sub_48e329(*((int *)iter), a0, v3))
                    return *((int *)iter) + v3 + 1;
            }
        }
    }
    return 0;
}
