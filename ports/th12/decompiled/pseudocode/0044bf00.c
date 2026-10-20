// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44bf00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44bf00_0 {
    char padding_0[4];
    HANDLE field_4;
    unsigned int field_8;
} st_44bf00_0;

char sub_44bf00(st_44bf00_0 *a0, unsigned int a1, Byte *a2, unsigned int count)
{
    unsigned int v3;  // esi
    unsigned int v0;  // [bp-0x8]
    st_44bf00_0 *v1;  // [bp-0x4], Other Possible Types: unsigned int
    unsigned int v2;  // [bp-0x4]

    v1 = a0;
    v1 = 0;
    if (a0->field_8 == 0x40000000)
    {
        v0 = v3;
        WriteFile(a0->field_4, a2, count, &v2, NULL);
        return count == v2;
    }
    return 0;
}
