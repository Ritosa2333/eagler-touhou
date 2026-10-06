// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48d9ac; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_48d9ac(void)
{
    void* v10;  // ecx
    int v11;  // edi
    char v20;  // 4106
    unsigned int *v12;  // eax
    void* iter;  // ecx
    unsigned int v14;  // eax
    unsigned int v15;  // eax
    void* iter1;  // edi
    unsigned int v17;  // eax
    unsigned int v18;  // edx
    void* node;  // ecx
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x14]
    int v2;  // [bp-0x10]
    int v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x8]
    unsigned int v5;  // [bp-0x8]
    char v6;  // [bp+0x0]
    unsigned int v7;  // [bp+0x4]
    unsigned int v8;  // [bp+0x8]
    unsigned int v9;  // [bp+0xc]

    if (!v10)
    {
        v1 = 22;
        *((unsigned int *)sub_46e96f(v2, v3, v10, &v6)) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        return;
    }
    if (v7 <= NULL)
    {
LABEL_48d9e0:
        v12 = sub_46e96f(v11);
        v0 = 22;
        goto LABEL_48d9e7;
    }
    *((char *)v10) = 0;
    if (v7 <= (unsigned int)((v9) + 1))
    {
LABEL_48da0e:
        v12 = sub_46e96f();
        v0 = 0x22;
LABEL_48d9e7:
        *(v12) = v0;
        sub_470f2d(0, 0, 0, 0, 0);
    }
    else
    {
        if (v8 - 2 > 0x22)
            goto LABEL_48d9e0;
        v5 = 0;
        iter = v10;
        if (v9)
        {
            *((char *)v10) = 45;
            iter = v10 + 1;
            v5 = 1;
            v15 = -(v14);
        }
        iter1 = iter;
        do
        {
            v4 = v5;
            v17 = v15 / v8;
            v18 = v15 % v8;
        } while ((*((char *)iter) = (v18 <= 9 ? (char)v18 + 48 : (char)v18 + 87), iter += 1, v5 = v4 + 1, v17 > 0 && (v15 = v17, v4 + 1 < v7)));
        if (v4 + 1 >= v7)
        {
            *((char *)v10) = 0;
            goto LABEL_48da0e;
        }
        else
        {
            *((char *)iter) = 0;
            node = iter - 1;
            do
            {
                v20 = *((char *)node);
                *((char *)node) = *((char *)iter1);
                node -= 1;
                *((char *)iter1) = v20;
                iter1 += 1;
            } while (iter1 < node);
        }
    }
    return;
}
