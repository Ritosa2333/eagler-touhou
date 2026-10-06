// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4929c7; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4929c7_0 {
    char padding_0[24];
    unsigned int field_18;
} st_4929c7_0;

extern int g_4ab538;

void sub_4929c7(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    void* idx;  // ebp
    unsigned int *v1;  // eax
    unsigned int v2;  // ebx
    st_4929c7_0 *v3;  // esi
    st_4929c7_0 *v4;  // edi
    int v5;  // eax
    unsigned int v6;  // eax
    unsigned int v7;  // eax

    sub_46fcec(&g_4ab538, 8);
    v1 = (int)idx[16];
    v2 = (!(*(v1) & 0x80000000) ? v1[2] + (int)idx[12] + 12 : (int)idx[12]);
    *((unsigned int *)((char *)idx - 4)) = 0;
    v3 = (int)idx[20];
    v4 = (int)idx[8];
    v5 = sub_492848(v4, (int)idx[12], v1, v3);
    if (v5 == 1)
    {
        v7 = sub_4921d6(v4->field_18, &v3->padding_0[8]);
        sub_491a13(v2, v3->field_18, v7);
    }
    else if (v5 == 2)
    {
        a0 = 1;
        v6 = sub_4921d6(v4->field_18, &v3->padding_0[8]);
        sub_491a1a(v2, v3->field_18, v6);
    }
    *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
    sub_46fd31();
    return;
}
