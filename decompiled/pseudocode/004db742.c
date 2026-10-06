// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db742; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


void sub_4db742(unsigned short a0, unsigned int a1)
{
    void* v1;  // esi
    void* v2;  // esi
    unsigned int v3;  // eax
    unsigned int v4;  // edx
    char *v5;  // ebx
    unsigned int v6;  // 4106
    unsigned int v7;  // 4115
    unsigned int v8;  // 4101

    v2 = v1 + 1;
    __unsupported_jumpkind_Ijk_NoDecode()
    v3 = _INSERT(0, 0, *((char *)v2));
    v4 = _INSERT(a1, 1, 129);
    __unsupported_jumpkind_Ijk_Privileged()
    v6 = _ccall(4, *(v5), (char)a0, 0);
    v7 = _ccall(10, 9, v3, 2290344162 ^ v6 & 1, v6 & 1);
    if (v7 & 1)
        goto LABEL_0x4db755;
    v8 = _ccall(12, 9, v3, 2290344162 ^ v6 & 1, v6 & 1);
    if (!(v8 & 1))
    {
        *((char *)v2 - 91) = *((char *)v2 - 91) ^ *((char *)((void*)&a0 - *((int *)(v4 + 405213341)) + 1));
        1565209767();
        __unsupported_jumpkind_Ijk_NoDecode()
    }
}
