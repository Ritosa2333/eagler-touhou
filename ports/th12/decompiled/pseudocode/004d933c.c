// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d933c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_0;

int sub_4d933c(void)
{
    char *v11;  // eax
    void* v12;  // esi
    void* v21;  // eax
    char v22;  // 4138
    unsigned int v23;  // 4518
    void* idx;  // eax
    char v25;  // 4223
    unsigned int v26;  // 4575
    char v27;  // 4241
    unsigned int v28;  // 4593
    char v29;  // 4263
    unsigned int v30;  // 4613
    char *idx1;  // edx
    char v31;  // 4114
    unsigned int v32;  // 4101
    char v33;  // 4110
    unsigned int v34;  // 4177
    char v35;  // 4120
    unsigned int v36;  // 4191
    char *v37;  // eax
    char *v38;  // eax
    char *index;  // eax
    unsigned int v40;  // edx
    void* v14;  // ecx
    char *v41;  // esi
    char v42;  // 4161
    void* v43;  // eax
    char *v44;  // esi
    unsigned int v45;  // 4242
    char v46;  // 4102
    unsigned int v47;  // 4144
    char v15;  // bl
    char *v16;  // ebx
    char v17;  // 4164
    void* v18;  // ecx
    unsigned int v19;  // 4226
    unsigned int v20;  // eax
    int v0;  // [bp-0x9]
    unsigned int v1;  // [bp-0x4]
    unsigned int v2;  // [bp+0x3]
    unsigned int v4;  // [bp+0x7]
    char *v5;  // [bp+0xf]
    char *v6;  // [bp+0x17]
    unsigned int v7;  // [bp+0x23]
    char *v8;  // [bp+0x27]
    unsigned int v9;  // [bp+0x2b]
    void* v10;  // [bp+0x2f]

    v1 = 3509055806;
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *((char **)&(&v0)[1]) = v11;
    *(v11) = *(v11) + *((char *)&v11);
    *((char **)v12) = &v11[*((int *)v12)];
    idx1[71] = idx1[71] + *((char *)&v14);
    v15 = (char)v11;
    *(v16) = *(v16) + *((char *)&v16);
    *(v16) = *(v16) + *((char *)&v16);
    *(v16) = *(v16) + *((char *)&v16);
    v17 = *(v16);
    *(v16) = *(v16) + *((char *)&v16);
    v18 = v14 - 1;
    v19 = _ccall(5, 1, v17, *((char *)&v16), 0);
    if (!(v14 != 0x1 & v19 & 1))
        goto LABEL_4d9367;
LABEL_4d9367:
    v20 = _INSERT(v16, 0, *((char *)&v16) + *((char *)v18));
    v21 = v20 | *((int *)v18);
    *((unsigned long *)v21) = *((int *)v21) | v21;
    *((char *)v12) = *((char *)v12) + v15;
    *((char *)v21) = *((char *)v21) + *((char *)&v21);
    *((char *)v12) = *((char *)v12) + v15;
    *((char *)v21) = *((char *)v21) + *((char *)&v21);
    *((char *)v21) = *((char *)v21) + *((char *)&v21);
    *((char *)v21) = *((char *)v21) + *((char *)&v21);
    *((unsigned int **)v21) = *((int *)v21) + (char *)&v40;
    v22 = *((char *)v21);
    *((char *)v21) = *((char *)v21) + *((char *)&v21);
    v23 = _ccall(1, v22, *((char *)&v21), 0);
    *((char *)v21) = *((char *)v21) + *((char *)&v21) + ((char)v23 & 1);
    *((char *)v21) = *((char *)v21) + *((char *)&v21);
    *((char *)v21) = *((char *)v21) ^ *((char *)&v21);
    *((char *)v21) = *((char *)v21) + *((char *)&v21);
    *((char *)v21) = *((char *)v21) + *((char *)&v21);
    *((char *)v21) = *((char *)v21) + *((char *)&idx1);
    *((char *)v21) = *((char *)v21) + *((char *)&v21);
    *(idx1) = *(idx1) + *((char *)&v21);
    *((char *)v21) = *((char *)v21) + *((char *)&v21);
    idx = v21;
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    g_0 = g_0 + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)(idx * 2)) = *((char *)(idx * 2)) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *(idx1) = *(idx1) + *((char *)&idx);
    *((char *)idx - 127) = *((char *)idx - 127) + *((char *)&idx);
    v25 = *((char *)idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    v26 = _ccall(1, v25, *((char *)&idx), 0);
    *((char *)idx) = *((char *)idx) + *((char *)&idx) + ((char)v26 & 1);
    *((char *)idx) = *((char *)idx) + *((char *)&idx1);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    v27 = *((char *)idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    v28 = _ccall(1, v27, *((char *)&idx), 0);
    *((char *)idx) = *((char *)idx) + *((char *)&idx) + ((char)v28 & 1);
    *((char *)idx) = *((char *)idx) + *((char *)&idx1);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    v29 = *((char *)idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    v30 = _ccall(1, v29, *((char *)&idx), 0);
    *((char *)idx) = *((char *)idx) + *((char *)&idx) + ((char)v30 & 1);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) | *((char *)((void*)&idx1 + 1));
    *((char *)&idx[1]) = (char)idx[1] + *((char *)&v18);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)&idx[134217839]) = (char)idx[134217839] + *((char *)((void*)&idx1 + 1));
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx1);
    v31 = *((char *)v12);
    *((char *)v12) = v31 + *((char *)((void*)&v18 + 1));
    if (v31 + *((char *)((void*)&v18 + 1)))
    {
        v32 = _ccall(8, 1, v31, *((char *)((void*)&v18 + 1)), 0);
        if (v32 & 1)
            goto LABEL_0x4d94c2;
        *((char *)idx) = *((char *)idx) + *((char *)&idx);
        *((char *)idx) = *((char *)idx) + *((char *)&idx);
        *((char *)idx) = *((char *)idx) & *((char *)&idx);
        v33 = *((char *)idx);
        *((char *)idx) = *((char *)idx) + *((char *)&idx);
        v34 = _ccall(1, v33, *((char *)&idx), 0);
        *((char *)idx) = *((char *)idx) + *((char *)&idx) + ((char)v34 & 1);
        v35 = *((char *)idx);
        *((char *)idx) = *((char *)idx) + *((char *)&idx);
        v36 = _ccall(1, v35, *((char *)&idx), 0);
        v37 = _INSERT(idx, 0, *((char *)&idx) + ((char)v36 & 1));
        *(v37) = *(v37) + *((char *)&idx) + ((char)v36 & 1);
        v38 = _INSERT(v37, 0, *((char *)&idx) + ((char)v36 & 1));
        *(v38) = *(v38) + *((char *)&idx) + ((char)v36 & 1);
        *(v38) = *(v38) + *((char *)&idx) + ((char)v36 & 1);
        *(v38) = *(v38) + *((char *)&idx) + ((char)v36 & 1);
        *(v38) = *(v38) + *((char *)&idx) + ((char)v36 & 1);
        *(v38) = *(v38) + *((char *)&idx) + ((char)v36 & 1);
        *(v38) = *(v38) + *((char *)&idx) + ((char)v36 & 1);
        *(v38) = *(v38) + *((char *)((void*)&v38 + 1));
        index = _INSERT(v38, 0, *((char *)&idx) + ((char)v36 & 1) + *((char *)((void*)&v38 + 1)));
        if (*((char *)&idx) + ((char)v36 & 1) + *((char *)((void*)&v38 + 1)) >= 0 && (index = v5, v40 = v4, v41 = (char *)*((unsigned int *)(void*)&v0), (unsigned int)(*((char *)(void*)&idx) + ((char)v36 & 1)) + (unsigned int)*((char *)((void*)&v38 + 1))))
        {
            *(index) = *(index) + *((char *)&index);
            *(index) = *(index) + (char)v40;
            *(index) = *(index) + *((char *)&index);
            *(index) = *(index) + *((char *)((void*)&v40 + 1));
            *(index) = *(index) + *((char *)&index);
            *(v41) = *(v41) + *((char *)&index);
            *(index) = *(index) + *((char *)&index);
            *(index) = *(index) + (char)v2;
            *(index) = *(index) + *((char *)&index);
            *(index) = *(index) + *((char *)&index);
            *(index) = *(index) + *((char *)&index);
            *(index) = *(index) + *((char *)&index);
            *(index) = *(index) + *((char *)&index);
            *(index) = *(index) + *((char *)&index);
            *(index) = *(index) + *((char *)&index);
            index[1] = index[1] + *((char *)&index + 1);
            v42 = *(v41);
            *(v41) = v42 >> 4;
            v43 = v10;
            v44 = v6;
            v45 = _ccall(4, 25, v42 >> 4, v42 >> 3, 0);
            if (v45 & 1)
                goto LABEL_0x4d94ff;
            *((char *)v43) = *((char *)v43) + *((char *)&v43);
            v46 = *((char *)v43);
            *((char *)v43) = *((char *)v43) + *((char *)&v43);
            v47 = _ccall(1, v46, *((char *)&v43), 0);
            *((char *)v43) = *((char *)v43) + *((char *)&v43) + ((char)v47 & 1);
            *((char *)v43) = *((char *)v43) + *((char *)&v43);
            idx = v43 + 1;
            *((char *)idx) = *((char *)idx) + *((char *)&idx);
            *(v8) = *(v8) + (char)v9;
            *((char *)idx) = *((char *)idx) + *((char *)&idx);
            *(v44) = *(v44) + (char)v7;
            *((char *)idx) = *((char *)idx) + *((char *)&idx);
        }
        else
        {
            *(index) = *(index) + *((char *)&index);
            *(index) = *(index) + *((char *)&index);
            *(index) = *(index) + *((char *)&index);
            *(index) = *(index) + *((char *)&index);
            *(index) = *(index) + *((char *)&index);
            *(index) = *(index) + *((char *)&index);
            *(index) = *(index) + *((char *)&index);
            if ((char)v40 + *((char *)&index) >= 0)
                goto LABEL_0x4d955d;
        }
    }
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)idx) = *((char *)idx) + *((char *)&idx);
    *((char *)&idx[1]) = (char)idx[1] + *((char *)&idx + 1);
    *(v44) = 0;
}
