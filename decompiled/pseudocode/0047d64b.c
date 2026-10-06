// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47d64b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47d64b_0 {
    char field_0;
    char field_1;
    char field_2;
    char field_3;
} st_47d64b_0;

extern st_47d64b_0 *g_4b4318;

unsigned int sub_47d64b(void)
{
    st_47d64b_0 *iter;  // ecx
    unsigned int v2;  // esi
    unsigned int v11;  // 4101
    unsigned int v12;  // eax
    unsigned int v13;  // esi
    unsigned int v14;  // esi
    unsigned int v15;  // esi
    unsigned int v16;  // ecx
    unsigned int v17;  // ecx
    unsigned int v18;  // eax
    unsigned int v19;  // eax
    unsigned int v20;  // eax
    char v3;  // bl
    unsigned int v22;  // eax
    unsigned int v23;  // eax
    unsigned int v24;  // esi
    unsigned int v25;  // eax
    unsigned int v26;  // esi
    unsigned int v27;  // esi
    unsigned int v28;  // esi
    unsigned int v29;  // eax
    unsigned int v30;  // eax
    st_47d64b_0 *node;  // ecx
    char v31;  // al
    unsigned int v32;  // esi
    unsigned int v33;  // esi
    unsigned int v34;  // esi
    unsigned int v35;  // esi
    unsigned int v5;  // eax
    char *v6;  // eax
    char *v7;  // ecx
    unsigned int v8;  // eax
    unsigned int v9;  // eax
    unsigned int v10;  // 4112

    iter = g_4b4318;
    while (1)
    {
        v2 = 0;
        if (iter->field_0 == 95)
        {
            iter = &iter->field_1;
            v2 = 0x4000;
            g_4b4318 = iter;
        }
        if (iter->field_0 >= 65 && iter->field_0 <= 90)
        {
            v12 = iter->field_0 - 65;
            v13 = v2 | 0x8000;
            g_4b4318 = &iter->field_1;
            v14 = (!((char)v12 & 1) ? v13 & 0xffffdfff : v13 | 0x2000);
            if (v12 < 24)
            {
                v15 = (!(v14 & 0x8000) ? v14 & 0xffff9fff : v14 & 0xffffefff | 0x800);
                v16 = v12;
                v17 = v16 & 24;
                if (!(v16 & 24))
                {
                    v14 = (!(v15 & 0x8000) ? v15 & 0xffffefff | 0x800 : v15 & 0xffffff7f | 64);
                }
                else if (v17 != 8)
                {
                    if (v17 != 16)
                    {
LABEL_47d75a:
                        return 0xffff;
                    }
                    v14 = (!(v15 & 0x8000) ? v15 & 0xffffe7ff : v15 & 0xffffff3f);
                }
                else
                {
                    v14 = (!(v15 & 0x8000) ? v15 & 0xfffff7ff | 0x1000 : v15 & 0xffffffbf | 128);
                }
                v18 = v12 & 6;
                if (v18)
                {
                    v19 = v18 - 1;
                    if (v19 != 1)
                    {
                        v20 = v19 - 2;
                        if (v20 != 1)
                        {
                            if (v20 != 3)
                            {
LABEL_47d75a:
                                return 0xffff;
                            }
                            v14 = v14 & 0xfffffcff | 0x400;
                            break;
                        }
                        else
                        {
                            v14 = v14 & 0xfffff9ff | 0x100;
                            break;
                        }
                    }
                    else
                    {
                        v14 = (!(v14 & 0x8000) ? v14 & 0xffff9fff : v14 & 0xfffffaff | 0x200);
                        break;
                    }
                }
            }
            break;
        }
        if (iter->field_0 != 36)
        {
            v31 = iter->field_0;
            if (iter->field_0 >= 48 && v31 <= 56)
            {
                v32 = v2 & 0xffff7fff;
                g_4b4318 = &iter->field_1;
                switch (v31)
                {
                case 48:
                    v33 = (!(v32 & 0x8000) ? v32 & 0xffff9fff : v32 & 0xfffffaff | 0x200);
                    v14 = (!(v33 & 0x8000) ? v33 & 0xffffefff | 0x800 : v33 & 0xffffff7f | 64);
                    break;
                case 49:
                    v34 = (!(v32 & 0x8000) ? v32 & 0xffff9fff : v32 & 0xfffffaff | 0x200);
                    v14 = (!(v34 & 0x8000) ? v34 & 0xfffff7ff | 0x1000 : v34 & 0xffffffbf | 128);
                    break;
                case 50:
                    v35 = (!(v32 & 0x8000) ? v32 & 0xffff9fff : v32 & 0xfffffaff | 0x200);
                    v14 = (!(v35 & 0x8000) ? v35 & 0xffffe7ff : v35 & 0xffffff3f);
                    break;
                case 51:
                    v14 = v32 & 0xffffdfff | 0x4000;
                    break;
                case 52:
                    v14 = v32 & 0xffffbfff | 0x2000;
                    break;
                case 53:
                    v14 = v32 & 0xffffe3ff | 0x6000;
                    break;
                case 54:
                    v14 = v32 & 0xffffebff | 0x6800;
                    break;
                case 55:
                    v14 = v32 & 0xfffff3ff | 0x7000;
                    break;
                case 56:
                    v14 = v32 & 0xfffffbff | 0x7800;
                    break;
                default:
LABEL_47d75a:
                    return 0xffff;
                }
            }
            else if (v31 == 57)
            {
                g_4b4318 = &iter->field_1;
                v14 = 0xfffd;
                break;
            }
            else
            {
                v14 = (unsigned int)((v31) + 0xfffe);
                break;
            }
        }
        v3 = 0;
        node = &iter->field_1;
        g_4b4318 = node;
        v5 = node->field_0;
        if (v5 > 66)
        {
            v22 = v5 - 67;
            if (v5 != 67)
            {
                v23 = v22 - 1;
                if (v22 == 1)
                {
                    v14 = v2 & 0xfffff5ff | 0x9100;
                    goto LABEL_47d9a9;
                }
                else if (v23 == 1)
                {
                    v14 = v2 & 0xfffff6ff | 0x9200;
                    goto LABEL_47d9a9;
                }
                else if (v23 != 14)
                {
LABEL_47d75a:
                    return 0xffff;
                }
                else
                {
                    node = &node->field_1;
                    g_4b4318 = node;
                    v3 = 1;
                    if (node->field_0 < 48 || node->field_0 > 53)
                        return (unsigned int)((!node->field_0) + 0xfffe);
                }
            }
            else
            {
                v14 = v2 | 0x7c00;
LABEL_47d9a9:
                g_4b4318 = &node->field_1;
                break;
            }
            v24 = v2 | 0x8000;
            v25 = node->field_0 - 48;
            v26 = (!(v24 & 0x8000) ? v24 & 0xffff9fff : v24 & 0xffffefff | 0x800);
            v27 = (!v3 ? v26 & 0xfffffdff | 0x500 : v26 & 0xfffffeff | 0x600);
            v28 = (!((char)v25 & 1) ? v27 & 0xffffdfff : v27 | 0x2000);
            v29 = v25 & 6;
            if (v29)
            {
                v30 = v29 - 1;
                if (v30 != 1)
                {
                    if (v30 != 3)
                    {
LABEL_47d75a:
                        return 0xffff;
                    }
                    v14 = (!(v28 & 0x8000) ? v28 & 0xffffe7ff : v28 & 0xffffff3f);
                    goto LABEL_47d9a9;
                }
                else
                {
                    v14 = (!(v28 & 0x8000) ? v28 & 0xfffff7ff | 0x1000 : v28 & 0xffffffbf | 128);
                    goto LABEL_47d9a9;
                }
            }
            else
            {
                v14 = (!(v28 & 0x8000) ? v28 & 0xffffefff | 0x800 : v28 & 0xffffff7f | 64);
                goto LABEL_47d9a9;
            }
            goto LABEL_47d9a9;
        }
        if (v5 == 66)
        {
            v14 = v2 | 0x9800;
            goto LABEL_47d9a9;
        }
        if (!v5)
        {
            v14 = 0xfffe;
            node = (char *)node - 1;
            goto LABEL_47d9a9;
        }
        if (v5 != 36)
        {
            if (v5 <= 47)
            {
LABEL_47d75a:
                return 0xffff;
            }
            if (v5 > 53)
            {
                if (v5 != 65)
                {
LABEL_47d75a:
                    return 0xffff;
                }
                v14 = v2 & 0xfffff4ff | 0x9000;
                goto LABEL_47d9a9;
                goto LABEL_47d9a9;
            }
        }
        v6 = &node->field_1;
        if (*(v6) == 80)
            node = v6;
        v7 = &node->field_1;
        g_4b4318 = v7;
        v8 = *(v7);
        if (v8 <= 74)
        {
            if (v8 == 74)
                goto LABEL_47d838;
            v9 = v8;
            if (!v8)
                return 0xfffe;
            if (v9 != 70 && !(v10 = (unsigned int)_ccall(6, v9, 70, 0), v11 = (unsigned int)_ccall(4, 21, v9 - 72, 0, v10), v11 & 1))
                goto LABEL_47d75a;
        }
        else
        {
            if (v8 < 76)
            {
LABEL_47d75a:
                return 0xffff;
            }
            if (v8 > 77)
            {
                if (v8 <= 79)
                {
LABEL_47d838:
                    node = v7 + 1;
                    g_4b4318 = node;
                    if (node->field_0 >= 48 && node->field_0 <= 57)
                    {
                        g_4b4318 = &(&node->field_0)[node->field_0] - 47;
                        return sub_47d64b() | 0x10000;
                    }
                    v14 = 0xffff;
                    goto LABEL_47d9a9;
                }
                if (v8 != 81)
                {
LABEL_47d75a:
                    return 0xffff;
                }
            }
        }
        iter = v7 + 1;
        g_4b4318 = iter;
    }
    return v14;
}
