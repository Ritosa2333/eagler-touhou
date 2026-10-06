// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40dd50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40dd50_1 {
    char padding_0[116];
    unsigned int field_74;
} st_40dd50_1;

typedef struct st_40dd50_0 {
    char padding_0[124];
    unsigned int field_7c;
    char padding_80[12];
    unsigned int field_8c;
    unsigned int field_90;
    unsigned int field_94;
    char padding_98[16];
    unsigned int field_a8;
} st_40dd50_0;

extern unsigned int g_4b0cb0;
extern st_40dd50_0 *g_4b43cc;
extern st_40dd50_1 *g_4b44e8;
extern unsigned int g_4b4518;

void sub_40dd50(void)
{
    unsigned int v6;  // ebx
    unsigned int v7;  // esi
    unsigned int v16;  // 4186
    int v17;  // edi
    int v18;  // eax
    unsigned int v19;  // ecx
    unsigned int v20;  // eax
    char v8;  // al
    unsigned int v9;  // edi
    unsigned short v10;  // ftop
    unsigned short v11;  // ftop
    unsigned short v12;  // ftop
    unsigned short v13;  // ftop
    unsigned short v14;  // ftop
    double v0;  // [bp-0x24], Other Possible Types: unsigned long
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    double v4;  // [bp-0xc], Other Possible Types: unsigned long

    v3 = v6;
    v2 = v7;
    v8 = g_4b43cc->field_7c;
    v1 = v9;
    if (v8 & 1)
    {
        if (!(v8 & 64))
        {
            sub_4508b0();
            if (/* unsupported instruction */)
            {
                *((long long *)&g_4b43cc->padding_98[0]) = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                *((unsigned long long *)&g_4b43cc->padding_98[0]) = nan;
                /* unsupported instruction */
            }
            g_4b43cc->field_7c = g_4b43cc->field_7c | 64;
            return;
        }
        else
        {
            return;
        }
    }
    else
    {
        if (v8 & 64)
        {
            g_4b43cc->field_94 = g_4b43cc->field_90;
            sub_4508b0();
            if (/* unsupported instruction */)
            {
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
            }
            if (/* unsupported instruction */)
                v4 = /* unsupported instruction */;
            else
                v4 = (double)nan;
            if (/* unsupported instruction */)
                *((long long *)&g_4b43cc->padding_98[8]) = /* unsupported instruction */;
            else
                *((unsigned long long *)&g_4b43cc->padding_98[8]) = nan;
            v11 = v10 - 1;
            if (/* unsupported instruction */)
            {
                v12 = v11 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v12 = v11 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            sub_4933ca();
            v13 = v12 - 1;
            if (/* unsupported instruction */)
            {
                v14 = v13 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v14 = v13 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            *((long long *)&g_4b43cc->padding_98[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v16 = _ccall(10, 13, (char)(((v14 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
            if (!(v16 & 1))
            {
                if (/* unsupported instruction */)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                if (/* unsupported instruction */)
                {
                    *((long long *)&g_4b43cc->padding_98[8]) = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    *((unsigned long long *)&g_4b43cc->padding_98[8]) = nan;
                    /* unsupported instruction */
                }
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            if (/* unsupported instruction */)
            {
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
            }
            if (/* unsupported instruction */)
            {
                v0 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                v0 = (double)nan;
                /* unsupported instruction */
            }
            sub_493290();
            /* unsupported instruction */
            /* unsupported instruction */
            v17 = sub_4931e0();
            if (v17 >= 1000)
                v17 = 999;
            if (/* unsupported instruction */)
            {
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
            }
            if (/* unsupported instruction */)
            {
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v18 = sub_4931e0();
            /* unsupported instruction */
            /* unsupported instruction */
            *((long long *)&g_4b43cc->padding_98[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            g_4b43cc->field_7c = g_4b43cc->field_7c & 0xffffffbf;
            v19 = g_4b43cc->field_8c;
            v20 = ((v18 + v17 + 22) * 1000 + (int)((v17 + 66) % 1000)) * 100 + (int)((v18 + 33) % 100);
            g_4b43cc->field_a8 = v20;
            if (!g_4b44e8->field_74)
            {
                *((unsigned int *)(*((int *)(g_4b4518 + g_4b0cb0 * 4 + 32)) + v19 * 4 + 76)) = v20;
                g_4b43cc->field_8c = g_4b43cc->field_8c + 1;
                return;
            }
            g_4b43cc->field_a8 = *((int *)(*((int *)(g_4b4518 + g_4b0cb0 * 36 + 184)) + v19 * 4 + 76));
            if (sub_40d710())
            {
                sub_40d770(g_4b43cc);
                g_4b43cc->field_8c = g_4b43cc->field_8c + 1;
            }
            else
            {
                g_4b43cc->field_8c = g_4b43cc->field_8c + 1;
            }
            return;
        }
        else
        {
            return;
        }
    }
}
