// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4822f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4822f0_1 {
    char padding_0[4];
    unsigned int field_4;
} st_4822f0_1;

typedef struct st_4822f0_0 {
    char field_0;
} st_4822f0_0;

extern st_4822f0_0 *g_4b4318;

unsigned int * sub_4822f0(unsigned int *a0, st_4822f0_1 *a1)
{
    char v14;  // al
    unsigned int v15;  // ebx
    unsigned int *v25;  // eax
    unsigned int *v26;  // eax
    unsigned int *v27;  // eax
    unsigned int v28;  // ecx
    unsigned int v29;  // eax
    unsigned int v30;  // eax
    unsigned int v31;  // eax
    unsigned int v32;  // eax
    unsigned int v33;  // eax
    unsigned int v16;  // esi
    unsigned int v34;  // eax
    unsigned int v35;  // eax
    unsigned int v36;  // eax
    unsigned int v37;  // eax
    char *v38;  // eax
    unsigned int *v39;  // eax
    unsigned int v17;  // edi
    unsigned int v18;  // edi
    unsigned int v19;  // eax
    unsigned int v20;  // ebx
    char *v21;  // eax
    char v22;  // 4106
    unsigned int v23;  // eax
    unsigned int v0;  // [bp-0x40]
    char *v1;  // [bp-0x3c], Other Possible Types: unsigned int
    int v2;  // [bp-0x38], Other Possible Types: unsigned int
    unsigned int v3;  // [bp-0x34]
    unsigned int v4;  // [bp-0x30]
    unsigned int v5;  // [bp-0x2c]
    char v6;  // [bp-0x28]
    char v7;  // [bp-0x20], Other Possible Types: unsigned int
    unsigned int v8;  // [bp-0x1c]
    char v9;  // [bp-0x18], Other Possible Types: unsigned int
    unsigned int v10;  // [bp-0x14]
    unsigned int v11;  // [bp-0x10]
    unsigned int v12;  // [bp-0xc]
    char v13;  // [bp-0x5]

    v14 = g_4b4318->field_0;
    v5 = v15;
    v4 = v16;
    v3 = v17;
    if (!g_4b4318->field_0)
    {
        sub_47ec26(a0, 1, a1);
LABEL_482679:
        return a0;
    }
    g_4b4318 = g_4b4318 + 1;
    v11 = 0;
    v18 = v14;
    v12 &= 0xffff0000;
    v19 = v18;
    v20 = 0xffffffff;
    v13 = 0;
    if (v19 <= 78)
    {
        if (v19 == 78)
            goto LABEL_482564;
        switch (v19)
        {
        case 67: case 68: case 69:
            v2 = "char";
            break;
        case 70: case 71:
            v2 = "short";
            break;
        case 72: case 73:
            v2 = "int";
            break;
        case 74: case 75:
            v2 = "long";
            break;
        case 77:
            v2 = "float";
            break;
        default:
            v21 = &v6;
            goto LABEL_48248f;
        }
    }
    else
    {
        if (v19 == 79)
        {
            sub_47e896("long ");
LABEL_482564:
            sub_47ea48("double");
LABEL_482571:
            if (v20 != 0xffffffff)
            {
LABEL_4824c6:
                v12 &= 0xffff0000;
                v9 = (unsigned int)a1->padding_0;
                v11 = 0;
                v10 = a1->field_4;
                if (v20 == 0xfffffffe)
                {
                    v10 |= 0x800;
                    sub_48206c(&v7, &v11, &v9, 0);
                    if (!(v8 & 0x800))
                        sub_47ea48("[]");
                    v27 = a0;
                    *(v27) = v7;
                    v27[1] = v8;
                    return v27;
                }
                if (!a1->padding_0)
                {
                    if ((char)v20 & 1)
                    {
                        sub_47e896("const");
                        if ((char)v20 & 2)
                            sub_47ea48(" volatile");
                    }
                    else if ((char)v20 & 2)
                    {
                        sub_47e896("volatile");
                    }
                }
                sub_482165(a0, &v11, &v9);
                goto LABEL_482679;
                goto LABEL_482679;
            }
LABEL_48257a:
            v29 = v18;
            v30 = v29 - 67;
            if (v29 != 67)
            {
                v1 = 2;
                v31 = v30 - 2;
                if (v30 != 2 && !(v32 = v31 - 2, v31 == 2 || (v33 = v32 - 2, v32 == 2 || v33 == 2)))
                {
                    if (v33 != 22 || !(v34 = (unsigned int)v13, v35 = v34 - 69, v34 == 69 || (v36 = v35 - 2, v35 == 2 || (v37 = v36 - 2, v36 == 2 || v36 == 4 || v36 == 6))))
                        goto LABEL_4825ee;
                    v1 = &v11;
                    v0 = "unsigned ";
                    v38 = &v6;
                }
                else
                {
                    v1 = &v11;
                    v0 = "unsigned ";
                    v38 = &v7;
                }
            }
            else
            {
                v1 = &v11;
                v0 = "signed ";
                v38 = &v9;
            }
            v39 = sub_47ec4a(v38);
            v11 = *(v39);
            v12 = v39[1];
LABEL_4825ee:
            if (a1->padding_0)
                sub_47e7bf(sub_47ec02(&v6, 32, a1));
            v27 = a0;
            *(v27) = v11;
            v27[1] = v28;
            return v27;
        }
        if (v19 <= 79)
        {
            v21 = &v6;
LABEL_48248f:
            g_4b4318 = (char *)g_4b4318 - 1;
            v25 = sub_480514(v21);
            v11 = *(v25);
            v12 = v25[1];
            if (!*(v25))
            {
                v26 = a0;
                *(v26) = 0;
                v26[1] = v12;
                return v26;
            }
            goto LABEL_48257a;
        }
        if (v19 <= 83)
        {
            v20 = v18 & 3;
            goto LABEL_482571;
        }
        if (v19 != 88)
        {
            if (v19 != 95)
            {
                v21 = &v6;
                goto LABEL_48248f;
            }
            v22 = g_4b4318->field_0;
            g_4b4318 = g_4b4318 + 1;
            v13 = v22;
            v23 = v22;
            if (v23 <= 75)
            {
                if (v23 >= 74)
                {
                    v2 = "__int64";
                }
                else if (v23 > 69)
                {
                    if (v23 < 70)
                        goto LABEL_482532;
                    if (v23 > 71)
                    {
                        if (v23 > 73)
                            goto LABEL_482532;
                        v2 = "__int32";
                    }
                    else
                    {
                        v2 = "__int16";
                    }
                }
                else if (v23 >= 0x44)
                {
                    v2 = "__int8";
                }
                else if (!v23)
                {
                    g_4b4318 = (char *)g_4b4318 - 1;
                    sub_47e2f7(1);
                    goto LABEL_48257a;
                }
                else if (v23 == 36)
                {
                    sub_47ec4a(a0, "__w64 ", sub_4822f0(&v7, a1));
                    goto LABEL_482679;
                }
            }
            else
            {
                if (v23 < 76)
                {
LABEL_482532:
                    v2 = "UNKNOWN";
                }
                else if (v23 <= 77)
                {
                    v2 = "__int128";
                }
                else if (v23 != 78)
                {
                    if (v23 == 79)
                    {
                        v2 = 0xfffffffe;
                        v20 = 0xfffffffe;
                        goto LABEL_4824c6;
                    }
                    if (v23 == 87)
                    {
                        v2 = "wchar_t";
                    }
                    else if (v23 - 88 <= 1)
                    {
                        v21 = &v9;
                        goto LABEL_48248f;
                    }
                }
                else
                {
                    v2 = "bool";
                }
            }
        }
        else
        {
            v2 = "void";
        }
    }
    sub_47e896(v2);
    goto LABEL_48257a;
}
