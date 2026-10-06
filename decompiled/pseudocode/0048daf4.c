// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48daf4; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned int sub_48daf4(unsigned long long a0, unsigned int a1, unsigned int a2, unsigned long long a3, unsigned int iter)
{
    unsigned int v4;  // ebx
    void* v5;  // edi
    int v13;  // esi, Other Possible Types: unsigned int
    unsigned int v15;  // edx
    unsigned int v16;  // ecx
    unsigned int v17;  // ecx
    void* node;  // esi
    char v19;  // 4111
    unsigned int *v6;  // eax
    int v7;  // ebx
    unsigned int v8;  // esi
    unsigned long long v9;  // edx
    unsigned int v10;  // eax
    void* iter1;  // esi
    unsigned long long v12;  // edx
    unsigned int v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x18]
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]

    v4 = 0;
    if (!v5 || a2 <= 0)
    {
LABEL_48db04:
        v6 = sub_46e96f(v13, v7);
        v0 = 22;
        goto LABEL_48db0b;
    }
    else
    {
        *((char *)v5) = 0;
        if (a2 <= (unsigned int)((iter) + 1))
        {
            v6 = sub_46e96f();
            v0 = 0x22;
LABEL_48db0b:
            v8 = v0;
            *(v6) = v8;
        }
        else
        {
            if (a3 - 2 > 0x22)
                goto LABEL_48db04;
            v9 = a0;
            v10 = a1;
            v3 = 0;
            iter1 = v5;
            if (iter)
            {
                v12 = -(v9);
                *((char *)v5) = 45;
                iter1 = v5 + 1;
                v3 = 1;
                v10 = -(v10 + (0 < v9));
                v9 = v12;
            }
            iter = iter1;
            a1 = 0;
            while (1)
            {
                v2 = v4;
                v4 = v15;
                v9 = (unsigned int)sub_47c890(v9, v10, a3, a1, v13);
                v10 = v4;
                *((char *)iter1) = (v16 <= 9 ? (char)v16 + 48 : (char)v16 + 87);
                iter1 += 1;
                v17 = v3 + 1;
                v3 = v17;
                if (!v10 && !v9)
                    break;
                if (v17 >= a2)
                {
                    v17 = v3;
                    break;
                }
            }
            if (v17 >= a2)
            {
                *((char *)v5) = 0;
                v1 = 0x22;
                *((unsigned int *)sub_46e96f()) = 0x22;
                v8 = 0x22;
            }
            else
            {
                *((char *)iter1) = 0;
                node = iter1 - 1;
                do
                {
                    v19 = *((char *)node);
                    *((char *)node) = *((char *)iter);
                    node -= 1;
                    *((char *)iter) = v19;
                    iter += 1;
                } while (iter < node);
                return 0;
            }
        }
        sub_470f2d(0, 0, 0, 0, 0);
        return v8;
    }
}
