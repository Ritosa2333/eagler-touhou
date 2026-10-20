// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48aaac; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_48aaac(int *a0, char *a1, int a2, int a3, int a4, int a5)
{
    int v7;  // edi
    unsigned int v8;  // eax
    unsigned int v9;  // eax
    void* v10;  // edi
    unsigned int v11;  // eax
    int v12;  // eax
    void* v13;  // edi
    void* v14;  // edi
    unsigned int v15;  // eax
    unsigned int v16;  // eax
    unsigned int v0;  // [bp-0x44]
    int v1;  // [bp-0x3c]
    int v2;  // [bp-0x38]
    char v3;  // [bp-0x34], Other Possible Types: unsigned int
    int v4;  // [bp-0x30]
    int v5;  // [bp-0x24]
    char v6;  // [bp-0x20]

    v0 = 22;
    sub_48e03c(*(a0), a0[1], &v3, &v6, 22, v7, v1, v2);
    if (a1 && a2 > NULL)
    {
        v5 = v4 - 1;
        v9 = v3 == 45;
        v10 = &a1[v9];
        if (sub_48dec0(v10, (a2 == -0x1 ? 0xffffffff : a2 - v9), a3, &v3))
        {
            *(a1) = 0;
            return v11;
        }
        v12 = v4 - 1;
        if (v12 >= -0x4 && v12 < a3)
        {
            if (v5 < v12)
            {
                do
                {
                    v13 = v10;
                    v14 = v13 + 1;
                    v10 = v14;
                } while (*((char *)v13));
                *((char *)v14 - 2) = 0;
            }
            sub_48a8dd(a2, a3, 1, a5);
            return v15;
        }
        sub_48a2eb(a2, a3, a4, &v3, 1, a5);
        return v16;
    }
    *((unsigned int *)sub_46e96f()) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    return v8;
}
