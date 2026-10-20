// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db26d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4db26d(void)
{
    unsigned int v2;  // cc_op
    unsigned int v3;  // cc_dep1
    unsigned int v12;  // 4133
    unsigned long v13;  // ldt
    unsigned long v14;  // gdt
    unsigned long long v15;  // 4105
    unsigned int v16;  // 4101
    unsigned int v17;  // 4101
    unsigned int v18;  // 4112
    unsigned int v19;  // eax
    char *v20;  // eax
    char v21;  // 4103
    unsigned int v4;  // cc_dep2
    unsigned long long *v22;  // edi
    unsigned int v23;  // ecx
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4101
    unsigned int v7;  // 4113
    char *v8;  // ecx
    unsigned int v9;  // ecx
    void* v10;  // eax
    unsigned int v11;  // 4106
    char v0;  // [bp+0x0]

    v6 = _ccall(14, v2, v3, v4, v5);
    if (v6 & 1)
    {
        v7 = _ccall(v2, v3, v4, v5);
        v9 = _INSERT(v8, 1, *((char *)((void*)&v8 + 1)) + *(v8) + ((char)v7 & 1));
        do
        {
            v11 = *((int *)((char *)v10 - 25));
            *((unsigned int *)((char *)v10 - 25)) = *((int *)((char *)v10 - 25)) + v9;
            v12 = _ccall(10, 9, v11, v9 ^ 0, 0);
        } while (!(v12 & 1));
        v15 = _ccall(v13, v14, 0x94ff, 2340386338);
        goto *((void *)((unsigned int)v15));
    }
    else
    {
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v16 = _ccall(14, v2, v3, v4, v5);
            if (v16 & 1)
                goto LABEL_4db2a0;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v17 = _ccall(14, v2, v3, v4, v5);
            if (v17 & 1)
            {
LABEL_4db2a0:
                v18 = _ccall(v2, v3, v4, v5);
                v20 = v19 + 1211906514 + (v18 & 1);
                v21 = *(v20);
                *(v20) = v21 & 25;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                if ((v21 & 25) >= 0)
                {
                    *(v22) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                }
                else if (!((v23 + esp >= 0x80000000 ? 1 : 0) & 1))
                {
                    if (!&(&v0)[v23])
                    {
                        __unsupported_jumpkind_Ijk_NoDecode()
                        return;
                    }
                    return;
                }
            }
        }
    }
}
