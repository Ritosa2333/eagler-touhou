// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48a45a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_48a45a(int *a0, char *a1, int a2, int a3, int a4, int a5)
{
    int v5;  // edi
    unsigned int v6;  // eax
    unsigned int v7;  // eax
    unsigned int v8;  // eax
    unsigned int v0;  // [bp-0x40]
    int v1;  // [bp-0x38]
    int v2;  // [bp-0x34]
    char v3;  // [bp-0x30], Other Possible Types: unsigned int
    char v4;  // [bp-0x20]

    v0 = 22;
    sub_48e03c(*(a0), a0[1], &v3, &v4, 22, v5, v1, v2);
    if (a1 && a2 > NULL)
    {
        if (sub_48dec0(&a1[(v3 == 45) + (a3 > 0)], (a2 == -0x1 ? 0xffffffff : a2 - (v3 == 45) - (a3 > 0)), a3 + 1, &v3))
        {
            *(a1) = 0;
            return v7;
        }
        sub_48a2eb(a2, a3, a4, &v3, 0, a5);
        return v8;
    }
    *((unsigned int *)sub_46e96f()) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    return v6;
}
