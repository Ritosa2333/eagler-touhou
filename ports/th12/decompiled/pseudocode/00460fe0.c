// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x460fe0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_460fe0_2 {
    char padding_0[184];
    unsigned int field_b8;
} st_460fe0_2;

typedef struct st_460fe0_1 {
    unsigned int field_0;
} st_460fe0_1;

typedef struct st_460fe0_0 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1112];
    unsigned int field_47c;
    char padding_480[8];
    struct st_460fe0_1 *field_488;
} st_460fe0_0;

extern unsigned int g_4cee78;
extern char g_4cf1d0;
extern char g_4cf221;

unsigned int sub_460fe0(unsigned int a0)
{
    unsigned int v4;  // ebx
    unsigned int v5;  // esi
    int idx;  // eax
    st_460fe0_2 *idx1;  // edi
    st_460fe0_2 *iter;  // ecx
    st_460fe0_0 **v9;  // eax
    st_460fe0_0 *index;  // esi
    st_460fe0_0 **v11;  // ebx
    unsigned int v0;  // [bp-0x90]
    unsigned int v1;  // [bp-0x8c]
    unsigned int v2;  // [bp-0x88]
    char v3;  // [bp-0x84]

    v2 = v4;
    v1 = v5;
    if (g_4cee78 & 0x8000)
    {
        v0 = &g_4cf1d0;
        EnterCriticalSection(&g_4cf1d0);
        g_4cf221 = g_4cf221 + 1;
    }
    idx = 0;
    iter = &idx1[47527].padding_0[36];
    do
    {
        *((st_460fe0_2 **)&(&v3)[4 * idx]) = iter;
        *((unsigned int *)&iter->padding_0[28]) = 0;
        idx += 1;
        iter = &iter[6].padding_0[76];
    } while (idx < 30);
    v9 = *((int *)&idx1[47527].padding_0[20]);
    if (v9)
    {
        do
        {
            index = *(v9);
            v11 = v9[1];
            if (index->field_47c & 0x10000000)
            {
                sub_461450(idx1);
            }
            else if (index->field_488 && index->field_488())
            {
                sub_461450(idx1);
            }
            else if (sub_455630(index))
            {
                sub_461450(idx1);
            }
            else
            {
                *((st_460fe0_0 **)((&v0)[index->field_20] + 28)) = index;
                (&v0)[index->field_20] = index;
                *((unsigned int *)&index->padding_0[28]) = 0;
            }
        } while ((idx1->field_b8 = idx1->field_b8 + 1, v9 = v11, v11));
    }
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf1d0);
        g_4cf221 = g_4cf221 - 1;
    }
    return 1;
}
