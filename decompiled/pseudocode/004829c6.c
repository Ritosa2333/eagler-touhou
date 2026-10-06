// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4829c6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4829c6_0 {
    char padding_0[4];
    unsigned int field_4;
} st_4829c6_0;

extern unsigned int g_4adb9c;

int sub_4829c6(void)
{
    unsigned int v2;  // esi
    st_4829c6_0 *v3;  // eax
    unsigned int v4;  // edx
    unsigned int v0;  // [bp-0x8]
    st_4829c6_0 *v1;  // [bp+0x4]

    v0 = v2;
    v3 = v1;
    do
    {
    } while (v3->field_4 != v4 && (v3 += 12, v3 < g_4adb9c * 12 + v1));
    if (v3 < 12 * g_4adb9c + (char *)v1 && v3->field_4 == v4)
        return;
    return;
}
