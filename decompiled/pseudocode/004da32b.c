// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da32b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_8de21b68;

int sub_4da32b(unsigned int a0)
{
    void* v1;  // edi
    unsigned int v2;  // ebx
    char *v3;  // ebx
    unsigned int v4;  // ebp
    unsigned int v6;  // cc_op
    unsigned int v7;  // cc_dep1
    unsigned int v8;  // cc_dep2
    unsigned int v9;  // cc_ndep
    unsigned int v10;  // 4113

    InterlockedExchange(v1 - 77, v2);
    v3 = *((int *)((char *)v1 - 77));
    g_8de21b68 = syscall(v4);
    v10 = _ccall(v6, v7, v8, v9);
    *(v3) = *(v3) - *((char *)&v3) - ((char)v10 & 1);
    return g_8de21b68;
}
