// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x477afe; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4adb9c;

int sub_477afe(void)
{
    unsigned int v2;  // esi
    unsigned int *v3;  // eax
    unsigned int v4;  // edx
    unsigned int v0;  // [bp-0x8]
    unsigned int *v1;  // [bp+0x4]

    v0 = v2;
    v3 = v1;
    do
    {
    } while (*(v3) != v4 && (v3 += 12, v3 < g_4adb9c * 12 + v1));
    if (v3 < &v1[3 * g_4adb9c] && *(v3) == v4)
        return;
    return;
}
