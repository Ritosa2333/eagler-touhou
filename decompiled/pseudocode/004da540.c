// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da540; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


void sub_4da540(void* idx, void* a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5, unsigned int a6)
{
    unsigned int v2;  // cc_op
    unsigned int v3;  // cc_dep1
    unsigned int v12;  // 4165
    unsigned int v13;  // eax
    unsigned int v4;  // cc_dep2
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4101
    int *v7;  // edi
    unsigned int v8;  // 4136
    int v9;  // eax
    unsigned int v10;  // ebx
    int v11;  // cc_dep2
    int *v0;  // [bp+0x0]
    unsigned int v1;  // [bp+0x1c]

    v6 = _ccall(2, v2, v3, v4, v5);
    if (!(v6 & 1))
        return;
    v7 = v0;
    v1 = 1413328991;
    v8 = _ccall(v2, v3, v4, v5);
    v9 = a6 + 1969553598 - (v8 & 1);
    v10 = _INSERT(a3, 0, 169);
    v11 = *(v7);
    v12 = _ccall(0, 6, v9, *(v7), 0);
    if (!(v12 & 1))
    {
        a5 = v10;
        *((unsigned int *)idx) = *((int *)idx) ^ 0xffffffd4;
        *((unsigned int *)((char *)a1 - 77)) = *((int *)((char *)a1 - 77)) + a4;
        v13 = _INSERT(v9, 0, *((char *)idx));
        x86g_dirtyhelper_OUT(45<32>, vvar_60{r8|4b}, 4<32>)
    }
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
        *((short *)&idx[1100549623]) = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        *((short *)&idx[1100549623]) = nan;
        /* unsupported instruction */
    }
    if (v9 >= v11)
        __debugbreak();
}
