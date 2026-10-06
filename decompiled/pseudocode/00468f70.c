// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x468f70; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_468f70_1 {
    char padding_0[4104];
    unsigned int field_1008;
    unsigned int field_100c;
    char padding_1010[4];
    struct st_468f70_2 *field_1014;
} st_468f70_1;

typedef struct st_468f70_2 {
    struct st_468f70_0 *field_0;
} st_468f70_2;

typedef struct st_468f70_0 {
    char padding_0[4];
    unsigned int field_4;
} st_468f70_0;

int sub_468f70(st_468f70_1 *idx, unsigned int a1, int a2)
{
    unsigned int v0;  // edx
    unsigned int v1;  // 4108
    int v2;  // eax
    unsigned int v3;  // edx

    if (a2 >= 0)
        return *((int *)&idx->padding_0[8 + a2 + idx->field_100c]);
    if (a2 == -0x1)
    {
        v0 = idx->field_1008 - 4;
        v1 = _ccall(8, 3, idx->field_1008, 0xfffffffc, 0);
        if (v1 & 1)
            return a2;
        idx->field_1008 = v0;
        v2 = *((int *)&idx->padding_0[8 + v0]);
        v3 = v0 - 4;
        idx->field_1008 = v3;
        a2 = v2;
        if (idx->padding_0[8 + v3] != 0x66)
            return a2;
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
        return sub_4931e0();
    }
    goto *((void *)(idx->field_1014->field_0->field_4));
}
