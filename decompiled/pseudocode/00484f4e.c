// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x484f4e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_484f4e_7 {
    char field_0;
} st_484f4e_7;


int sub_484f4e(void)
{
    st_484f4e_7 **v13;  // ecx
    unsigned int v14;  // ebx
    unsigned int idx;  // edx
    unsigned int v24;  // eax
    unsigned int v25;  // eax
    unsigned int v26;  // ecx
    unsigned int v27;  // eax
    unsigned int v28;  // eax
    char v15;  // al
    unsigned int v29;  // eax
    unsigned int v30;  // eax
    int v31;  // eax
    unsigned int v32;  // eax
    unsigned int v16;  // eax
    unsigned int v33;  // eax
    unsigned int v34;  // eax
    unsigned int v35;  // eax
    unsigned int *v36;  // eax
    unsigned int v17;  // esi
    unsigned int v37;  // eax
    unsigned int v38;  // eax
    unsigned int v39;  // eax
    unsigned int v18;  // edi
    int v40;  // esi
    unsigned int v19;  // eax
    unsigned int v20;  // eax
    unsigned int v21;  // eax
    unsigned int v22;  // eax
    unsigned int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x24]
    st_484f4e_7 **v2;  // [bp-0x20]
    unsigned int *v3;  // [bp-0x1c], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0x18]
    unsigned int v5;  // [bp-0x14]
    unsigned int v6;  // [bp-0x10]
    unsigned int v7;  // [bp-0xc]
    st_484f4e_7 **v8;  // [bp-0x8], Other Possible Types: unsigned int
    int v9;  // [bp+0x4]
    unsigned int *v10;  // [bp+0x8]
    unsigned int v11;  // [bp+0xc]
    unsigned int v12;  // [bp+0x10]

    v8 = v13;
    v7 = v14;
    v16 = v15;
    v6 = v17;
    v5 = v18;
    if (v16 <= 89)
    {
        if (v16 == 89)
        {
            if (*((int *)(idx + 20)) < -0x76c)
            {
                *((unsigned int *)sub_46e96f()) = 22;
                sub_470f2d(0, 0, 0, 0, 0);
                return;
            }
            if (*((int *)(idx + 20)) <= 8099)
            {
                v4 = 100;
                v4 = v12;
                v3 = 4;
                sub_484ed3();
                return;
            }
            goto LABEL_485103;
        }
        if (v16 <= 73)
        {
            if (v16 != 73)
            {
                v19 = v16 - 4;
                if (v16 == 4)
                    return;
                v20 = v19 - 9;
                if (v19 == 9)
                    return;
                v21 = v20 - 24;
                if (v20 == 24)
                {
                    *(v13)->field_0 = 37;
                    *(v13) = *(v13) + 1;
                    *(v10) = *(v10) - 1;
                    return;
                }
                v22 = v21 - 28;
                if (v21 != 28)
                {
                    if (v22 != 1)
                    {
                        if (v22 != 7)
                            return;
                        if (*((int *)(idx + 8)) >= 0 && *((int *)(idx + 8)) <= 23)
                            goto LABEL_48525e;
                    }
                    else
                    {
                        if (*((int *)(idx + 16)) >= 0 && *((int *)(idx + 16)) <= 11)
                        {
                            sub_484e7d();
                            return;
                        }
                    }
                }
                else
                {
                    if (*((int *)(idx + 24)) >= 0 && *((int *)(idx + 24)) <= 6)
                    {
                        sub_484e7d();
                        return;
                    }
                }
            }
            else
            {
                v24 = *((int *)(idx + 8));
                if (v24 >= 0 && v24 <= 23)
                {
                    v4 = 12;
                    if (!(v24 % 12))
                        goto LABEL_48525e;
                }
            }
        }
        else
        {
            v25 = v16 - 77;
            if (v16 != 77)
            {
                v4 = 6;
                v26 = 6;
                if (v25 == 6)
                {
                    v31 = *((int *)idx);
                    goto LABEL_4850db;
                }
                v27 = v25 - 7;
                if (v27 != 1)
                {
                    v28 = v27 - 2;
                    if (v28 != 1)
                    {
                        if (v28 == 2)
                        {
                            v4 = v11;
                            v3 = v10;
                            goto LABEL_4851e2;
                        }
                    }
                    else
                    {
                        v29 = *((int *)(idx + 24));
                        if (v29 >= 0 && v29 <= 6 && v29)
                            v26 = v29 - 1;
                    }
                }
                else
                {
                    v26 = *((int *)(idx + 24));
                    if (v26 >= 0 && v26 <= 6 && (v30 = (unsigned int)*((int *)(idx + 28)), v30 >= 0 && v30 <= 365))
                    {
                        if (v30 < v26)
                        {
                            goto LABEL_48525e;
                        }
                        else
                        {
                            v4 = 7;
                            if (v30 % 7 >= v26)
                                goto LABEL_48525e;
                        }
                    }
                }
            }
            else
            {
                v31 = *((int *)(idx + 4));
LABEL_4850db:
                if (v31 >= 0 && v31 <= 59)
                    goto LABEL_48525e;
            }
        }
    }
    else if (v16 > 109)
    {
        v37 = v16 - 112;
        if (v16 != 112)
        {
            v38 = v37 - 7;
            if (v37 != 7)
            {
                v39 = v38 - 1;
                if (v38 == 1)
                {
                    v4 = v11;
                    v3 = v10;
                    v2 = v13;
                    v1 = idx;
                    if (v12)
                        v0 = 1;
                    else
                        v0 = 0;
LABEL_4851e6:
                    if (sub_485336(v9))
                        return;
                    return;
                }
                if (v39 != 1)
                {
                    if (v39 == 2)
                    {
LABEL_48527d:
                        sub_476f80();
                        sub_477413();
                        v8 = *((int *)(idx + 32));
                        sub_484e7d();
                        return;
                    }
                    return;
                }
                else
                {
                    if (*((int *)(idx + 20)) >= 0)
                    {
                        v4 = v12;
                        v3 = 100;
                        v3 = 2;
                        sub_484ed3();
                        return;
                    }
                }
            }
            else
            {
                if (*((int *)(idx + 24)) >= 0 && *((int *)(idx + 24)) <= 6)
                {
                    v4 = v12;
                    sub_484ed3();
                    return;
                }
            }
        }
        else
        {
            v40 = *((int *)(idx + 8));
            if (v40 >= 0 && v40 <= 23)
            {
                if (v40 <= 11)
                {
                    sub_484e7d();
                    return;
                }
                sub_484e7d();
                return;
            }
        }
    }
    else if (v16 != 109)
    {
        v32 = v16 - 90;
        if (v16 == 90)
            goto LABEL_48527d;
        v33 = v32 - 7;
        if (v32 != 7)
        {
            v34 = v33 - 1;
            if (v33 != 1)
            {
                v35 = v34 - 1;
                if (v34 == 1)
                {
                    v4 = v11;
                    v3 = v10;
                    v2 = v13;
                    v1 = idx;
                    if (v12)
                        v0 = 1;
                    else
                        v0 = 0;
                    if (sub_485336(v9) && *(v10))
                    {
                        v4 = v11;
                        *(v13)->field_0 = 32;
                        *(v13) = *(v13) + 1;
                        *(v10) = *(v10) - 1;
                        v3 = v10;
LABEL_4851e2:
                        v2 = v13;
                        v1 = idx;
                        v0 = 2;
                        goto LABEL_4851e6;
                    }
                }
                if (v35 != 1)
                {
                    if (v35 == 7)
                    {
                        if (*((int *)(idx + 28)) >= 0 && *((int *)(idx + 28)) <= 365)
                        {
                            v4 = v12;
                            v3 = 3;
                            sub_484ed3();
                            return;
                        }
                    }
                    else
                    {
                        return;
                    }
                }
                else
                {
                    if (*((int *)(idx + 12)) < 1 || *((int *)(idx + 12)) > 31)
                    {
LABEL_485103:
                        *((unsigned int *)sub_46e96f()) = 22;
                        sub_470f2d(0, 0, 0, 0, 0);
                        return;
                    }
                    goto LABEL_48525e;
                    goto LABEL_48525e;
                }
            }
            else
            {
                if (*((int *)(idx + 16)) >= 0 && *((int *)(idx + 16)) <= 11)
                {
                    sub_484e7d();
                    return;
                }
            }
        }
        else
        {
            if (*((int *)(idx + 24)) >= 0 && *((int *)(idx + 24)) <= 6)
            {
                sub_484e7d();
                return;
            }
        }
    }
    else if (*((int *)(idx + 16)) >= 0 && *((int *)(idx + 16)) <= 11)
    {
LABEL_48525e:
        v4 = v12;
        v3 = 2;
        sub_484ed3();
        return;
    }
    v36 = sub_46e96f();
    *(v36) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    return;
}
