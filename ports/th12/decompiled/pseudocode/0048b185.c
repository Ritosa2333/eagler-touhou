// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48b185; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4aaf98;

int sub_48b185(void)
{
    void* idx;  // ebp
    unsigned int *v3;  // edi
    char *v4;  // ecx
    unsigned int v5;  // eax
    int v6;  // esi
    unsigned int v7;  // eax
    unsigned int v0;  // [bp-0xc]

    sub_46fcec(&g_4aaf98, 12);
    sub_46ec9a(7);
    *((unsigned int *)((char *)idx - 4)) = 0;
    v3 = (int)idx[8];
    if (v3)
    {
        *(v3) = 0;
        v4 = (int)idx[12];
        if (v4)
        {
            if ((int)idx[16] > 0)
                goto LABEL_48b1df;
            if (v4)
                goto LABEL_48b1e4;
        }
        if (!(int)idx[16])
        {
LABEL_48b1df:
            v5 = 1;
        }
        else
        {
LABEL_48b1e4:
            v5 = 0;
        }
        if (v5)
        {
            if (v4)
                *(v4) = 0;
            v6 = sub_48af42((int)idx[20]);
            if (v6)
            {
                v7 = sub_475860(v6) + 1;
                *(v3) = v7;
                if ((int)idx[16])
                {
                    if (v7 > (int)idx[16])
                    {
                        *((unsigned int *)((char *)idx - 28)) = 0x22;
                        *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
                        sub_48b254();
                        return sub_46fd31();
                    }
                    else if (sub_47a2b9((int)idx[12], (int)idx[16], v6))
                    {
                        sub_470dc6(0, 0, 0, 0, 0);
                    }
                }
            }
            *((unsigned int *)((char *)idx - 28)) = 0;
            *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
            sub_48b254();
            return sub_46fd31();
        }
    }
    v0 = 22;
    *((unsigned int *)sub_46e96f()) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    *((unsigned int *)((char *)idx - 28)) = 22;
    *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
    sub_48b254();
    return sub_46fd31();
}
