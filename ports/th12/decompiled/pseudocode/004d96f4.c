// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d96f4; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4d96f4(void)
{
    unsigned int *v4;  // ebx
    void* v5;  // edi
    unsigned int v14;  // 4100
    unsigned int v15;  // 4118
    void* v6;  // ecx
    unsigned int v7;  // 4123
    unsigned int v8;  // id
    unsigned int v9;  // ac
    unsigned int idx;  // edx
    char v11;  // 4103
    unsigned int v12;  // 4159
    unsigned int v13;  // 4160
    unsigned int v0;  // [bp-0xc]
    void* v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]

    *(v4) = *(v4) | v5;
    v7 = _ccall(13, *((char *)((void*)&v4 + 1)) & *((char *)v6 - 64), 0, 0);
    v2 = v7 | 0x202 | v8 * 0x200000 & 0x200000 | v9 * 0x40000 & 0x40000;
    v11 = *((char *)v5 + 4 * idx);
    *((char *)v5 + 4 * idx) = v11 * 0;
    v12 = _ccall(22, v11 * 0x200, v11 * 0x100, 0);
    v13 = _ccall(CONCAT((unsigned short)v12, *((unsigned short *)&v4)), 47);
    if (!(((char)v13 + 139 >= 0x80000000 ? 1 : 0) & 1))
    {
        v1 = v5;
        v14 = *((int *)v5);
        *((unsigned int *)v5) = *((int *)v5) | idx;
        v15 = _ccall(15, v14 | idx, 0, 0);
        v0 = v15 | 0x202 | v8 * 0x200000 & 0x200000 | v9 * 0x40000 & 0x40000;
    }
    644671013();
}
