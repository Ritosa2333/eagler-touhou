// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45c900; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45c900_0 {
    char padding_0[60];
    unsigned int field_3c;
    char padding_40[892];
    char field_3bc[4];
    char field_3bf;
    unsigned int field_3c0;
    char padding_3c4[56];
    unsigned int field_3fc;
    char padding_400[120];
    int field_478;
    unsigned int field_47c;
} st_45c900_0;


int sub_45c900(void)
{
    unsigned int v16;  // ebx
    st_45c900_0 *idx;  // eax
    unsigned int v18;  // eax
    unsigned int v19;  // esi
    unsigned int v20;  // edi
    unsigned int v21;  // eax
    unsigned int v0;  // [bp-0x54]
    unsigned int v1;  // [bp-0x50]
    unsigned int v2;  // [bp-0x4c]
    unsigned int v3;  // [bp-0x48]
    unsigned int v4;  // [bp-0x44]
    unsigned int v5;  // [bp-0x40]
    unsigned int v6;  // [bp-0x3c]
    unsigned int v7;  // [bp-0x34]
    unsigned int v8;  // [bp-0x30]
    unsigned int v9;  // [bp-0x2c]
    unsigned int v10;  // [bp-0x28]
    unsigned int v11;  // [bp-0x24]
    unsigned int v12;  // [bp-0x20]
    unsigned int v13;  // [bp-0x1c]
    unsigned int v14;  // [bp-0x18]
    unsigned int v15;  // [bp+0x4]

    v9 = v16;
    v18 = idx->field_478;
    v8 = v19;
    v7 = v20;
    if (!((char)v18 & 1))
    {
        return;
    }
    else if (!((char)v18 & 2))
    {
        return;
    }
    else
    {
        switch (v18 >> 23 & 31)
        {
        case 0:
            if (!idx->field_3bc[3])
                return;
            sub_45a9f0(v15);
            return;
        case 1:
            if (!idx->field_3bc[3])
                return;
            sub_45b1b0(v15);
            return;
        case 2:
            if (!idx->field_3bc[3])
                return;
            sub_45ae50(v15, idx);
            return;
        case 3:
            if (!idx->field_3bc[3])
                return;
            sub_45b1e0(v15);
            return;
        case 4:
            if (!idx->field_3bc[3])
                return;
            sub_45b5e0(v15);
            return;
        case 5:
            if (!idx->field_3bc[3])
                return;
            sub_45ba90();
            return;
        case 6:
            if (!idx->field_3bc[3])
                return;
            sub_45b610();
            return;
        case 7:
            if (!idx->field_3bc[3])
                return;
            sub_45bac0(v15, idx);
            return;
        case 8:
            if (!idx->field_3bc[3])
                return;
            sub_45bce0(v15, idx);
            return;
        case 9: case 12: case 13:
            sub_45c3a0(*((int *)&idx->padding_400[116]), *((int *)&idx->padding_3c4[52]) * 2);
            return;
        case 11:
            sub_45c800(v15, *((int *)&idx->padding_400[116]), *((int *)&idx->padding_3c4[52]) * 2);
            return;
        case 14: case 18: case 19: case 20:
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
                v12 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                v12 = nan;
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
            sub_45d8c0();
            if (idx->field_3c)
            {
                sub_45d8c0();
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
                    v13 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v13 = nan;
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
                    v14 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v14 = nan;
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
                    v12 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v12 = nan;
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
            }
            sub_459a60(idx);
            switch ((unsigned int)(idx->field_478) >> 23 & 31)
            {
            case 14:
                if (/* unsupported instruction */)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    break;
                }
                else
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    break;
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
                    v3 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v3 = nan;
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
                    v2 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v2 = nan;
                    /* unsupported instruction */
                }
                sub_45ce60();
                return;
            case 18:
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
                    v3 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v3 = nan;
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
                    v2 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v2 = nan;
                    /* unsupported instruction */
                }
                sub_45d0a0();
                return;
            case 19:
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
                    v3 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v3 = nan;
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
                    v2 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v2 = nan;
                    /* unsupported instruction */
                }
                sub_45d2e0();
                return;
            case 20:
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
                    v3 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v3 = nan;
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
                    v2 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v2 = nan;
                    /* unsupported instruction */
                }
                sub_45d380();
                return;
            case 15: case 16: case 17:
                break;
            default:
                return;
            }
        case 15: case 16:
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
                v12 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                v12 = nan;
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
            sub_45d8c0();
            if (idx->field_3c)
            {
                sub_45d8c0();
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
                    v13 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v13 = nan;
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
                    v14 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v14 = nan;
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
                    v12 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v12 = nan;
                    /* unsupported instruction */
                }
            }
            sub_459a60(idx);
            v21 = (unsigned int)(idx->field_478) >> 23 & 31;
            if (v21 == 15)
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
                v6 = *((int *)&idx->field_3bf);
                v5 = *((int *)&idx->padding_3c4[52]);
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
                    v3 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v3 = nan;
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
                    v2 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v2 = nan;
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
                    v1 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v1 = nan;
                    /* unsupported instruction */
                }
                sub_45d430();
            }
            else if (v21 == 16)
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
                v6 = (unsigned int)idx->field_3bc;
                v5 = *((int *)&idx->padding_3c4[52]);
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
                    v3 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v3 = nan;
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
                    v2 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v2 = nan;
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
                    v1 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v1 = nan;
                    /* unsupported instruction */
                }
                v0 = v15;
                sub_45d710();
                return;
            }
        case 17:
            if (!idx->field_3bc[3])
                return;
            sub_4302c0();
            sub_45bce0(v15, idx);
            sub_430300(v15, idx);
            return;
        }
    }
}
