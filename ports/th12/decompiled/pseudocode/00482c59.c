// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x482c59; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_482c59_1 {
    unsigned int field_0;
} st_482c59_1;

typedef struct st_482c59_0 {
    char padding_0[92];
    struct st_482c59_1 *field_5c;
    unsigned int field_60;
    unsigned int field_64;
} st_482c59_0;

extern char g_4aaf58;
extern unsigned int g_4adb90;
extern unsigned int g_4adb94;
extern st_482c59_1 *g_4b4368;
extern st_482c59_1 *g_4b436c;
extern st_482c59_1 *g_4b4370;
extern st_482c59_1 *g_4b4374;

void sub_482c59(unsigned int a0)
{
    st_482c59_0 *idx;  // edi
    void* v3;  // ebp
    unsigned int v12;  // eax
    unsigned int v4;  // eax
    unsigned int v5;  // eax
    unsigned int v6;  // eax
    unsigned int v7;  // eax
    st_482c59_1 **v8;  // esi
    unsigned int v9;  // eax
    int v10;  // eax
    unsigned int v11;  // eax
    unsigned int *v0;  // [bp-0xc]
    unsigned int v1;  // [bp+0x0]

    sub_46fcec(&g_4aaf58, 32);
    idx = NULL;
    *((unsigned int *)((char *)v3 - 28)) = 0;
    *((st_482c59_0 **)((char *)v3 - 40)) = NULL;
    if ((int)v3[8] > 11)
    {
        v4 = (int)v3[8];
        v11 = v10 - 15;
        if (v10 != 15)
        {
            if (v11 != 6)
            {
                if (v11 == 7)
                    goto LABEL_482cec;
LABEL_482cd0:
                *((unsigned int *)sub_46e96f()) = 22;
                v0 = NULL;
                sub_470f2d(0, 0, 0, 0, 0);
                sub_46fd31(v0, &g_4aaf58, 32, v1);
                return;
            }
            v8 = &g_4b436c;
            v0 = &g_4b436c->field_0;
        }
        else
        {
            v8 = &g_4b4374;
            v0 = &g_4b4374->field_0;
        }
    }
    else if (v4 != 11)
    {
        v5 = v4;
        v0 = 0x2;
        v6 = v5 - 2;
        if (v5 != 2)
        {
            v7 = v6 - 2;
            if (v6 == 2)
                goto LABEL_482c8c;
            if (v7 != 2)
            {
                if (v7 != 4)
                    goto LABEL_482cd0;
                goto LABEL_482c8c;
            }
            else
            {
LABEL_482cec:
                v8 = &g_4b4370;
                v0 = &g_4b4370->field_0;
            }
        }
        else
        {
            v8 = &g_4b4368;
            v0 = &g_4b4368->field_0;
        }
    }
    else
    {
LABEL_482c8c:
        idx = sub_4753ee();
        *((st_482c59_0 **)((char *)v3 - 40)) = idx;
        if (idx)
        {
            v0 = &idx->field_5c->field_0;
            v8 = sub_4829c6(idx->field_5c) + 8;
            v9 = *(v8);
            goto LABEL_482d1b;
        }
    }
    *((unsigned int *)((char *)v3 - 28)) = 1;
    v9 = sub_4751de(v0);
LABEL_482d1b:
    *((unsigned int *)((char *)v3 - 32)) = v9;
    v12 = 0;
    if (*((int *)((char *)v3 - 32)) == 0x1)
    {
        sub_46fd31(v0, &g_4aaf58, 32, v1);
        return;
    }
    if (!*((int *)((char *)v3 - 32)))
    {
        v0 = 0x3;
        v12 = sub_472f0b(3);
    }
    if (*((int *)((char *)v3 - 28)) != v12)
        sub_46ec9a(v12);
    *((unsigned int *)((char *)v3 - 4)) = 0;
    if (v4 == 8 || v4 == 11 || v4 == 4)
    {
        *((unsigned int *)((char *)v3 - 44)) = idx->field_60;
        idx->field_60 = 0;
        if (v4 != 8)
            goto LABEL_482da5;
        *((unsigned int *)((char *)v3 - 48)) = idx->field_64;
        idx->field_64 = 140;
    }
    if (v4 == 8)
    {
        for (*((unsigned int *)((char *)v3 - 36)) = g_4adb90; *((int *)((char *)v3 - 36)) < g_4adb94 + g_4adb90; *((unsigned int *)((char *)v3 - 36)) = *((int *)((char *)v3 - 36)) + 1)
        {
            idx->field_5c[2 + 3 * *((int *)((char *)v3 - 36))].field_0 = 0;
        }
    }
    else
    {
LABEL_482da5:
        *(v8) = sub_4751d5();
    }
    *((unsigned int *)((char *)v3 - 4)) = 0xfffffffe;
    sub_482dcd();
    if (v4 == 8)
        (*((int *)((char *)v3 - 32)))(8, idx->field_64);
    else
        (*((int *)((char *)v3 - 32)))(v4);
    if (v4 == 8 || v4 == 11 || v4 == 4)
    {
        idx->field_60 = *((int *)((char *)v3 - 44));
        if (v4 == 8)
            idx->field_64 = *((int *)((char *)v3 - 48));
    }
    sub_46fd31(v0, &g_4aaf58, 32, v1);
    return;
}
