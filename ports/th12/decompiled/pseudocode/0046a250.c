// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46a250; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46a250_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    unsigned int field_14;
    char padding_18[4];
    unsigned int field_1c;
    unsigned int field_20;
    unsigned int field_24;
    unsigned int field_28;
    unsigned int field_2c;
} st_46a250_0;

typedef struct st_46a250_1 {
    char padding_0[1072];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[60];
    struct st_46a250_0 *field_478;
    unsigned int field_47c;
} st_46a250_1;

extern char g_4b2ed0;

unsigned int sub_46a250(st_46a250_1 *index, unsigned int *idx)
{
    int v1;  // edi
    int v2;  // esi
    int v3;  // ebx
    st_46a250_0 *idx1;  // esi
    unsigned int v5;  // eax
    unsigned int v6;  // edx

    idx1 = sub_46d04a(48, v1, v2, v3);
    index->field_478 = idx1;
    sub_477420(idx1, 0, 48);
    /* unsupported instruction */
    /* unsupported instruction */
    idx1->field_24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx1->field_20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    idx1->field_28 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx1->field_2c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx1->field_0 = *(idx);
    idx1->field_4 = idx[1];
    idx1->field_8 = idx[2];
    v5 = idx1->field_1c;
    if (!((char)idx1->field_1c & 1))
    {
        if (/* unsupported instruction */)
            idx1->field_14 = /* unsupported instruction */;
        else
            idx1->field_14 = nan;
        idx1->field_10 = 0;
        idx1->field_c = 0xfff0bdc1;
        *((char **)&idx1->padding_18[0]) = &g_4b2ed0;
        idx1->field_1c = v5 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx1->field_14 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx1->field_14 = nan;
        /* unsupported instruction */
    }
    idx1->field_10 = 0;
    idx1->field_c = 0xffffffff;
    index->field_430 = idx1->field_0;
    index->field_434 = idx1->field_4;
    v6 = idx1->field_8;
    index->field_47c = index->field_47c & 0xfffffffd;
    index->field_438 = v6;
    return 0;
}
