// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x483089; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_483089(char *a0, unsigned int a1, char *node)
{
    char *v4;  // eax
    char *iter;  // edx
    unsigned int v6;  // edi
    unsigned int v7;  // edi
    char v8;  // 4103
    int v10;  // edi
    unsigned int v0;  // [bp-0x14]
    int v1;  // [bp-0xc]
    int v2;  // [bp-0x8]
    char v3;  // [bp+0x0]

    v4 = a0;
    if (v4 && a1 > NULL)
    {
        if (node)
        {
            iter = v4;
            do
            {
                v6 = a1;
                a1 = v6;
            } while (*(iter) && (iter += 1, a1 = v6 - 1, v6 != 1));
            if (a1)
            {
                do
                {
                    v7 = a1;
                    v8 = *(node);
                    *(iter) = *(node);
                    iter += 1;
                    node += 1;
                    a1 = v7;
                } while (v8 && (a1 = v7 - 1, v7 != 1));
                if (a1)
                    return 0;
                *(v4) = 0;
                v0 = 0x22;
                *((unsigned int *)sub_46e96f()) = 0x22;
                sub_470f2d(0, 0, 0, 0, 0);
                return 0x22;
            }
        }
        *(v4) = 0;
    }
    v0 = 22;
    *((unsigned int *)sub_46e96f(v10, v1, v2, &v3)) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    return 22;
}
