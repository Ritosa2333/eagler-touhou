// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x454185; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_454185_1 {
    struct st_454185_0 *field_0;
} st_454185_1;

typedef struct st_454185_0 {
    unsigned int field_0;
} st_454185_0;

typedef struct st_454185_3 {
    char padding_0[8];
    unsigned int field_8;
} st_454185_3;

extern unsigned int g_4cf4fc;
extern unsigned int g_4cf500;
extern st_454185_1 *g_4d4754;
extern HANDLE g_4d4758;

int sub_454185(void)
{
    st_454185_1 **count;  // ebx
    st_454185_3 *idx;  // ebp
    unsigned int v4;  // ecx
    st_454185_1 **v5;  // ecx
    unsigned int v0;  // [bp-0x4]

    if (g_4d4754 == count)
        goto LABEL_0x454282;
    v4 = idx->field_8;
    if (v4 == count)
    {
        v0 = 1;
    }
    else if (v4 == 0x1)
    {
        if (g_4cf500 == count)
            goto LABEL_0x454282;
        PostThreadMessageA(g_4cf4fc, 18, count, count);
    }
    else if (v4 != 0x2)
    {
        if (v4 != 0x3)
        {
            if (v4 != 0xa)
                goto LABEL_0x4543a1;
        }
        else
        {
            CloseHandle(g_4cf500);
            CloseHandle(g_4d4758);
            v5 = g_4d4754;
            g_4cf500 = count;
            if (v5 != count)
                *(v5)->field_0(1);
            g_4d4754 = count;
        }
    }
    else
    {
        if (WaitForSingleObject(g_4cf500, 0x100))
        {
            PostThreadMessageA(g_4cf4fc, 18, count, count);
            idx->field_8 = idx->field_8 - 1;
        }
        g_4cf500 = count;
    }
}
