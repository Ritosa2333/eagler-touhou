// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48b25d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4aafb8;

void sub_48b25d(unsigned int a0)
{
    void* idx;  // ebp
    char **v6;  // ebx
    int v7;  // eax
    unsigned int v9;  // edi
    char *v10;  // eax
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp+0xc]
    unsigned int v2;  // [bp+0x10]
    unsigned int v3;  // [bp+0x14]
    unsigned int v4;  // [bp+0x18]

    sub_46fcec(&g_4aafb8, 16);
    sub_46ec9a(7);
    *((unsigned int *)((char *)idx - 4)) = 0;
    v6 = (int)idx[8];
    if (v6)
    {
        *(v6) = NULL;
        if ((int)idx[12])
            *((unsigned int *)(int)idx[12]) = 0;
        if ((int)idx[16])
        {
            v7 = sub_48af42((int)idx[16]);
            *((int *)((char *)idx - 32)) = v7;
            if (v7)
            {
                v9 = sub_475860(v7) + 1;
                v10 = sub_48e3da(v9, 1);
                *(v6) = v10;
                if (!v10)
                {
                    *((unsigned int *)sub_46e96f()) = 12;
                    *((int *)((char *)idx - 28)) = *((int *)sub_46e96f());
                    *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
                    sub_48b33a();
                    sub_46fd31(v1, v2, v3, v4);
                    return;
                }
                if (sub_47a2b9(v10, v9, *((int *)((char *)idx - 32))))
                    sub_470dc6(0, 0, 0, 0, 0);
                if ((int)idx[12])
                    *((unsigned int *)(int)idx[12]) = v9;
            }
            *((unsigned int *)((char *)idx - 28)) = 0;
            *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
            sub_48b33a();
            sub_46fd31(v1, v2, v3, v4);
            return;
        }
    }
    v0 = 22;
    *((unsigned int *)sub_46e96f()) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    *((unsigned int *)((char *)idx - 28)) = 22;
    *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
    sub_48b33a();
    sub_46fd31(v1, v2, v3, v4);
    return;
}
