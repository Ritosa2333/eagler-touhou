// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4774a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned long long sub_4774a0(unsigned int a0, unsigned int a1, unsigned long long a2, unsigned int a3)
{
    unsigned int v0;  // edi
    unsigned int i;  // eax
    unsigned int v10;  // 4168
    unsigned long long v11;  // 4169
    unsigned int v12;  // ecx
    unsigned int v13;  // edx
    unsigned int v14;  // edx
    unsigned long long iter;  // eax
    unsigned int v3;  // eax
    unsigned int v4;  // edx
    unsigned long long v5;  // ecx
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
        v0 += 1;
        i = -(i) - (0 < a2);
        a3 = i;
        a2 = -(a2);
    }
    if (!i)
    {
        iter = (unsigned int)(CONCAT(a1 % a2, a0) / a2);
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
            v5 = (unsigned int)v8;
            v9 = v4 >> 1;
            v10 = _ccall(27, v4 >> 1, v4, 0);
            v11 = _ccall(v3, 1, v10, 4);
            v3 = v11;
            i >>= 1;
            v4 = v9;
        } while (i);
        iter = (unsigned int)(CONCAT(v9, v3) / v5);
        v12 = iter * a3;
        v13 = a2 * iter >> 32;
        v14 = v13 + v12;
        if (v13 + v12 < 0)
        {
            iter -= 1;
        }
        else if (v14 > a1)
        {
            iter -= 1;
        }
        else if (v14 >= a1 && (unsigned int)(a2 * iter) > a0)
        {
            iter -= 1;
        }
    }
    if (v0 == 1)
        iter = -(iter);
    return iter;
}
