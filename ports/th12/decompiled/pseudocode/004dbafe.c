// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dbafe; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dbafe_0 {
    char padding_0[68];
    unsigned int field_44;
} st_4dbafe_0;

extern char g_2c5b792c;
extern unsigned int g_62292c80;

int sub_4dbafe(void)
{
    unsigned int v3;  // eax
    unsigned int v4;  // ebx
    unsigned int v13;  // edx
    unsigned int *v14;  // edx
    unsigned int v15;  // 4196
    unsigned int v16;  // 4198
    unsigned short v17;  // ax
    unsigned int v18;  // 4200
    unsigned int v19;  // eax
    unsigned int v20;  // 4204
    char *v5;  // eax
    unsigned int v6;  // ecx
    void* v7;  // esi
    st_4dbafe_0 *idx;  // ecx
    unsigned short v9;  // ds
    void* v10;  // eax
    unsigned int v11;  // edx
    void* v12;  // ebp
    char v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4], Other Possible Types: unsigned short

    v5 = v3 ^ v4;
    idx = v6 ^ *((int *)((char *)v7 - 2089787186));
    if (!(v6 ^ *((int *)((char *)v7 - 2089787186))))
    {
        v1 = 3422261851;
        v14 = _INSERT(v13, 0, (char)v13 + *(v5));
        v15 = _ccall(15, *((int *)0xa6e11118) | 1413639272, 0, 0);
        v16 = _ccall(CONCAT((unsigned short)v15, (unsigned short)(*((int *)0xa6e11118) | 1413639272)), 55);
        v17 = v16;
        *(v14) = *(v14) & v7;
        v18 = _ccall(9, &v1, idx->field_44 ^ 0, 0);
        v19 = _INSERT(CONCAT(v17, 0), 0, (char)v17 - 63 + ((char)v18 & 1));
        *((unsigned int *)&(&v0)[idx->field_44]) = v19;
        v20 = _ccall(7, (char)v17, 193 ^ (char)v18 & 1, v18 & 1);
        InterlockedExchange8(v19 - 0xff4227f + (v20 & 1) + 101, v19 - 0xff4227f + (v20 & 1));
        syscall();
    }
    v1 = v9;
    v10 = _INSERT(v5 - 1, 0, *((char *)&v5 - 1) - *((char *)((void*)&v5 - 1 + 1)));
    *((unsigned int *)((char *)v10 + 0x2 * idx)) = v11;
    *((char *)v10) = (char)(*((char *)v10)) >> (*((char *)&idx) & 31 & 7) | *((char *)v10) << 8 - (*((char *)&idx) & 31 & 7);
    *((char *)v7 - 55) = *((char *)v7 - 55) - 91;
    g_62292c80 = g_62292c80 << (*((char *)&idx) & 31) | g_62292c80 >> 32 - (*((char *)&idx) & 31);
    g_2c5b792c = v10 & 1921592329;
    *((void* *)((char *)v12 - 123)) = v7;
}
