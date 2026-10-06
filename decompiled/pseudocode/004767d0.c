// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4767d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned long long sub_4767d0(unsigned int a0, unsigned int a1, unsigned long long a2, unsigned int a3)
{
    unsigned long long v0;  // ebx
    unsigned int v1;  // eax
    unsigned long long iter;  // esi
    unsigned int v11;  // ecx
    unsigned int v12;  // edx
    unsigned int v13;  // edx
    unsigned int i;  // ecx
    unsigned int v3;  // edx
    unsigned int v5;  // 4165
    unsigned long long v6;  // 4166
    unsigned int v7;  // edx
    unsigned int v8;  // 4168
    unsigned long long v9;  // 4169

    if (!a3)
        return (unsigned int)(CONCAT(a1 % a2, a0) / a2);
    v0 = a2;
    v1 = a0;
    i = a3;
    v3 = a1;
    do
    {
        v5 = _ccall(27, i >> 1, i, 0);
        v6 = _ccall(v0, 1, v5, 4);
        v0 = (unsigned int)v6;
        v7 = v3 >> 1;
        v8 = _ccall(27, v3 >> 1, v3, 0);
        v9 = _ccall(v1, 1, v8, 4);
        v1 = v9;
        i >>= 1;
        v3 = v7;
    } while (i);
    iter = (unsigned int)(CONCAT(v7, v1) / v0);
    v11 = iter * a3;
    v12 = a2 * iter >> 32;
    v13 = v12 + v11;
    if (v12 + v11 < 0)
    {
        iter -= 1;
    }
    else if (v13 > a1)
    {
        iter -= 1;
    }
    else if (v13 >= a1 && (unsigned int)(a2 * iter) > a0)
    {
        iter -= 1;
    }
    return iter;
}
