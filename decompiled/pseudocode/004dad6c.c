// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dad6c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dad6c_0 {
    char padding_0[48];
    unsigned int field_30;
} st_4dad6c_0;


int sub_4dad6c(void)
{
    unsigned int v2;  // eax
    unsigned int v3;  // eax
    unsigned int v12;  // 4230
    unsigned int v13;  // eax
    unsigned int v14;  // 4131
    char *v15;  // esi
    unsigned int v4;  // cc_dep1
    unsigned int v5;  // 4101
    unsigned int v6;  // 4101
    st_4dad6c_0 *v7;  // edx
    unsigned int *v8;  // eax
    unsigned int v9;  // 4105
    unsigned int *v10;  // ecx
    unsigned int v11;  // ebp
    char v0;  // [bp+0x0]

    v3 = v2 | &v0;
    if ((v2 | &v0) >= 0)
        __debugbreak();
    v4 = v3 ^ 3660320644;
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v5 = _ccall(0, 15, v4, 0, 0);
        if (v5 & 1)
            goto LABEL_0x4dad94;
        else
            goto LABEL_4dad7e;
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v6 = _ccall(0, 15, v4, 0, 0);
        if (v6 & 1)
            goto LABEL_0x4dad94;
        else
            goto LABEL_4dad7e;
    }
LABEL_4dad7e:
    esp = &v0 | v7->field_30;
    v8 = _INSERT(v3 ^ 3660320644, 0, x86g_dirtyhelper_IN(3<32>, 1<32>));
    v9 = *(v8);
    *(v8) = *(v8) - (char *)v10;
    esp = esp - 4;
    *((unsigned int *)(esp - 4)) = v11 + 1;
    /* unsupported instruction */
    v12 = _ccall(12, v9, v10 ^ 0, 0);
    v13 = _INSERT(v8, 0, (int)((v12 & 1) * 0x80000000) >> 31);
    *(v10) = *(v10) + 1645169457;
    v14 = *((int *)esp);
    *((unsigned int *)esp) = *((int *)esp) ^ 2496967972;
    *((unsigned int *)(esp - 4)) = v13;
    if (!(v14 ^ 2496967972))
        goto LABEL_0x4dad39;
    *((char *)*((int *)(esp - 4))) = *(v15);
}
