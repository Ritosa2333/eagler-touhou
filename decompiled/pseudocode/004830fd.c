// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4830fd; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_4830fd(char *a0, unsigned int a1, char *a2, unsigned int a3)
{
    unsigned int v5;  // ebx
    unsigned int v6;  // esi
    unsigned int v7;  // edi
    unsigned int v8;  // edi
    char *node;  // edx
    char *iter;  // eax
    unsigned int v11;  // edi
    char v12;  // 4103
    unsigned int v13;  // edi
    char v14;  // 4103
    unsigned int v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x10]
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]
    unsigned int v4;  // [bp+0x10]

    v3 = v5;
    v2 = v6;
    v1 = v7;
    if (!a3)
    {
        if (a0)
            goto LABEL_483123;
        if (!a1)
            return 0;
    }
    else if (a0)
    {
LABEL_483123:
        v8 = a1;
        if (v8 > 0)
        {
            if (!a3)
            {
                *(a0) = 0;
                return 0;
            }
            node = a2;
            if (!node)
            {
                *(a0) = 0;
            }
            else
            {
                iter = a0;
                if (a3 == 0xffffffff)
                {
                    do
                    {
                        v11 = v8;
                        v12 = *(node);
                        *(iter) = *(node);
                        iter += 1;
                        node += 1;
                        v13 = v11;
                    } while (v12 && (v8 = v11 - 1, v13 = v8, v11 != 1));
                }
                else
                {
                    do
                    {
                        v4 = a3;
                        v14 = *(node);
                        *(iter) = *(node);
                        iter += 1;
                        node += 1;
                        a3 = v4;
                        v13 = v8;
                    } while (v14 && (v13 = v8 - 1, a3 = v4, v8 != 1 && (a3 = v4 - 1, v8 = v13, v4 != 1)));
                    if (!a3)
                        *(iter) = 0;
                }
                if (v13)
                {
                    return 0;
                }
                else if (a3 == 0xffffffff)
                {
                    v0 = 80;
                    *(&a0[a1] - 1) = 0;
                    return 80;
                }
                else
                {
                    *(a0) = 0;
                    v0 = 0x22;
                    *((unsigned int *)sub_46e96f()) = 0x22;
                    sub_470f2d(0, 0, 0, 0, 0);
                    return 0x22;
                }
            }
        }
    }
    v0 = 22;
    *((unsigned int *)sub_46e96f()) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    return 22;
}
