// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4589e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4589e0_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    unsigned int field_14;
    char padding_18[12];
    unsigned int field_24;
    unsigned int field_28;
    unsigned int field_2c;
    unsigned int field_30;
    int field_34;
    unsigned int field_38;
    char padding_3c[4];
    unsigned int field_40;
    unsigned int field_44;
    unsigned int field_48;
} st_4589e0_0;

extern char g_4b2ed0;

int sub_4589e0(void)
{
    st_4589e0_0 *idx;  // eax
    st_4589e0_0 *index;  // esi
    unsigned int v26;  // ecx
    unsigned int v27;  // edx
    unsigned int *idx1;  // ebx
    unsigned int v29;  // eax
    unsigned int v30;  // esi
    unsigned int v31;  // eax
    unsigned int v32;  // ecx
    unsigned int v33;  // edx
    unsigned int v34;  // edx
    unsigned int *idx2;  // ebx
    int v18;  // edi
    unsigned int v36;  // esi
    unsigned int v37;  // eax
    unsigned int v38;  // ecx
    unsigned int v39;  // edx
    unsigned int v40;  // edx
    unsigned int v41;  // ebp
    unsigned int *v42;  // ebx
    unsigned int v43;  // eax
    unsigned int v44;  // eax
    unsigned int v45;  // eax
    int v19;  // esi
    unsigned int *v46;  // ebx
    unsigned int v47;  // ebp
    unsigned int v48;  // eax
    unsigned int v49;  // eax
    unsigned int *v50;  // ebx
    unsigned int v20;  // ecx
    unsigned int v21;  // eax
    unsigned int v22;  // 4103
    unsigned int v23;  // ecx
    unsigned int v24;  // edx
    unsigned int *v25;  // ebx
    int v0;  // [bp-0x44]
    unsigned int v1;  // [bp-0x40]
    unsigned int v2;  // [bp-0x3c]
    unsigned int v3;  // [bp-0x38]
    unsigned int v4;  // [bp-0x34]
    unsigned int v5;  // [bp-0x30]
    unsigned int v6;  // [bp-0x2c]
    unsigned int v7;  // [bp-0x28]
    unsigned int v8;  // [bp-0x24]
    unsigned int v9;  // [bp-0x20]
    unsigned int v10;  // [bp-0x1c]
    unsigned int v11;  // [bp-0x18]
    unsigned int v12;  // [bp-0x14]
    unsigned int v13;  // [bp-0x8]
    unsigned int v14;  // [bp-0x4]

    if (idx->field_44 > 0)
    {
        index = &idx->field_30;
        sub_464a80(v18, v19, v0, idx->field_44);
        v20 = idx->field_44;
        v1 = idx->field_44;
        if (idx->field_34 >= v20)
        {
            v21 = index->field_10;
            if (!((char)v21 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                index->field_8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                index->field_4 = 0;
                index->field_0 = 0xfff0bdc1;
                index->field_c = &g_4b2ed0;
                index->field_10 = v21 | 1;
            }
            /* unsupported instruction */
            /* unsupported instruction */
            index->field_4 = v20;
            index->field_0 = v20 - 1;
            index->field_8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v22 = idx->field_48;
            idx->field_44 = 0;
            if (v22 != 7)
            {
                v23 = idx->field_10;
                v24 = idx->field_14;
                *(v25) = idx->field_c;
                v25[1] = v23;
                v25[2] = v24;
            }
            else
            {
                v26 = idx->field_4;
                v27 = idx->field_8;
                *(idx1) = idx->field_0;
                idx1[1] = v26;
                idx1[2] = v27;
            }
            return;
        }
    }
    v29 = idx->field_48;
    if (v29 == 7)
    {
        v30 = idx->field_8;
        v31 = idx->field_c + idx->field_0;
        v32 = idx->field_10 + idx->field_4;
        v33 = idx->field_14;
        idx->field_0 = v31;
        v34 = v33 + v30;
        idx->field_4 = v32;
        idx->field_8 = v34;
        *(idx2) = v31;
        idx2[1] = v32;
        idx2[2] = v34;
        return;
    }
    else if (v29 == 0x11)
    {
        v36 = idx->field_8;
        v37 = idx->field_24 + idx->field_0;
        v38 = idx->field_28 + idx->field_4;
        v39 = idx->field_2c;
        idx->field_0 = v37;
        v40 = v39 + v36;
        idx->field_4 = v38;
        idx->field_8 = v40;
        v12 = idx->field_28 + idx->field_10;
        v41 = idx->field_2c + idx->field_14;
        idx->field_24 = idx->field_24 + idx->field_c;
        idx->field_28 = v12;
        idx->field_2c = v41;
        *(v42) = v37;
        v42[1] = v38;
        v42[2] = v40;
        return;
    }
    else
    {
        if (v29 == 8)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
            v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
            v43 = sub_4931e0();
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v44 = sub_4931e0();
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v14 = sub_4931e0();
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
            v8 = sub_4931e0();
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v9 = sub_4931e0();
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v10 = sub_4931e0();
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
            v5 = sub_4931e0();
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v6 = sub_4931e0();
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v7 = sub_4931e0();
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
            v11 = sub_4931e0();
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v12 = sub_4931e0();
            v45 = sub_4931e0();
            *(v46) = v11 + v5 + v8 + v43;
            v46[1] = v12 + v6 + v9 + v44;
            v46[2] = v45 + v7 + v10 + v14;
        }
        else
        {
            sub_459300();
            v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v47 = idx->field_4;
            v8 = idx->field_c - idx->field_0;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v9 = idx->field_10 - v47;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v3 = idx->field_8;
            v10 = idx->field_14 - idx->field_8;
            v48 = sub_4931e0();
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v13 = sub_4931e0();
            v49 = sub_4931e0();
            *(v50) = idx->field_0 + v48;
            v50[1] = v47 + v13;
            v50[2] = v49 + v3;
        }
        return;
    }
}
