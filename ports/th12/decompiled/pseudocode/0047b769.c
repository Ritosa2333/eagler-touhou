// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47b769; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47b769_0 {
    char padding_0[20];
    unsigned int field_14;
} st_47b769_0;

typedef struct st_47b769_3 {
    HANDLE field_0;
    char field_4;
} st_47b769_3;

typedef struct st_47b769_2 {
    char padding_0[52];
    char field_34;
    char padding_35[3];
    unsigned int field_38;
} st_47b769_2;

typedef struct st_47b769_1 {
    char padding_0[108];
    struct st_47b769_0 *field_6c;
} st_47b769_1;

extern unsigned int g_4ad138;
extern unsigned int g_4d6320[4];

int sub_47b769(int a0, void* a1, void* a2)
{
    Byte v19[4];  // edi
    Byte v20[4];  // edi, Other Possible Types: unsigned int
    void* iter;  // ebx
    st_47b769_2 *v30;  // eax
    unsigned int v31;  // esi
    void* v32;  // ebx
    st_47b769_3 *v33;  // eax
    void* v34;  // ecx
    void* iter1;  // eax
    char v36;  // dl
    void* v37;  // ebx
    unsigned int v21;  // esi
    void* v38;  // ecx
    void* iter2;  // eax
    unsigned short v40;  // dx
    void* v41;  // ebx
    void* v42;  // ecx
    unsigned short *v43;  // eax
    unsigned short v44;  // dx
    int v45;  // esi
    int v46;  // ebx
    unsigned int v22;  // ebx
    unsigned int v47;  // eax
    unsigned int *v23;  // esi
    unsigned int v24;  // edi
    char *v25;  // eax
    char v26;  // 4175
    char v27;  // bl
    st_47b769_1 *v28;  // eax
    enum CONSOLE_MODE ch;  // [bp-0x1ae8], Other Possible Types: unsigned int
    unsigned int v1;  // [bp-0x1ae4]
    char v2;  // [bp-0x1add]
    unsigned int *v3;  // [bp-0x1adc]
    void* v4;  // [bp-0x1ad8]
    unsigned int v5;  // [bp-0x1ad4]
    void* v6;  // [bp-0x1acc]
    void* node;  // [bp-0x1ac8], Other Possible Types: unsigned int
    unsigned int v8;  // [bp-0x1ac4]
    void* i;  // [bp-0x1ac0], Other Possible Types: unsigned int
    Byte v10[1704];  // [bp-0x1abc]
    Byte v11[3416];  // [bp-0x1414]
    Char v12[1700];  // [bp-0x6bc]
    char *v13;  // [bp-0x18], Other Possible Types: void*
    unsigned int v14;  // [bp-0x14]
    unsigned int v15;  // [bp-0xc]
    unsigned int v16;  // [bp-0x8]
    char v17;  // [bp-0x4]
    char v18;  // [bp+0x0]

    v20 = v19;
    sub_48d060(&v18);
    v16 = g_4ad138 ^ &v17;
    v16 = v21;
    v6 = NULL;
    v5 = 0;
    if (a2)
    {
        if (!a1)
        {
            *((unsigned int *)sub_46e982()) = 0;
            *((unsigned int *)sub_46e96f()) = 22;
            sub_470f2d(0, 0, 0, 0, 0);
        }
        else
        {
            v15 = v22;
            v20 = v19;
            v23 = &g_4d6320[a0 >> 5];
            v24 = (a0 & 31) * 64;
            v25 = *(v23) + v24;
            v26 = v25[36];
            v27 = (char)(v26 * 2) >> 1;
            v3 = v23;
            v2 = (char)(v26 * 2) >> 1;
            if (((char)(v26 * 2) >> 1 == 2 || v27 == 1) && !(*((char *)&~(a2)) & 1))
            {
                *((unsigned int *)sub_46e982()) = 0;
                *((unsigned int *)sub_46e96f()) = 22;
                sub_470f2d(0, 0, 0, 0, 0);
                goto LABEL_47be7c;
            }
            if (v25[4] & 32)
                sub_47b5cb(a0, 0, 0, 2);
            if (sub_47bfc1(a0) && *((char *)(v24 + *(v23) + 4)) & 128 && !(v28 = (st_47b769_1 *)sub_475467(), v1 = (unsigned int)(char)(!*((int *)(*((int *)(sub_475467() + 108)) + 20))), !GetConsoleMode(*((int *)(v24 + *(v23))), &ch)))
            {
                if (v1 && !v27)
                    goto LABEL_47bafe;
                iter = a1;
                ch = GetConsoleCP();
                node = 0;
                if (a2 <= NULL)
                    goto LABEL_47be15;
                i = NULL;
                while (1)
                {
                    if (!v2)
                    {
                        v1 = *((char *)iter) == 10;
                        v30 = *(v3) + v24;
                        if (v30->field_38)
                        {
                            *((char *)&v20) = v30->field_34;
                            v20[1] = *((char *)iter);
                            v30->field_38 = 0;
                            v14 = 2;
                            v13 = &v20[0];
                        }
                        else if (!sub_47c5db(*((char *)iter)))
                        {
                            v14 = 1;
                            v13 = iter;
                        }
                        else if (a1 - iter + a2 > 1)
                        {
                            if (sub_48c27e(&v8, iter, 2) == 0xffffffff)
                                goto LABEL_47be0c;
                            iter += 1;
                            i += 1;
                            goto LABEL_47b97f;
                        }
                        else
                        {
                            v6 += 1;
                            *((char *)(v24 + *(v3) + 52)) = *((char *)iter);
                            *((unsigned int *)(v24 + *(v3) + 56)) = 1;
                            goto LABEL_47be0c;
                        }
                        if (sub_48c27e(&v8) == 0xffffffff)
                            goto LABEL_47be0c;
LABEL_47b97f:
                        iter += 1;
                        i += 1;
                        v31 = WideCharToMultiByte(ch, 0, &v8, 1, v20, 5, NULL, NULL);
                        if (!v31)
                            goto LABEL_47be0c;
                        if (!WriteFile(*((int *)(v24 + *(v3))), v20, v31, &node, NULL))
                            goto LABEL_47be00;
                        v6 = i + v5;
                        if (node < v31)
                            goto LABEL_47be0c;
                        if (!v1)
                            goto LABEL_47bad1;
                        *(v20) = 13;
                        if (!WriteFile(*((int *)(v24 + *(v3))), v20, 1, &node, NULL))
                            goto LABEL_47be00;
                        if (node < 1)
                            goto LABEL_47be0c;
                        v5 += 1;
                        v6 += 1;
                        goto LABEL_47bad1;
                    }
                    if (v2 == 1 || v2 == 2)
                    {
                        v32 = iter + 2;
                        i += 2;
                        v8 = *((short *)iter);
                        v1 = (unsigned short)v8 == 10;
                        iter = v32;
                    }
                    if (v2 == 1 || v2 == 2)
                        break;
LABEL_47bad1:
                    if (i >= a2)
                        goto LABEL_47be0c;
                }
                if ((unsigned short)sub_48ceb4(v8) == (unsigned short)v8)
                {
                    v6 += 2;
                    if (!v1)
                        goto LABEL_47bad1;
                    v14 = 13;
                    v8 = 13;
                    if ((unsigned short)sub_48ceb4(v8) != (unsigned short)v8)
                        goto LABEL_47be00;
                    v6 += 1;
                    v5 += 1;
                    goto LABEL_47bad1;
                }
            }
            else
            {
LABEL_47bafe:
                v33 = *(v23) + v24;
                if (v33->field_4 & 128)
                {
                    v8 = 0;
                    if (!v27)
                    {
                        node = a1;
                        if (a2 <= NULL)
                            goto LABEL_47be51;
                        while (1)
                        {
                            i = 0;
                            v34 = node - a1;
                            iter1 = v10;
                            do
                            {
                                if (v34 >= a2)
                                    break;
                                node += 1;
                                v36 = *((char *)node);
                                v34 += 1;
                                if (*((char *)node) == 10)
                                {
                                    v5 += 1;
                                    *((char *)iter1) = 13;
                                    iter1 += 1;
                                    i += 1;
                                }
                                *((char *)iter1) = v36;
                                iter1 += 1;
                                i += 1;
                            } while (i < 0x13ff);
                            v37 = iter1 - v10;
                            if (!WriteFile(*((int *)(v24 + *(v23))), v10, v37, &v4, NULL))
                                break;
                            v6 += v4;
                            if (v4 < v37 || node - a1 >= a2)
                                goto LABEL_47be0c;
                            v23 = v3;
                        }
                    }
                    else
                    {
                        i = node;
                        if (v27 == 2)
                        {
                            if (a2 <= NULL)
                                goto LABEL_47be51;
                            while (1)
                            {
                                node = 0;
                                v38 = i - a1;
                                iter2 = v10;
                                do
                                {
                                    if (v38 >= a2)
                                        break;
                                    i += 2;
                                    v40 = *((short *)i);
                                    v38 += 2;
                                    if (v40 == 10)
                                    {
                                        v5 += 2;
                                        v14 = 13;
                                        *((unsigned short *)iter2) = 13;
                                        iter2 += 2;
                                        node += 2;
                                    }
                                    node += 2;
                                    *((unsigned short *)iter2) = v40;
                                    iter2 += 2;
                                } while (node < 0x13fe);
                                v41 = iter2 - v10;
                                if (!WriteFile(*((int *)(v24 + *(v23))), v10, v41, &v4, NULL))
                                    break;
                                v6 += v4;
                                if (v4 < v41 || i - a1 >= a2)
                                    goto LABEL_47be0c;
                                v23 = v3;
                            }
                        }
                        else
                        {
                            if (a2 > NULL)
                            {
                                do
                                {
                                    node = 0;
                                    v42 = i - a1;
                                    v14 = 2;
                                    v43 = v12;
                                    do
                                    {
                                        if (v42 >= a2)
                                            break;
                                        v44 = *((short *)i);
                                        i += 2;
                                        v42 += 2;
                                        if (v44 == 10)
                                        {
                                            v14 = 13;
                                            *(v43) = 13;
                                            v43 += 1;
                                            node += 2;
                                        }
                                        node += 2;
                                        *(v43) = v44;
                                        v43 += 1;
                                    } while (node < 1704);
                                    v45 = 0;
                                    v46 = WideCharToMultiByte(65001, 0, v12, (v43 - v12) / 2, v11, 3413, NULL, NULL);
                                    if (!v46)
                                        goto LABEL_47be00;
                                    else
                                        goto LABEL_47bd69;
                                    do
                                    {
LABEL_47bd69:
                                        if (!WriteFile(*((int *)(v24 + *(v3))), &v11[v45], v46 - v45, &v4, NULL))
                                        {
                                            v8 = (unsigned int)GetLastError();
                                            break;
                                        }
                                    } while ((v45 = (int)(v45 + v4), v46 > v45));
                                } while (v46 <= v45 && (v6 = (void*)(i - a1), v6 < a2));
                                goto LABEL_47be0c;
                            }
                        }
                    }
                }
                else if (WriteFile(v33->field_0, a1, a2, &v4, NULL))
                {
                    v8 = 0;
                    v6 = v4;
                    goto LABEL_47be0c;
                }
            }
LABEL_47be00:
            v8 = (unsigned int)GetLastError();
LABEL_47be0c:
            if (!v6)
            {
LABEL_47be15:
                if (v8)
                {
                    v14 = 5;
                    if (v8 == 5)
                    {
                        *((unsigned int *)sub_46e96f()) = 9;
                        *((unsigned int *)sub_46e982()) = 5;
                        goto LABEL_47be7c;
                    }
                    else
                    {
                        sub_46e995(v8);
                        goto LABEL_47be7c;
                    }
                }
                else
                {
                    v23 = v3;
                }
LABEL_47be51:
                if (*((char *)(v24 + *(v23) + 4)) & 64 && *((char *)a1) == 26)
                {
                    goto LABEL_47be8d;
                }
                else
                {
                    *((unsigned int *)sub_46e96f()) = 28;
                    *((unsigned int *)sub_46e982()) = 0;
                }
LABEL_47be7c:
            }
LABEL_47be8d:
        }
    }
    _security_check_cookie();
    return v47;
}
