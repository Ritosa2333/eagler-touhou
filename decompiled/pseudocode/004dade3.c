// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dade3; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dade3_0 {
    char padding_0[34];
    unsigned short field_22;
} st_4dade3_0;


int sub_4dade3(void)
{
    unsigned int v8;  // eax
    unsigned int v9;  // ecx
    unsigned int v10;  // edx
    unsigned int v11;  // ebx
    unsigned int v12;  // ebp
    unsigned int v13;  // esi
    int i;  // edi
    unsigned short v15;  // cs
    unsigned int v16;  // 4105
    int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x10]
    unsigned int v4;  // [bp-0xc]
    unsigned int v5;  // [bp-0x8]
    st_4dade3_0 **v6;  // [bp-0x4]

    v6 = v8 & 304818675;
    v5 = v9;
    v4 = v10;
    v3 = v11;
    v2 = v12;
    v1 = v13;
    v0 = i;
    InterlockedExchange(v6, v9);
    *(v6)->field_22 = v15;
    do
    {
        v16 = _ccall(10, 6, i, *((int *)0xa225a00), 0);
        if (!(v16 & 1))
            goto LABEL_0x4dadde;
        if (i > *((int *)0xa225a00))
            goto LABEL_0x4dad8c;
    } while (i > *((int *)0xa225a00));
}
