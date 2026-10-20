// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48eb7b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_48eb7b(void* idx)
{
    unsigned int v3;  // ebx
    int v4;  // edi
    unsigned int v5;  // edi
    int v0;  // [bp-0xc]
    int v1;  // [bp-0x8]
    char v2;  // [bp+0x0]

    v3 = 0xffffffff;
    if (!idx)
    {
        *((unsigned int *)sub_46e96f(v4, v0, v1, &v2)) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        return 0xffffffff;
    }
    if ((char)idx[12] & 131)
    {
        v3 = sub_48d12a(idx);
        sub_49112f(idx);
        if (sub_491062(sub_47c1da(idx)) < NULL)
        {
            v3 = 0xffffffff;
        }
        else if ((int)idx[28])
        {
            sub_46c941((int)idx[28], v5);
            *((unsigned int *)&idx[28]) = 0;
        }
    }
    *((unsigned int *)&idx[12]) = 0;
    return v3;
}
