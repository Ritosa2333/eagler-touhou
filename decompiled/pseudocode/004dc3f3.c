// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dc3f3; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned int sub_4dc3f3(unsigned int a0, unsigned int a1)
{
    unsigned int v2;  // edx
    char *v13;  // esi
    unsigned int v11;  // 4102
    unsigned int v12;  // 4109
    char *v3;  // esi
    char *v4;  // esi
    unsigned int v5;  // edx
    char v6;  // al
    unsigned int v7;  // cc_op
    unsigned int v8;  // cc_dep1
    unsigned int v9;  // 4119
    unsigned int v0;  // [bp-0x4]

    v0 = a1;
    v2 = 2621121678;
    while (1)
    {
        v4 = v3 + 1;
        if (!*(v3))
            break;
        v5 = _INSERT(v2, 0, (char)v2 ^ *(v3));
        v6 = 8;
        do
        {
            v7 = 27;
            v8 = v5 >> 1;
            v2 = v5 >> 1;
            v9 = _ccall(2, 27, v5 >> 1, v5, 0);
            if (v9 & 1)
            {
                v7 = 15;
                v8 = v2 ^ 3249009562;
                v5 = 0;
                v2 ^= 3249009562;
            }
            v11 = _ccall(v7, v8, v5, 0);
            v12 = _ccall(4, 19, v6 - 1, 0, v11);
            v13 = v4;
            v5 = v2;
            v6 -= 1;
        } while (!(v12 & 1));
    }
    return v2;
}
