// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47295a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47295a_0 {
    char padding_0[4];
    int field_4;
} st_47295a_0;

typedef struct st_47295a_2 {
    char padding_0[4];
    struct st_47295a_1 *field_4;
} st_47295a_2;

typedef struct st_47295a_1 {
    unsigned int field_0;
    struct st_47295a_1 *field_4;
} st_47295a_1;

extern char g_4aaba0;
extern int g_4ad138;

void sub_47295a(unsigned int a0)
{
    void* idx;  // ebp
    st_47295a_0 *v5;  // esi
    int v6;  // edi
    int v7;  // esi
    int v8;  // esi
    char *v9;  // eax
    st_47295a_1 *v10;  // edi
    int v11;  // ebx
    int v12;  // esi
    st_47295a_2 *index;  // eax
    unsigned int v0;  // [bp+0xc]
    unsigned int v1;  // [bp+0x10]
    unsigned int v2;  // [bp+0x14]
    unsigned int v3;  // [bp+0x18]

    sub_46fcec(&g_4aaba0, 16);
    v5 = (int)idx[8];
    if (!v5->field_4)
    {
        sub_46ec9a(14);
        *((unsigned int *)((char *)idx - 4)) = 0;
        if (!v5->field_4)
        {
            v6 = sub_472927(0, &v5[1].padding_0[1], 0, 0x2800);
            *((int *)((char *)idx - 28)) = v6;
            if (!v6)
            {
                sub_47b478(&g_4ad138, idx - 16, -0x2);
                sub_46fd31(v0, v1, v2, v3);
                return;
            }
            v7 = sub_475860(v6);
            *((int *)((char *)idx - 32)) = v7;
            while (1)
            {
                v8 = v7 - 1;
                *((int *)((char *)idx - 32)) = v8;
                if (!v7 || !(v9 = (char *)(v8 + v6), *(v9) == 32))
                    break;
                *(v9) = 0;
                v7 = v8;
            }
            v10 = sub_46d04a(8);
            if (v10)
            {
                v11 = v8 + 2;
                v12 = sub_46d04a(v11);
                if (v12)
                {
                    if (sub_47a2b9(v12, v11, *((int *)((char *)idx - 28))))
                        sub_470dc6(0, 0, 0, 0, 0);
                    *((int *)((int)idx[8] + 4)) = v12;
                    v10->field_0 = v12;
                    index = (int)idx[12];
                    v10->field_4 = index->field_4;
                    index->field_4 = v10;
                }
                else
                {
                    sub_46c941(v10, &g_4aaba0);
                }
            }
            sub_46c941(*((int *)((char *)idx - 28)), v0);
        }
        *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
        sub_472a60();
    }
    sub_46fd31(v0, v1, v2, v3);
    return;
}
