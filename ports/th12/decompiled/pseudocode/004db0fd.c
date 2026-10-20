// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db0fd; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_b7707d06;

int sub_4db0fd(void)
{
    unsigned int v1;  // cc_op
    unsigned int v2;  // cc_dep1
    unsigned int v11;  // esi
    char v12;  // 4126
    unsigned int v3;  // cc_dep2
    unsigned int v4;  // cc_ndep
    unsigned int v5;  // 4106
    char *v6;  // ecx
    char v7;  // 4097
    unsigned int v8;  // ecx
    void* v9;  // ebx
    unsigned int v10;  // edi

    v5 = _ccall(5, v1, v2, v3, v4);
    if (v6 != 0x1 & v5 & 1)
        __debugbreak();
    g_b7707d06 = 0x33fd06e1;
    2408568029();
    v7 = *(v6);
    v8 = _INSERT(v6, 1, *((char *)((void*)&v6 + 1)) & v7);
    *((char *)v9 - 127) = ~(*((char *)v9 - 127));
    *((char *)(v10 + v11 + 101)) = ~(*((char *)(v10 + v11 + 101)));
    /* unsupported instruction */
    /* unsupported instruction */
    v12 = *((char *)*((int *)0xb7707d02));
    *((char *)*((int *)0xb7707d02)) = *((char *)*((int *)0xb7707d02)) & *((char *)((void*)&v6 + 1)) & v7;
    if (v8 != 1 & !(v12 & *((char *)((void*)&v6 + 1)) & v7))
        goto LABEL_0x4db10f;
    goto *((void *)(*((int *)0x4bd3)));
}
