// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9ee0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_6d497985;

int sub_4d9ee0(void)
{
    unsigned int v2;  // cc_op
    unsigned int v3;  // cc_dep1
    unsigned int v4;  // cc_dep2
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4105
    unsigned int idx;  // ebx
    unsigned int v8;  // edx
    unsigned int v9;  // 4108
    unsigned int v10;  // esi
    unsigned int v0;  // [bp-0x4]

    v6 = _ccall(v2, v3, v4, v5);
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
    *((unsigned int *)(5 * idx + (char *)&g_6d497985)) = v8 - 1;
    v9 = _ccall(6, 21, v8 - 1, 0, v6);
    if (v9 & 1)
        goto LABEL_0x4d9ef0;
    v0 = v10;
    __debugbreak();
}
