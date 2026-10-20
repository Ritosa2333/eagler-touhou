// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x476077; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_476077_2 {
    unsigned short field_0;
    unsigned int field_2;
    unsigned int field_6;
    unsigned short field_a;
} st_476077_2;

typedef struct st_476077_0 {
    char field_0;
} st_476077_0;


int sub_476077(st_476077_2 *idx, st_476077_0 **a1, char *a2, unsigned int a3, unsigned int a4, unsigned int a5, unsigned int a6, unsigned int a7)
{
    unsigned int v25;  // edi
    char *v26;  // edi
    unsigned int v27;  // eax
    char *iter2;  // edx
    char v29;  // al
    unsigned int v1;  // [bp-0x8c]
    unsigned int v3;  // [bp-0x78]
    st_476077_0 **v4;  // [bp-0x74]
    unsigned int v5;  // [bp-0x70]
    int v6;  // [bp-0x6c]
    int v7;  // [bp-0x68]
    unsigned int v8;  // [bp-0x64]
    unsigned int v9;  // [bp-0x60]
    unsigned int v10;  // [bp-0x5c]
    char *v11;  // [bp-0x58]
    unsigned int v12;  // [bp-0x54]
    int v13;  // [bp-0x50]
    char v21;  // [bp-0x30]

    v1 = v25;
    v4 = a1;
    v26 = &v21 + 12;
    v3 = 0;
    v6 = 1;
    v13 = 0;
    v10 = 0;
    v9 = 0;
    v8 = 0;
    v7 = 0;
    v12 = 0;
    v5 = 0;
    if (!a7)
    {
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        return v27;
    }
    iter2 = a2;
    v11 = iter2;
    while (1)
    {
        v29 = *(iter2);
        if (*(iter2) != 32 && v29 != 9 && v29 != 10 && v29 != 13)
            break;
        iter2 += 1;
    }
    iter2 += 1;
}
