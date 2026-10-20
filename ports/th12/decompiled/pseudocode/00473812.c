// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x473812; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct CPINFO {
    unsigned int MaxCharSize;
    Byte DefaultChar[2];
    Byte LeadByte[12];
} CPINFO;

typedef struct st_473812_1 {
    char padding_0[285];
    char field_11d;
} st_473812_1;

typedef struct st_473812_0 {
    char field_0;
    char padding_1[6];
    char field_7;
    char field_8;
} st_473812_0;

void sub_473812(void)
{
    unsigned int v7;  // edi
    void* v8;  // esi
    char v17;  // cl
    unsigned int v18;  // ecx
    st_473812_1 *v19;  // eax
    unsigned int v20;  // edx
    char v21;  // dl
    unsigned int i;  // eax
    char v10;  // al
    st_473812_0 *v11;  // ebx
    unsigned int v12;  // ecx
    unsigned int v13;  // eax
    st_473812_0 *v14;  // ebx
    unsigned int v15;  // eax
    char v16;  // cl
    unsigned int v0;  // [bp-0x528]
    unsigned int v1;  // [bp-0x520]
    CPINFO v2;  // [bp-0x51c]
    char v3;  // [bp-0x508]
    char v4;  // [bp-0x308]
    char v5;  // [bp-0x208]
    char v22;  // [bp-0x108], Other Possible Types: unsigned short

    v0 = v7;
    if (GetCPInfo((int)v8[4], &v2))
    {
        i = 0;
        do
        {
            *((char *)&v22 + i) = i;
            i += 1;
        } while (i < 0x100);
        v10 = *(&v2.LeadByte[0]);
        v22 = 32;
        if (*(&v2.LeadByte[0]))
        {
            v11 = &v2.LeadByte[1];
            do
            {
                v12 = v10;
                v13 = v11->field_0;
                if (v12 <= v13)
                    sub_477420(&(&v22)[v12], 32, v13 - v12 + 1);
            } while ((v14 = v11 + 1, v10 = v14->field_0, v11 = v14 + 1, v14->field_0));
        }
        sub_48384c(0, 1, &v22, 0x100, &v3, (int)v8[4], (int)v8[12], 0);
        sub_48364d(0, (int)v8[12], 0x100, &v22, 0x100, &v5, 0x100, (int)v8[4], 0);
        sub_48364d(0, (int)v8[12], 0x200, &v22, 0x100, &v4, 0x100, (int)v8[4], 0);
        v15 = 0;
        do
        {
            v16 = *((short *)&(&v3)[2 * v15]);
            if (v16 & 1)
            {
                *((char *)v8 + v15 + 29) = *((char *)v8 + v15 + 29) | 16;
                v17 = (&v5)[v15];
                goto LABEL_473929;
            }
            else if (v16 & 2)
            {
                *((char *)v8 + v15 + 29) = *((char *)v8 + v15 + 29) | 32;
                v17 = (&v4)[v15];
LABEL_473929:
                *((char *)v8 + v15 + 285) = v17;
            }
            else
            {
                *((char *)v8 + v15 + 285) = 0;
            }
        } while ((v15 += 1, v15 < 0x100));
        return;
    }
    else
    {
        v1 = 0xffffff9f;
        v18 = 0;
        v1 -= v8 + 285;
        do
        {
            v19 = v8 + v18 + 285;
            v20 = &v19->padding_0[v1];
            if (v20 + 32 <= 25)
            {
                *((char *)v8 + v18 + 29) = *((char *)v8 + v18 + 29) | 16;
                v21 = (char)v18 + 32;
                goto LABEL_47398b;
            }
            else if (v20 <= 25)
            {
                *((char *)v8 + v18 + 29) = *((char *)v8 + v18 + 29) | 32;
                v21 = (char)v18 - 32;
LABEL_47398b:
                v19->padding_0[0] = v21;
            }
            else
            {
                v19->padding_0[0] = 0;
            }
        } while ((v18 += 1, v18 < 0x100));
        return;
    }
}
