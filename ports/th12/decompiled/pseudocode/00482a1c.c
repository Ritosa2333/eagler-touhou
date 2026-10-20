// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x482a1c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_482a1c_1 {
    char padding_0[92];
    int field_5c;
} st_482a1c_1;

typedef struct st_482a1c_0 {
    char padding_0[8];
    unsigned int field_8;
} st_482a1c_0;

extern int g_49d5c8;
extern char g_4aaf38;
extern int g_4adb98;
extern unsigned int g_4adb9c;
extern unsigned int g_4b4368;
extern unsigned int g_4b436c;
extern unsigned int g_4b4370;
extern unsigned int g_4b4374;
extern unsigned int g_4b4378;

void sub_482a1c(unsigned int a0)
{
    void* idx;  // ebp
    unsigned int v3;  // esi
    unsigned int v12;  // eax
    int v4;  // ebx
    st_482a1c_1 *v5;  // esi
    int v6;  // eax
    st_482a1c_0 *iter;  // eax
    enum WIN32_ERROR *v8;  // eax
    int v9;  // eax
    unsigned int v10;  // eax
    unsigned int v11;  // eax
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp+0x0]

    sub_46fcec(&g_4aaf38, 16);
    *((unsigned int *)((char *)idx - 32)) = 0;
    v3 = (int)idx[12];
    v4 = (int)idx[8];
    if (v3 != 4 && v3 != 3)
    {
        v0 = 2;
        if (!(v4 != 2 && v4 != 21 && v4 != 22 && v4 != 6 && v4 != 15))
        {
            sub_46ec9a(0);
            *((unsigned int *)((char *)idx - 4)) = 0;
            if ((v4 == 2 || v4 == 21) && !g_4b4378)
            {
                if (SetConsoleCtrlHandler(sub_48292b, 1) == 1)
                {
                    g_4b4378 = 1;
                }
                else
                {
                    v8 = sub_46e982();
                    *(v8) = GetLastError();
                    *((unsigned int *)((char *)idx - 32)) = 1;
                    v3 = (int)idx[12];
                }
            }
            v9 = v4;
            v10 = v9 - 2;
            if (v9 != 2)
            {
                v11 = v10 - 4;
                if (v10 != 4)
                {
                    v12 = v11 - 9;
                    if (v11 != 9)
                    {
                        if (v12 != 6)
                        {
                            if (v12 == 7)
                                goto LABEL_482b79;
                            goto LABEL_482bf8;
                        }
                        else
                        {
                            *((unsigned int *)((char *)idx - 28)) = sub_4751de(g_4b436c);
                            if (v3 != 2)
                            {
                                g_4b436c = sub_475163(v3);
LABEL_482bf7:
                                goto LABEL_482bf8;
                            }
                        }
                    }
                    *((unsigned int *)((char *)idx - 28)) = sub_4751de(g_4b4374);
                    if (v3 == 2)
                        goto LABEL_482bf8;
                    g_4b4374 = sub_475163(v3);
                    goto LABEL_482bf7;
                }
LABEL_482b79:
                *((unsigned int *)((char *)idx - 28)) = sub_4751de(g_4b4370);
                if (v3 == 2)
                    goto LABEL_482bf8;
                g_4b4370 = sub_475163(v3);
                goto LABEL_482bf7;
            }
            else
            {
                *((unsigned int *)((char *)idx - 28)) = sub_4751de(g_4b4368);
                if (v3 != 2)
                {
                    g_4b4368 = sub_475163(v3);
                    goto LABEL_482bf7;
                }
            }
LABEL_482bf8:
            *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
            sub_482c14();
            if (!*((int *)((char *)idx - 32)))
            {
LABEL_482c0a:
                sub_46fd31(&g_4aaf38, 16, v1, a0);
                return;
            }
        }
        else if ((v4 == 8 || v4 == 4 || v4 == 11) && (v5 = (st_482a1c_1 *)sub_4753ee(), v5))
        {
            if (v5->field_5c == &g_49d5c8)
            {
                v6 = sub_4777ce(g_4adb98);
                v5->field_5c = v6;
                if (v6)
                {
                    sub_47a330(v6, &g_49d5c8, g_4adb98);
                    goto LABEL_482ac6;
                }
            }
            else
            {
LABEL_482ac6:
                iter = sub_4829c6(v5->field_5c);
                if (iter)
                {
                    if ((int)idx[12] != 2)
                    {
                        do
                        {
                        } while (*((int *)&iter->padding_0[4]) == v4 && (iter->field_8 = (unsigned int)(int)idx[12], iter += 12, iter < g_4adb9c * 12 + v5->field_5c));
                    }
                    goto LABEL_482c0a;
                }
            }
        }
    }
    if (v4 != 1 && v4 != 3 && v4 != 13 && (v4 <= 15 || v4 > 0x11))
    {
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
    }
    sub_46fd31(&g_4aaf38, 16, v1, a0);
    return;
}
