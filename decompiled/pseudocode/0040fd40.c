// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40fd40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40fd40_1 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    unsigned int field_14;
    unsigned int field_18;
    unsigned int field_1c;
    unsigned int field_20;
    unsigned int field_24;
    unsigned int field_28;
    unsigned int field_2c;
    unsigned int field_30;
} st_40fd40_1;

typedef struct st_40fd40_0 {
    char padding_0[1072];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[60];
    struct st_40fd40_1 *field_478;
} st_40fd40_0;

unsigned int sub_40fd40(st_40fd40_0 *idx, unsigned int *index)
{
    int v1;  // edi
    int v2;  // esi
    unsigned int *idx1;  // eax
    unsigned int v4;  // edx

    idx1 = sub_46c9ea(52, v1, v2);
    idx->field_478 = idx1;
    *(idx1) = *(index);
    idx1[1] = index[1];
    idx1[2] = index[2];
    idx1[3] = index[3];
    idx1[4] = index[4];
    idx1[5] = index[5];
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
    if (/* unsupported instruction */)
    {
        idx1[6] = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx1[6] = nan;
        /* unsupported instruction */
    }
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
    if (/* unsupported instruction */)
    {
        idx1[7] = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx1[7] = nan;
        /* unsupported instruction */
    }
    v4 = *(idx1);
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
    if (/* unsupported instruction */)
    {
        idx1[8] = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx1[8] = nan;
        /* unsupported instruction */
    }
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
    if (/* unsupported instruction */)
    {
        idx1[9] = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx1[9] = nan;
        /* unsupported instruction */
    }
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
    if (/* unsupported instruction */)
    {
        idx1[10] = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx1[10] = nan;
        /* unsupported instruction */
    }
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
    if (/* unsupported instruction */)
    {
        idx1[11] = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx1[11] = nan;
        /* unsupported instruction */
    }
    idx1[12] = index[12];
    idx->field_430 = v4;
    idx->field_434 = idx1[1];
    idx->field_438 = idx1[2];
    return 0;
}
