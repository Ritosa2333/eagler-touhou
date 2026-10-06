// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x488df0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

char * sub_488df0(char *a0, char *a1)
{
    char *v8;  // eax
    char *v9;  // edx
    char *v10;  // esi
    unsigned int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x24]
    unsigned int v2;  // [bp-0x20]
    unsigned int v3;  // [bp-0x1c]
    unsigned int v4;  // [bp-0x18]
    unsigned int v5;  // [bp-0x14]
    unsigned int v6;  // [bp-0x10]
    unsigned int v7;  // [bp-0xc]

    v8 = NULL;
    v7 = 0;
    v6 = 0;
    v5 = 0;
    v4 = 0;
    v3 = 0;
    v2 = 0;
    v1 = 0;
    v0 = 0;
    v9 = a1;
    while (1)
    {
        v8 = _INSERT(v8, 0, *(v9));
        if (!*(v9))
            break;
        v9 += 1;
        *((char *)&v0 + (v8 >> 3)) = *((char *)&v0 + (v8 >> 3)) | 1 << ((char)v8 & 7);
    }
    while (1)
    {
        v10 = a0;
        v8 = _INSERT(v8, 0, *(v10));
        if (!*(v10))
            return v8;
        a0 = v10 + 1;
        if ((char)(*((char *)&v0 + (v8 >> 3))) >> ((char)v8 & 7) & 1)
            return v10;
    }
}
