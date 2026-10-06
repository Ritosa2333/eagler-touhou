// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48e0d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned long long sub_48e0d0(unsigned int a0, unsigned int a1, unsigned long long a2, unsigned int a3)
{
    unsigned int v0;  // edi
    int v1;  // ebp
    unsigned int v10;  // edx
    unsigned int v11;  // 4168
    unsigned long long v12;  // 4169
    unsigned int v13;  // ecx
    unsigned int v14;  // edx
    unsigned long long v16;  // eax
    unsigned int i;  // eax
    unsigned long long v3;  // esi
    unsigned int v4;  // eax
    unsigned int v5;  // edx
    unsigned long long v6;  // ecx
    unsigned int v8;  // 4165
    unsigned long long v9;  // 4166

    v0 = 0;
    v1 = 0;
    if (a1 < 0)
    {
        v0 = 1;
        v1 = 1;
        a1 = -(a1) - (0 < a0);
        a0 = -(a0);
    }
    i = a3;
    if (a3 < 0)
    {
        v0 += 1;
        i = -(i) - (0 < a2);
        a3 = i;
        a2 = -(a2);
    }
    if (!i)
    {
        v3 = (unsigned int)(CONCAT(a1 % a2, a0) / a2);
    }
    else
    {
        v4 = a0;
        v5 = a1;
        v6 = a2;
        do
        {
            v8 = _ccall(27, i >> 1, i, 0);
            v9 = _ccall(v6, 1, v8, 4);
            v6 = (unsigned int)v9;
            v10 = v5 >> 1;
            v11 = _ccall(27, v5 >> 1, v5, 0);
            v12 = _ccall(v4, 1, v11, 4);
            v4 = v12;
            v5 = v10;
            i >>= 1;
        } while (i);
        v3 = (unsigned int)(CONCAT(v10, v4) / v6);
        v13 = v3 * a3;
        v14 = a2 * v3 >> 32;
        if (v14 + v13 < 0 || v14 + v13 > a1 || v14 + v13 >= a1 && (unsigned int)(a2 * v3) > a0)
            v3 -= 1;
    }
    v16 = v3;
    if (v0 == 1)
        v16 = -(v16);
    return v16;
}
