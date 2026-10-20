// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4105e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4105e0_0 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[60];
    void* field_478;
} st_4105e0_0;

extern char g_4b2ed0;

unsigned int sub_4105e0(st_4105e0_0 *index, unsigned int *idx)
{
    void* idx1;  // edi
    unsigned int v7;  // eax
    int v8;  // ecx
    void* iter;  // eax
    int v0;  // [bp-0x18]
    int v1;  // [bp-0x14]
    int v2;  // [bp-0x10]
    int v3;  // [bp-0xc]
    int node;  // [bp-0x8], Other Possible Types: unsigned int

    idx1 = sub_46d04a(216, v0, v1, v2, v3);
    index->field_478 = idx1;
    sub_477420(idx1, 0, 216);
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
    *((int *)idx1) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&idx1[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    node = sub_464440();
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
    if (node < 0)
    {
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
        node = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        node = nan;
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
        *((int *)&idx1[192]) = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        *((unsigned int *)&idx1[192]) = nan;
        /* unsupported instruction */
    }
    v7 = (int)idx1[212];
    if (!((char)(int)idx1[212] & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&idx1[0xcc]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((unsigned int *)&idx1[200]) = 0;
        *((unsigned int *)&idx1[196]) = 0xfff0bdc1;
        *((char **)&idx1[208]) = &g_4b2ed0;
        *((unsigned int *)&idx1[212]) = v7 | 1;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    *((unsigned int *)&idx1[200]) = 1;
    *((int *)&idx1[0xcc]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((unsigned int *)&idx1[196]) = 0;
    node = 0;
    v8 = 0;
    iter = idx1 + 128;
    do
    {
        *((unsigned int *)iter) = 0xff40ff00;
        if (v8 < 8)
        {
            *((char *)iter) = 0xff - (char)v8 * 32;
        }
        else
        {
            node += 1;
            *((char *)&iter[3]) = 0xff - (char)node * 16;
        }
    } while ((v8 = (int)(v8 + 1), iter += 4, v8 < 16));
    index->field_430 = *(idx);
    index->field_434 = idx[1];
    index->field_438 = idx[2];
    index->field_20 = 13;
    return 0;
}
