// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db719; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4db719_0 {
    unsigned int field_0;
    char field_4;
    char field_5;
} st_4db719_0;

extern char g_159ffe34;
extern unsigned int g_a53ad60a;

int sub_4db719(void)
{
    void* v5;  // edi
    void* v6;  // edi
    void* v15;  // ecx
    unsigned int *v16;  // ebp
    unsigned int *v17;  // eax
    void* iter;  // edi
    unsigned int *v19;  // eax
    unsigned int v20;  // 4118
    char v21;  // 4097
    char v22;  // 4098
    unsigned int v23;  // ecx
    unsigned int v24;  // 4151
    unsigned short v7;  // ss
    unsigned int v25;  // ebp
    unsigned short v26;  // cs
    unsigned int *v27;  // eax
    unsigned int v28;  // eax
    unsigned int v29;  // eax
    unsigned int v30;  // 4109
    unsigned int v31;  // 4115
    st_4db719_0 *v8;  // esi
    st_4db719_0 *v9;  // esi
    unsigned int v10;  // ebx
    void* v11;  // edi
    st_4db719_0 *v12;  // esi
    unsigned int *v13;  // eax
    unsigned int v14;  // ebp
    void* v0;  // [bp-0x5]
    unsigned int v1;  // [bp-0x4]
    unsigned int v3;  // [bp+0xf]
    unsigned int v4;  // [bp+0x1f]

    v6 = v5 + 1;
    *((unsigned short *)&v1) = v7;
    v9 = v8;
    v10 = v1;
    *((unsigned int *)v6) = v9->field_0;
    v11 = v6 + 4;
    v12 = &v9->field_4;
    *(v13) = *(v13) | v11;
    v16 = v14 - *((int *)((char *)v15 - 0x44));
    v17 = (char *)v13 - 1;
    v0 = v11;
    iter = v11 + 4;
    v19 = (char *)v17 - 1;
    /* unsupported instruction */
    /* unsupported instruction */
    v20 = _ccall(8, 21, (unsigned int *)((char *)v17 - 1), 0, v17 < *((int *)v11));
    if (!(v20 & 1))
    {
        while (1)
        {
            v25 = *(v16);
            esp = v16 + 1;
            esp = esp - 4;
            *((unsigned int *)(esp - 4)) = v25;
            *((unsigned short *)(esp - 4)) = v26;
            *((char *)iter - 1347507218) = *((char *)iter - 1347507218) | 203;
            iter += 1;
            v27 = (char *)v19 - 588533935;
            esp = *((int *)(esp - 4));
            g_159ffe34 = *((char *)&v27);
            v28 = _INSERT(v27, 0, *((char *)&v27) - 124);
            if (v15)
            {
                v29 = v10;
                v10 = v28;
                g_a53ad60a = 1192779467;
                v30 = _ccall(4, *((char *)&v27) - 14, 110, 0);
                v31 = _ccall(10, 18, esp + 1, 0, v30);
                if (v31 & 1)
                    *((unsigned int *)iter) = _INSERT(v29, 0, 104);
                x86g_dirtyhelper_OUT(24267<32>, vvar_105{r8|4b}, 4<32>)
                v19 = v29 | 2267786305;
                v16 = v25 + 1;
            }
        }
    }
    else if ((char *)&v1 + 3 > 0x5e)
    {
        v21 = *((char *)iter);
        v22 = v12->field_0;
        v23 = _INSERT(v15, 0, *((char *)&v15) * 2 + (v22 < v21));
        *((char *)&iter[1]) = *((char *)&v12->field_0 + 1);
        v24 = _ccall(5, 7, *((char *)&v15), *((char *)&v15) ^ v22 < v21, v22 < v21);
        if (v23 != 1 & v24 & 1)
            goto LABEL_0x4db75e;
        v4 = v3;
    }
    else
    {
        v0 = v15;
    }
}
