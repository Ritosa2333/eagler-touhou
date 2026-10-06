// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x477550; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned int sub_477550(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    int v0;  // edi
    unsigned int i;  // eax
    unsigned int v10;  // 4168
    unsigned long long v11;  // 4169
    unsigned long long v12;  // eax
    unsigned int v13;  // ecx
    unsigned int v14;  // edx
    unsigned int v15;  // eax
    unsigned int v2;  // eax
    unsigned int v3;  // eax
    unsigned int v4;  // edx
    unsigned int v5;  // ecx
    unsigned int v7;  // 4165
    unsigned long long v8;  // 4166
    unsigned int v9;  // edx

    v0 = 0;
    if (a1 < 0)
    {
        v0 = 1;
        a1 = -(a1) - (0 < a0);
        a0 = -(a0);
    }
    i = a3;
    if (a3 < 0)
    {
        i = -(i) - (0 < a2);
        a3 = i;
        a2 = -(a2);
    }
    if (!i)
    {
        v2 = CONCAT(a1 % a2, a0) % a2;
        if (v0 - 1 < 0)
            return CONCAT(a1 % a2, a0) % a2;
    }
    else
    {
        v3 = a0;
        v4 = a1;
        v5 = a2;
        do
        {
            v7 = _ccall(27, i >> 1, i, 0);
            v8 = _ccall(v5, 1, v7, 4);
            v5 = v8;
            v9 = v4 >> 1;
            v10 = _ccall(27, v4 >> 1, v4, 0);
            v11 = _ccall(v3, 1, v10, 4);
            v3 = v11;
            i >>= 1;
            v4 = v9;
        } while (i);
        v12 = (unsigned int)(CONCAT(v9, v3) / v5);
        v13 = v12 * a3;
        v14 = v12 * a2 >> 32;
        v15 = v12 * a2;
        if (v14 + v13 < 0 || v14 + v13 > a1 || v14 + v13 >= a1 && v15 > a0)
            v15 -= a2;
        v2 = v15 - a0;
        if (v0 - 1 >= 0)
            return v15 - a0;
    }
    return -(v2);
}
