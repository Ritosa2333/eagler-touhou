// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x492212; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_492212_1 {
    char padding_0[144];
    unsigned int field_90;
} st_492212_1;

typedef struct st_492212_0 {
    unsigned int field_0;
    char padding_4[12];
    unsigned int field_10;
    unsigned int field_14;
    unsigned int field_18;
    unsigned int field_1c;
} st_492212_0;


unsigned int sub_492212(st_492212_0 **a0, unsigned int *idx)
{
    unsigned int v2;  // esi
    unsigned int v3;  // edi
    st_492212_0 *v4;  // esi
    st_492212_1 *index;  // eax
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

    v1 = v2;
    v0 = v3;
    if (a0 && !(v4 = *(a0), !v4))
    {
        if (v4->field_0 == 3765269347 && v4->field_10 == 3 && (v4->field_14 == 429065504 || v4->field_14 == 429065505 || v4->field_14 == 429065506) && !v4->field_1c)
            v4 = *((int *)(sub_475467() + 0x88));
        sub_491d54(idx, v4->field_18);
        idx[2] = *((int *)(sub_475467() + 0x88));
        idx[3] = *((int *)(sub_475467() + 140));
        *((st_492212_0 **)(sub_475467() + 0x88)) = v4;
    }
    else
    {
        idx[2] = 0xffffffff;
        idx[3] = 0xffffffff;
    }
    index = sub_475467();
    index->field_90 = index->field_90 - 1;
    if (*((int *)(sub_475467() + 144)) < 0)
        *((unsigned int *)(sub_475467() + 144)) = 0;
    return 1;
}
