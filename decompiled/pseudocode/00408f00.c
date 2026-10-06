// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x408f00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_408f00_0 {
    char padding_0[24];
    int field_18;
    char padding_1c[36];
    int field_40;
    char padding_44[4];
    int field_48;
    char padding_4c[1224];
    unsigned int field_514;
} st_408f00_0;

typedef struct st_408f00_1 {
    char padding_0[1148];
    unsigned int field_47c;
} st_408f00_1;

unsigned int sub_408f00(unsigned int a0)
{
    st_408f00_0 *idx;  // edi
    int v14;  // ebx
    unsigned int v15;  // ebx
    int v16;  // esi
    unsigned int *idx1;  // esi
    int v18;  // eax
    unsigned int v20;  // ecx
    st_408f00_1 *idx2;  // eax
    st_408f00_1 *index;  // eax
    char *v0;  // [bp-0x68]
    unsigned int v1;  // [bp-0x64]
    char *v2;  // [bp-0x60]
    int v3;  // [bp-0x5c], Other Possible Types: unsigned int
    char *v4;  // [bp-0x58], Other Possible Types: unsigned int
    st_408f00_0 *v5;  // [bp-0x54]
    unsigned int v6;  // [bp-0x50]
    int v7;  // [bp-0x4c], Other Possible Types: unsigned int
    int v8;  // [bp-0x48], Other Possible Types: unsigned int
    int v9;  // [bp-0x44], Other Possible Types: unsigned int
    unsigned int v10;  // [bp-0x40]
    unsigned int v11;  // [bp-0x3c]

    v15 = sub_461920(idx->field_40, v14);
    if (!v15)
        idx->field_40 = v15;
    sub_406f60(40);
    if (!v15)
    {
        sub_461970(idx->field_48);
        idx->field_40 = 0;
        return 0xffffffff;
    }
    if (idx->field_18 == 180)
    {
        sub_453d90(v16);
        idx1 = sub_46c9ea(0x44);
        if (idx1)
        {
            idx1[16] = idx1[16] & 0xfffffffe;
            sub_477420(idx1, 0, 0x44);
            *(idx1) = *(idx1) | 2;
            sub_452a60(idx1, 8, 4, 1, 60, 10, 70);
        }
    }
    if (!idx->field_18)
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
        v9 = 13;
        v8 = 180;
        if (/* unsupported instruction */)
        {
            v7 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v7 = nan;
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
            v6 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v6 = nan;
            /* unsupported instruction */
        }
        v5 = &idx->padding_4c[1212];
    }
    else
    {
        if (idx->field_18 != 180)
            goto LABEL_408fed;
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
        v9 = 80;
        v8 = 100;
        if (/* unsupported instruction */)
        {
            v7 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v7 = nan;
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
            v6 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v6 = nan;
            /* unsupported instruction */
        }
        v5 = &idx->padding_4c[1212];
    }
    sub_4390f0(v5, v6, v7, v8, v9);
LABEL_408fed:
    v18 = idx->field_18;
    if (v18 >= 250)
    {
LABEL_4090eb:
        v8 = sub_464440();
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
        if (sub_464440() < 0)
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
            v8 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v8 = nan;
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
            v9 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v9 = nan;
            /* unsupported instruction */
        }
        v8 = sub_464440();
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
        if (sub_464440() < 0)
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
        v4 = &v9;
        v3 = 5;
        if (/* unsupported instruction */)
        {
            v8 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v8 = nan;
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
        v2 = &v8;
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
            v10 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v10 = nan;
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_40fbe0();
        index = sub_461920(v5);
        index->field_47c = index->field_47c & 0xffffff3f | 32;
    }
    else if (v18 >= 30)
    {
        v8 = sub_464440();
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
        if (v8 < 0)
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
        v4 = v20;
        if (/* unsupported instruction */)
        {
            v8 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v8 = nan;
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
            v7 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v7 = nan;
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
            v8 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v8 = nan;
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
            v4 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v4 = nan;
            /* unsupported instruction */
        }
        sub_409310();
        v3 = v20;
        if (/* unsupported instruction */)
        {
            v3 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v3 = nan;
            /* unsupported instruction */
        }
        sub_4092f0();
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
        v2 = &v7;
        v1 = 5;
        if (/* unsupported instruction */)
        {
            v7 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v7 = nan;
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
        v0 = &v6;
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
            v8 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v8 = nan;
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
            v9 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v9 = nan;
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
            v7 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v7 = nan;
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
            v8 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v8 = nan;
            /* unsupported instruction */
        }
        sub_40fbe0();
        idx2 = sub_461920(v3);
        idx2->field_47c = idx2->field_47c & 0xffffff3f | 32;
    }
    else if (!(v18 < 250))
    {
        goto LABEL_4090eb;
    }
    if (idx->field_18 >= 400)
    {
        sub_461970(idx->field_40);
        sub_461970(idx->field_48);
        idx->field_40 = 0;
        return 0;
    }
    sub_409220();
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
    if (!/* unsupported instruction */)
    {
        idx->field_514 = nan;
        /* unsupported instruction */
        return 0;
    }
    idx->field_514 = /* unsupported instruction */;
    /* unsupported instruction */
    return 0;
}
