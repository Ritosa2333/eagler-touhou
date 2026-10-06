// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47fb56; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47fb56_7 {
    struct st_47fb56_0 *field_0;
} st_47fb56_7;

typedef struct st_47fb56_1 {
    unsigned int field_0;
} st_47fb56_1;

typedef struct st_47fb56_2 {
    char padding_0[4];
    unsigned int field_4;
} st_47fb56_2;

typedef struct st_47fb56_0 {
    char padding_0[4];
    struct st_47fb56_1 *field_4;
} st_47fb56_0;

typedef struct st_47fb56_6 {
    struct st_47fb56_7 *field_0;
    unsigned int field_4;
} st_47fb56_6;

extern unsigned int g_49dc38[4];
extern unsigned int g_49dc5c[4];
extern unsigned int g_49dc80[4];
extern void* g_4b4318;

st_47fb56_2 * sub_47fb56(st_47fb56_2 *a0, char a1, char *a2)
{
    unsigned int v31;  // edx
    unsigned int v32;  // ebx
    st_47fb56_6 *v41;  // eax
    unsigned int v42;  // ecx
    unsigned int v43;  // ecx
    unsigned int v44;  // ecx
    unsigned int v45;  // eax
    unsigned int v46;  // eax
    unsigned int v47;  // eax
    unsigned int v33;  // esi
    unsigned int *v52;  // eax
    unsigned int v53;  // ecx
    st_47fb56_6 *v56;  // eax
    unsigned int v34;  // esi
    void* iter;  // eax
    st_47fb56_2 *v37;  // eax
    unsigned int v38;  // ecx
    st_47fb56_2 *v39;  // eax
    st_47fb56_0 **v40;  // ecx
    char *v0;  // [bp-0xcc]
    int v1;  // [bp-0xc8]
    st_47fb56_2 *v2;  // [bp-0xbc]
    char *v3;  // [bp-0xb8], Other Possible Types: int
    unsigned int v4;  // [bp-0xb4]
    unsigned int *v5;  // [bp-0xb0], Other Possible Types: unsigned int
    unsigned int v6;  // [bp-0xac]
    unsigned int v7;  // [bp-0xa8]
    char v8;  // [bp-0xa4]
    char v9;  // [bp-0x9c]
    char v10;  // [bp-0x94]
    char v11;  // [bp-0x8c]
    char v12;  // [bp-0x84]
    char v13;  // [bp-0x7c]
    char v14;  // [bp-0x74]
    char v15;  // [bp-0x6c]
    char v16;  // [bp-0x64]
    char v17;  // [bp-0x5c]
    char v18;  // [bp-0x54]
    char v19;  // [bp-0x4c]
    char v20;  // [bp-0x44]
    char v21;  // [bp-0x3c]
    char v22;  // [bp-0x34]
    char v23;  // [bp-0x2c]
    char v24;  // [bp-0x24]
    st_47fb56_0 **v25;  // [bp-0x1c], Other Possible Types: unsigned int
    unsigned int v26;  // [bp-0x18]
    char v27;  // [bp-0x14]
    unsigned int v28;  // [bp-0x10]
    st_47fb56_0 **v29;  // [bp-0xc]
    unsigned int v30;  // [bp-0x8]

    v31 = *((char *)g_4b4318);
    v7 = v32;
    v30 &= 0xffff0000;
    v26 &= 0xffff0000;
    v6 = v33;
    v34 = 0;
    iter = g_4b4318 + 1;
    v29 = NULL;
    v25 = 0;
    g_4b4318 = iter;
    if (v31 <= 65)
    {
        if (v31 == 65)
            goto LABEL_4800c3;
        if (!v31)
        {
            g_4b4318 = iter - 1;
            v3 = 1;
            sub_47e1ce(v3);
            return a0;
        }
        if (v31 > 47)
        {
            if (v31 <= 49)
            {
                v26 &= 0xffff0000;
                v25 = NULL;
                if (a1)
                {
                    sub_47e7bf(sub_47ec02(&v8, 60, sub_47f992(&v17)));
                    if (v25 && (char)*(v25)->field_4() == 62)
                        sub_47e9f6(32);
                    sub_47e9f6(62);
                    if (a2)
                        *(a2) = 1;
                    if (!*((char *)g_4b4318))
                    {
                        v37 = a0;
                        *((st_47fb56_0 ***)&v37->padding_0[0]) = v25;
                        v37->field_4 = v26;
                        return v37;
                    }
                    iter = g_4b4318 + 1;
                    g_4b4318 = iter;
                }
                v39 = sub_48023a(&v21, 0, 0);
                v40 = (st_47fb56_0 **)v39->padding_0;
                v29 = v40;
                v30 = v39->field_4;
                g_4b4318 = iter;
                if (v40 && *((char *)g_4b4318 - 1) == 49)
                {
                    v41 = sub_47ec02(&v11, 126, &v29);
                    v40 = v41->field_0;
                    v29 = v40;
                    v30 = v41->field_4;
                }
                if (v25)
                {
                    sub_47e7bf(&v25);
                    v37 = a0;
                    *((st_47fb56_0 ***)&v37->padding_0[0]) = v29;
                    v37->field_4 = v30;
                    return v37;
                }
                v37 = a0;
                *((st_47fb56_0 ***)&v37->padding_0[0]) = v40;
                v37->field_4 = v30;
                return v37;
            }
            if (v31 <= 57)
            {
                v5 = *((int *)(4 * *((char *)iter - 1) + 4840360));
                goto LABEL_47fbbf;
            }
        }
    }
    else
    {
        if (v31 == 66)
        {
            v34 = 1;
LABEL_4800c3:
            sub_47e896(*((int *)(4 * *((char *)iter - 1) + 4840332)));
            if (v34)
            {
                if (v29)
                    v30 |= 0x200;
LABEL_47fbec:
                v37 = a0;
                *((st_47fb56_0 ***)&v37->padding_0[0]) = v29;
                v37->field_4 = v30;
                return v37;
            }
LABEL_47fbc7:
            if (v29)
            {
                v56 = sub_47ec4a(&v24, "operator", &v29);
                v29 = v56->field_0;
                v30 = v56->field_4;
                v37 = a0;
                *((st_47fb56_0 ***)&v37->padding_0[0]) = v29;
                v37->field_4 = v30;
                return v37;
            }
            goto LABEL_47fbec;
        }
        if (v31 > 66)
        {
            if (v31 <= 90)
                goto LABEL_4800c3;
            if (v31 == 95)
            {
                v42 = *((char *)iter);
                iter += 1;
                g_4b4318 = iter;
                if (v42 <= 79)
                {
                    if (v42 >= 0x44)
                    {
                        v5 = *((int *)(4 * *((char *)iter - 1) + 4840476));
                        sub_47e59a(a0, 95, v5);
                        return a0;
                    }
                    if (v42 <= 57)
                    {
                        if (v42 == 57)
                        {
                            sub_47e59a(&v27, 95, g_49dc38[*((char *)iter - 1)]);
                            v37 = a0;
                            *((unsigned int *)&v37->padding_0[0]) = v27;
                            v37->field_4 = v28 | 0x8000;
                            return v37;
                        }
                        if (!v42)
                        {
                            g_4b4318 = iter - 1;
                            v3 = 1;
                            sub_47e1ce(v3);
                            return a0;
                        }
                        if (v42 > 47)
                        {
                            if (v42 > 54)
                            {
                                if (v42 <= 56)
                                {
                                    v5 = g_49dc38[*((char *)iter - 1)];
                                    sub_47e59a(a0, 95, v5);
                                    return a0;
                                }
                            }
                            else
                            {
                                v5 = g_49dc38[*((char *)iter - 1)];
                                goto LABEL_47fbbf;
                            }
                        }
                    }
                    else
                    {
                        if (v42 != 63)
                        {
                            if (v42 > 64)
                            {
                                if (v42 > 66)
                                {
                                    if (v42 == 67)
                                    {
                                        v5 = 1;
                                        v4 = "`string'";
                                        sub_47f4da(&v27);
                                        v37 = a0;
                                        *((unsigned int *)&v37->padding_0[0]) = v27;
                                        v37->field_4 = v38;
                                        return v37;
                                    }
                                }
                                else
                                {
                                    v5 = *((int *)(4 * *((char *)iter - 1) + 4840476));
                                    sub_47e59a(a0, 95, v5);
                                    return a0;
                                }
                            }
                        }
                        else
                        {
                            v43 = *((char *)iter);
                            iter += 1;
                            g_4b4318 = iter;
                            if (!v43)
                            {
                                g_4b4318 = iter - 1;
                                v3 = 1;
                                sub_47e1ce(v3);
                                return a0;
                            }
                            if (v43 == 48)
                            {
                                v5 = 0;
                                v4 = "`anonymous namespace'";
                                sub_47f4da(&v27);
                                v38 = v28 | 0x1000;
                                v37 = a0;
                                *((unsigned int *)&v37->padding_0[0]) = v27;
                                v37->field_4 = v38;
                                return v37;
                            }
                        }
                    }
                }
                else if (v42 <= 84)
                {
                    if (v42 < 83)
                    {
                        v44 = v42 - 80;
                        if (v42 == 80)
                        {
                            sub_47e896(*((int *)(4 * *((char *)iter - 1) + 4840476)));
                            v3 = 0;
                            v52 = sub_47fb56(&v12, 0, NULL);
                            v25 = *(v52);
                            v26 = v52[1];
                            if (!*(v52))
                            {
                                v3 = &v25;
                                v2 = a0;
                                sub_47e9ae(v0, v1);
                                return a0;
                            }
                            if (!(v52[1] & 0x400))
                            {
                                v3 = &v25;
                                v2 = a0;
                                sub_47e9ae(v0, v1);
                                return a0;
                            }
                        }
                        else if (v44 == 1)
                        {
                            v37 = a0;
                            *((st_47fb56_0 ***)&v37->padding_0[0]) = v29;
                            v37->field_4 = v30;
                            return v37;
                        }
                        else if (v44 == 2)
                        {
                            sub_47e896(*((int *)(4 * *((char *)iter - 1) + 4840476)));
                            if (!*((char *)g_4b4318))
                            {
                                sub_47e79b(a0, 1);
                                return a0;
                            }
                            v45 = *((char *)g_4b4318);
                            v46 = v45 - 48;
                            if (v45 - 48 >= 0 && v46 < 5)
                            {
                                sub_47e896(g_49dc5c[v46]);
                                v47 = *((char *)g_4b4318);
                                g_4b4318 = g_4b4318 + 1;
                                if (v47 == 48)
                                {
                                    sub_4827df(&v27, 0);
                                    v1 = 32;
                                    v0 = &v14;
                                    sub_47ec6e(&v14, 32, &v16, &v29, a0, &v25);
                                    sub_47e9ae(&v14, 32, &v16, &v29, a0, &v25);
                                    sub_47e9ae(v0, v1);
                                    return a0;
                                }
                                else if (v47 == 49)
                                {
                                    sub_47e9ae(&v27, &v25);
                                    sub_47f57e(&v13, &v19, 44);
                                    sub_47e7bf(sub_47ec6e(&v19, 44));
                                    sub_47f57e(&v9, &v23, 44);
                                    sub_47e7bf(sub_47ec6e(&v23, 44));
                                    sub_47f57e(&v22, &v15, 44);
                                    sub_47e7bf(sub_47ec6e(&v15, 44));
                                    sub_47ecb6(&v18, 0, &v20, 41);
                                    sub_47e7bf(sub_47ec6e(&v20, 41));
                                    sub_47ec6e(a0, 39);
                                    return a0;
                                }
                                else if (v47 - 50 > 2)
                                {
                                    g_4b4318 = g_4b4318 - 1;
                                    v3 = 1;
                                    sub_47e1ce(v3);
                                    return a0;
                                }
                                else
                                {
                                    v3 = &v25;
                                    v2 = a0;
                                    sub_47e9ae(v0, v1);
                                    return a0;
                                }
                            }
                        }
                    }
                    else
                    {
                        v5 = *((int *)(4 * *((char *)iter - 1) + 4840476));
                        sub_47e59a(a0, 95, v5);
                        return a0;
                    }
                }
                else
                {
                    if (v42 >= 0x55)
                    {
                        if (v42 <= 86)
                        {
                            v5 = *((int *)(4 * *((char *)iter - 1) + 4840476));
LABEL_47fbbf:
                            sub_47e896(v5);
                            goto LABEL_47fbc7;
                        }
                        else if (v42 > 87)
                        {
                            if (v42 <= 89)
                            {
                                v5 = *((int *)(4 * *((char *)iter - 1) + 4840476));
                                sub_47e59a(a0, 95, v5);
                                return a0;
                            }
                            else if (v42 == 95)
                            {
                                v53 = *((char *)iter);
                                g_4b4318 = iter + 1;
                                if (v53 >= 65)
                                {
                                    if (v53 <= 0x44)
                                    {
                                        v5 = g_49dc80[*((char *)g_4b4318 - 1)];
                                        sub_47e59a(a0, 95, v5);
                                        return a0;
                                    }
                                    else if (v53 <= 70)
                                    {
                                        sub_47e59a(&v27, 95, g_49dc80[*((char *)g_4b4318 - 1)]);
                                        if (*((char *)g_4b4318) == 63)
                                        {
                                            sub_47e7bf(sub_481298(&v10));
                                            if (*((char *)g_4b4318) == 64)
                                                g_4b4318 = g_4b4318 + 1;
                                        }
                                        else
                                        {
                                            sub_47e7bf(sub_48061b(&v24));
                                        }
                                        sub_47ea48("''");
                                        v37 = a0;
                                        *((unsigned int *)&v37->padding_0[0]) = v27;
                                        v37->field_4 = v28;
                                        return v37;
                                    }
                                    else if (v53 <= 74)
                                    {
                                        v5 = g_49dc80[*((char *)g_4b4318 - 1)];
                                        sub_47e59a(a0, 95, v5);
                                        return a0;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    v4 = 2;
    sub_47e1ce(v3);
    return a0;
}
