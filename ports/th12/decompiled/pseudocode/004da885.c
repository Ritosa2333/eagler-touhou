// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da885; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4da885(unsigned int a0, unsigned int a1)
{
    unsigned int v2;  // ebx
    char v3;  // bl
    unsigned int v12;  // 4154
    void* v13;  // esi
    void* v14;  // esi
    unsigned int v15;  // ebp
    unsigned int v16;  // 4155
    unsigned int v17;  // ecx
    unsigned int v18;  // 4100
    void* v19;  // eax
    unsigned int v20;  // 4148
    char *v21;  // edi
    unsigned int v4;  // cc_dep1
    unsigned int v5;  // cc_dep2
    unsigned int v6;  // cc_ndep
    void* v7;  // eax
    unsigned int v8;  // 4147
    unsigned int v9;  // 4121
    unsigned int v10;  // 4101
    char v11;  // bl
    char v0;  // [bp+0x0]

    v3 = (char)v2 * 2;
    v4 = *((char *)&&v0);
    v5 = 229 ^ v2 * 2 < v2;
    v6 = v2 * 2 < v2;
    v7 = _INSERT(&v0, 0, *((char *)&&v0) - 27 + (v2 * 2 < v2));
    v8 = _ccall(8, 7, *((char *)&&v0), 229 ^ v2 * 2 < v2, v2 * 2 < v2);
    if (!(v8 & 1))
        goto LABEL_0x4da875;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v9 = _ccall(10, 7, v4, v5, v6);
    if (v9 & 1)
    {
        v10 = _ccall(10, 7, v4, v5, v6);
        if (v10 & 1)
            goto LABEL_0x4da81f;
    }
    else
    {
        esp = *((int *)0x7d1d553a) + 3;
        v11 = v3 + *((char *)(a0 - 975628803)) + (a1 < a0);
        v12 = _ccall(7, v3, *((char *)(a0 - 975628803)) ^ a1 < a0, a1 < a0);
        v14 = v13 + 3;
        v16 = _ccall(14, 18, v15 + 1, 0, v12);
        if (!(v16 & 1))
            goto LABEL_0x4da952;
        *((int *)(esp - 4)) = esp;
        if (a0 != 1)
            goto LABEL_0x4da912;
        v17 = *((int *)esp);
        v18 = *((int *)((char *)v14 - 40));
        *((unsigned int *)((char *)v14 - 40)) = *((int *)((char *)v14 - 40)) - v17;
        if (v18 - v17 < 0)
        {
            *((char *)v7 - 117) = *((char *)v7 - 117) & v11;
            v19 = x86g_dirtyhelper_IN(62<32>, 4<32>);
            *((unsigned int *)((char *)v19 - 89)) = *((int *)((char *)v19 - 89)) - 736822214;
            v20 = _ccall(12, 3, *((unsigned short *)&v19), 3472519409, 0);
            if (v20 & 1)
                goto LABEL_0x4da90f;
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
            return *((unsigned short *)&v19) + 28913;
        }
        *(v21) = *((char *)&v7);
        x86g_dirtyhelper_OUT(18<32>, Conv(8->32, Extract(vvar_22{r8|4b}, 8bits@0<64>)), 1<32>)
    }
}
