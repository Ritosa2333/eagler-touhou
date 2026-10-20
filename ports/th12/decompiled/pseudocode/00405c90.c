// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x405c90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_405c90_0 {
    unsigned int field_0;
    char padding_4[24];
    unsigned int field_1c;
    char padding_20[52];
    unsigned int field_54;
    char padding_58[24];
    unsigned int field_70;
    int field_74;
    unsigned int field_78;
    char padding_7c[4];
    unsigned int field_80;
    unsigned int field_84;
    unsigned int field_88;
} st_405c90_0;

extern char g_4b2ed0;

unsigned int * sub_405c90(unsigned int a0, unsigned int a1, unsigned int *a2)
{
    st_405c90_0 *index;  // ebx
    st_405c90_0 *idx;  // esi
    unsigned int v26;  // ecx
    unsigned int *iter;  // edi
    st_405c90_0 *iter1;  // esi
    unsigned int v29;  // ecx
    unsigned int *l;  // esi
    unsigned int *i1;  // esi
    st_405c90_0 *iter2;  // ebp
    unsigned int v33;  // ecx
    unsigned int *v34;  // edi
    st_405c90_0 *m;  // esi
    int v18;  // edi
    unsigned int v36;  // ecx
    unsigned int *n;  // esi
    unsigned int *i0;  // eax
    unsigned int v39;  // ecx
    unsigned int v40;  // ecx
    unsigned int v41;  // eax
    unsigned int v42;  // ecx
    unsigned int *v43;  // edi
    st_405c90_0 *v19;  // esi
    unsigned int v20;  // eax
    unsigned int v21;  // ecx
    unsigned int *node;  // edi
    st_405c90_0 *i;  // esi
    st_405c90_0 *j;  // esi
    unsigned int v25;  // eax
    int v0;  // [bp-0xf0], Other Possible Types: unsigned int
    unsigned int v1;  // [bp-0xec]
    unsigned int v2;  // [bp-0xe8]
    int v3;  // [bp-0xe4], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0xdc]
    unsigned int v5;  // [bp-0xd4]
    unsigned int v6;  // [bp-0xd0]
    unsigned long v7;  // [bp-0xcc], Other Possible Types: unsigned long long
    unsigned int v8;  // [bp-0xc4]
    unsigned int v9;  // [bp-0xc0]
    unsigned int v10;  // [bp-0xbc]
    unsigned int v11;  // [bp-0xb8]
    char v12;  // [bp-0xb0]
    unsigned int v13;  // [bp-0x94]
    unsigned int v14;  // [bp-0x90]
    unsigned int *v15;  // [bp-0xc]

    if (index->field_84 > 0)
    {
        idx = &index->field_70;
        sub_464a80(v18, v19, index->field_84);
        a0 = index->field_84;
        v5 = a0;
        if (index->field_74 >= a0)
        {
            v20 = *((int *)&idx->padding_4[12]);
            v5 = a0;
            if (!((char)v20 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&idx->padding_4[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                *((unsigned int *)&idx->padding_4[0]) = 0;
                idx->field_0 = 0xfff0bdc1;
                *((char **)&idx->padding_4[8]) = &g_4b2ed0;
                *((unsigned int *)&idx->padding_4[12]) = v20 | 1;
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
            *((unsigned int *)&idx->padding_4[0]) = a0;
            idx->field_0 = a0 - 1;
            if (/* unsupported instruction */)
            {
                *((int *)&idx->padding_4[4]) = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                *((unsigned int *)&idx->padding_4[4]) = nan;
                /* unsupported instruction */
            }
            v21 = 7;
            index->field_84 = 0;
            if (index->field_88 != 7)
            {
                for (i = &index->field_1c; v21; i = i->padding_4)
                {
                    v21 -= 1;
                    *(node) = i->field_0;
                    node += 1;
                }
                return a2;
            }
            else
            {
                for (node = a2; v21; j = j->padding_4)
                {
                    v21 -= 1;
                    *(node) = j->field_0;
                    node += 1;
                }
                return a2;
            }
        }
    }
    v25 = j->field_88;
    if (v25 == 7)
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
        v26 = 7;
        iter = &v12;
        for (iter1 = j; v26; iter1 = iter1->padding_4)
        {
            v26 -= 1;
            *(iter) = iter1->field_0;
            iter += 1;
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
            *((int *)&v7) = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            *((unsigned int *)&v7) = nan;
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
            (&v7)[1] = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            (&v7)[1] = nan;
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
            v10 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v10 = nan;
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
            v11 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v11 = nan;
            /* unsupported instruction */
        }
        sub_406250();
        v29 = 7;
        for (l = &v7; v29; l += 1)
        {
            v29 -= 1;
            j->field_0 = *(l);
            j = j->padding_4;
        }
        i1 = &v7;
    }
    else
    {
        if (v25 == 0x11)
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
            iter2 = &j->field_54;
            v33 = 7;
            v34 = &v12;
            for (m = j; v33; m = m->padding_4)
            {
                v33 -= 1;
                *(v34) = m->field_0;
                v34 += 1;
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
                *((int *)&v7) = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                *((unsigned int *)&v7) = nan;
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
                (&v7)[1] = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                (&v7)[1] = nan;
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
                v10 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                v10 = nan;
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
                v11 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                v11 = nan;
                /* unsupported instruction */
            }
            sub_406250();
            v36 = 7;
            for (n = &v7; v36; n += 1)
            {
                v36 -= 1;
                j->field_0 = *(n);
                j = j->padding_4;
            }
            i0 = sub_406110();
            for (v39 = 7; v39; i0 += 1)
            {
                v39 -= 1;
                iter2->field_0 = *(i0);
                iter2 = iter2->padding_4;
            }
            i1 = &v7;
        }
        else
        {
            if (v25 == 8)
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
                v3 = a0;
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
                    v5 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v5 = nan;
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
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_4060d0();
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
                v2 = v40;
                if (/* unsupported instruction */)
                {
                    v2 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v2 = nan;
                    /* unsupported instruction */
                }
                sub_4060d0();
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
                v1 = v40;
                if (/* unsupported instruction */)
                {
                    v1 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v1 = nan;
                    /* unsupported instruction */
                }
                v41 = sub_4060d0();
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
                v0 = v40;
                v4 = v41;
                if (/* unsupported instruction */)
                {
                    v0 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v0 = nan;
                    /* unsupported instruction */
                }
                sub_4060d0();
                sub_406110(v0);
                sub_406110();
            }
            else
            {
                sub_406050();
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
                v3 = v40;
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
                sub_406090();
                sub_4060d0(v3);
            }
            i1 = sub_406110();
        }
    }
    v42 = 7;
    for (v43 = v15; v42; i1 += 1)
    {
        v42 -= 1;
        *(v43) = *(i1);
        v43 += 1;
    }
    return v15;
}
