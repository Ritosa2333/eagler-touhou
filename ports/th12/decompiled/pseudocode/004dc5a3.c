// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dc5a3; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dc5a3_1 {
    char padding_0[328965];
    char field_50505;
} st_4dc5a3_1;

typedef struct st_4dc5a3_0 {
    char field_0;
    char padding_1[79];
    char field_50;
} st_4dc5a3_0;


int sub_4dc5a3(void)
{
    void* v3;  // ecx
    char v4;  // al
    unsigned int v13;  // 4213
    char v14;  // 4256
    char *v15;  // eax
    unsigned int v16;  // eax
    unsigned int v17;  // eax
    st_4dc5a3_1 *v19;  // eax
    unsigned int v20;  // eax
    unsigned int v21;  // eax
    unsigned int v22;  // ecx
    void* v5;  // ebx
    unsigned int *v23;  // ecx
    unsigned int v24;  // 4192
    unsigned int v25;  // 4198
    unsigned int v26;  // 4397
    unsigned int v27;  // 4204
    unsigned int v28;  // 4398
    unsigned int v29;  // 4210
    unsigned int v30;  // 4399
    unsigned int v31;  // 4216
    unsigned int v32;  // 4400
    char v6;  // 4101
    unsigned int v33;  // 4222
    unsigned int v34;  // 4401
    unsigned int v35;  // 4402
    int v36;  // esi
    int v37;  // ebp
    unsigned short v7;  // es
    void* v8;  // edx
    unsigned int v9;  // eax
    char v10;  // 4120
    unsigned int v11;  // 4209
    st_4dc5a3_0 *v12;  // eax
    unsigned short v0;  // [bp-0x4]
    char v1;  // [bp+0x0]

    *((char *)v3) = *((char *)v3) + v4;
    v6 = *((char *)v5);
    v0 = v7;
    *((char *)v8) = *((char *)v8) | *((char *)&v3);
    v9 = _INSERT(CONCAT(v4, 0), 0, v4 + v6 + 5 | 14);
    v10 = *(v9 + (char *)v5);
    *(v9 + (char *)v5) = *(v9 + (char *)v5) + *((char *)&v8);
    v11 = _ccall(7, v10, *((char *)&v8) ^ 0, 0);
    v12 = _INSERT(v9, 0, (v4 + v6 + 5 | 14) - 32 - ((char)v11 & 1));
    v12->field_0 = v12->field_0 - *((char *)((void*)&v8 + 1));
    v13 = _ccall(0, 4, v12->field_50, (v4 + v6 + 5 | 14) - 32 - ((char)v11 & 1), 0);
    if (v13 & 1)
        goto LABEL_0x4dc53c;
    v14 = *(0xe0c0);
    v15 = _INSERT(v12, 0, v14);
    *(v15) = *(v15) + v14;
    *(v15) = *(v15) + v14;
    *(v15) = *(v15) + v14;
    *((char **)v3) = &v15[*((int *)v3)];
    *((char **)v3) = &v15[*((int *)v3)];
    v16 = _INSERT(v15, 0, v14 + *((char *)v8) + *((char *)v8));
    v17 = v16 + *((int *)v5) + *((int *)v5);
    v19 = _INSERT(v17, 0, (char)v17 + 8) + 0x50505;
    v19->padding_0[0] = v19->padding_0[0] + *((char *)&v19);
    *((char *)v3) = *((char *)v3) + *((char *)&v19);
    *((st_4dc5a3_1 **)v8) = &v19->padding_0[*((int *)v8)];
    v20 = _INSERT(v19, 0, *((char *)&v19) + *((char *)v5));
    v21 = v20 + *((int *)((char *)&edi + v20)) + 117835269;
    *((char *)v21) = *((char *)v21) | *((char *)&v3);
    *((unsigned long *)v3) = *((int *)v3) | v3;
    v22 = _INSERT(v3, 0, *((char *)&v3) | *((char *)v8));
    v23 = v22 | *((int *)v5);
    v24 = *(v23);
    *(v23) = *(v23) + v8;
    v25 = *(v23);
    v26 = _ccall(9, v24, v8 ^ 0, 0);
    *(v23) = *(v23) + v8 + (v26 & 1);
    v27 = *(v23);
    v28 = _ccall(9, v25, v8 ^ v26 & 1, v26 & 1);
    *(v23) = *(v23) + v8 + (v28 & 1);
    v29 = *(v23);
    v30 = _ccall(9, v27, v8 ^ v28 & 1, v28 & 1);
    *(v23) = *(v23) + v8 + (v30 & 1);
    v31 = *(v23);
    v32 = _ccall(9, v29, v8 ^ v30 & 1, v30 & 1);
    *(v23) = *(v23) + v8 + (v32 & 1);
    v33 = *(v23);
    v34 = _ccall(9, v31, v8 ^ v32 & 1, v32 & 1);
    *(v23) = *(v23) + v8 + (v34 & 1);
    v35 = _ccall(9, v33, v8 ^ v34 & 1, v34 & 1);
    *(v23) = *(v23) + v8 + (v35 & 1);
    sub_4dc615(v36, v37, &v1, v5, v8, v3, v12);
    return;
}
