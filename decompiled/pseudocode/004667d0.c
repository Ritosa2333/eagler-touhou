// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4667d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4667d0_7 {
    char padding_0[144];
    struct st_4667d0_6 *field_90;
} st_4667d0_7;

typedef struct st_4667d0_5 {
    struct st_4667d0_0 *field_0;
} st_4667d0_5;

typedef struct st_4667d0_1 {
    unsigned int field_0;
} st_4667d0_1;

typedef struct st_4667d0_6 {
    char padding_0[46];
    unsigned short field_2e;
} st_4667d0_6;

typedef struct st_4667d0_0 {
    char padding_0[16];
    struct st_4667d0_1 *field_10;
    char padding_14[24];
    struct st_4667d0_1 *field_2c;
    char padding_30[24];
    struct st_4667d0_1 *field_48;
    struct st_4667d0_1 *field_4c;
} st_4667d0_0;

typedef struct st_4667d0_8 {
    char field_0;
} st_4667d0_8;

typedef struct st_4667d0_9 {
    char padding_0[4];
    struct st_4667d0_5 *field_4;
    unsigned int field_8;
    struct st_4667d0_7 *field_c;
    char padding_10[80];
    struct st_4667d0_8 *field_60;
    unsigned int field_64;
    struct st_4667d0_5 *field_68;
    unsigned int field_6c;
    unsigned int field_70;
} st_4667d0_9;


int sub_4667d0(st_4667d0_9 *idx)
{
    int v11;  // esi
    int v12;  // ebx
    char *v21;  // ecx
    int v22;  // edi
    unsigned int v23;  // eax
    char *v24;  // edx
    unsigned int v25;  // 4102
    unsigned int v26;  // eax
    unsigned int v13;  // eax
    int v14;  // eax
    st_4667d0_0 **v15;  // eax
    unsigned int v16;  // eax
    unsigned int v17;  // eax
    unsigned int v18;  // eax
    char *v19;  // edi
    unsigned int v20;  // eax
    st_4667d0_0 **v1;  // [bp-0x5c]
    st_4667d0_8 **i;  // [bp-0x58]
    char *v3;  // [bp-0x48]
    st_4667d0_8 **v4;  // [bp-0x40]
    unsigned int v5;  // [bp-0x2c]
    unsigned int v6;  // [bp-0x24]
    unsigned int v7;  // [bp-0x20]
    st_4667d0_8 **v8;  // [bp-0x1c]
    char v9;  // [bp-0x10]
    char v10;  // [bp-0x8]

    if (!idx->field_4)
    {
        return 2147746288;
    }
    else if (idx->field_c)
    {
        (*((int *)(*((int *)&idx->field_4->field_0->padding_0[0]) + 16)))(idx->field_4->field_0, &v10, &v9);
        if (idx->field_68 >= (char *)v8 - idx->field_70 && idx->field_68 < v8)
            return 2147746288;
        if (!idx->field_4->field_0)
            return 2147746288;
        v13 = sub_466220(v11, v12);
        if (v13 < NULL)
            return v13;
        v4 = NULL;
        if (v7)
        {
            v14 = sub_465fe0(idx->field_4->field_0);
            return v14 & (unsigned int)((v14 >= 0) + 1);
        }
        v5 = 0;
        v6 = 0;
        v15 = idx->field_4->field_0;
        v3 = &v6;
        i = idx->field_68;
        v1 = idx->field_4->field_0;
        v16 = *(v15)->field_2c(v15, idx->field_68, idx->field_70, &v5, &v10, &v6, &v9);
        if (v16 < NULL)
        {
            return v16;
        }
        else if (&v9)
        {
            return 0x8000ffff;
        }
        else
        {
            if (!idx->field_6c)
            {
                v17 = sub_466d70(&v10, &v3);
                if (v17 < NULL)
                {
                    return v17;
                }
                else if (&v5 < &v10)
                {
                    v3 = &v5;
                    do
                    {
                        v18 = sub_466c90();
                        if (v18 < NULL)
                            return v18;
                        v19 = &v5;
                        v20 = sub_466d70(v19 + v15, &i);
                        if (v20 < NULL)
                            return v20;
                        i = v19 + &v3;
                    } while (i < v4);
                }
            }
            else
            {
                v21 = &v10;
                sub_477420(v21, (unsigned int)((idx->field_c->field_90->field_2e != 8) + 1) & 128, v22);
            }
            (*((int *)(*((int *)&idx->field_4->field_0->padding_0[0]) + 76)))(idx->field_4->field_0, idx->field_4->field_0, v12, 0, 0);
            v23 = (*((int *)(*((int *)&idx->field_4->field_0->padding_0[0]) + 16)))(idx->field_4->field_0, &v1, 0);
            if (v23 < NULL)
                return v23;
            v24 = &idx->field_60->field_0;
            idx->field_64 = idx->field_64 + (v21 < v24 ? idx->field_8 - v24 + v21 : v21 - v24);
            v25 = idx->field_6c;
            v26 = idx->field_64;
            idx->field_60 = v21;
            if (v25 && v26 >= *((int *)&idx->field_c->padding_0[44]))
                (*((int *)(*((int *)&idx->field_4->field_0->padding_0[0]) + 72)))(idx->field_4->field_0);
            idx->field_68 = (idx->field_68 + v15) % idx->field_8;
            return 0;
        }
    }
    else
    {
        return 2147746288;
    }
}
