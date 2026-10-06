// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4926ac; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4926ac_1 {
    char padding_0[140];
    unsigned int field_8c;
} st_4926ac_1;

typedef struct st_4926ac_0 {
    char padding_0[24];
    int field_18;
} st_4926ac_0;

extern unsigned int g_4ab4f0;

int sub_4926ac(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5)
{
    int v0;  // ecx
    int v1;  // ebx
    void* idx;  // ebp
    void* v3;  // edi
    st_4926ac_0 *v4;  // esi
    st_4926ac_1 *v5;  // eax

    sub_46fcec(&g_4ab4f0, 44);
    v1 = v0;
    v3 = (int)idx[12];
    v4 = (int)idx[8];
    *((int *)((char *)idx - 28)) = v1;
    *((unsigned int *)((char *)idx - 52)) = 0;
    *((int *)((char *)idx - 36)) = *((int *)((char *)v3 - 4));
    *((unsigned int *)((char *)idx - 40)) = sub_491d54(idx - 60, v4->field_18);
    *((int *)((char *)idx - 44)) = *((int *)(sub_475467() + 0x88));
    *((int *)((char *)idx - 48)) = *((int *)(sub_475467() + 140));
    *((st_4926ac_0 **)(sub_475467() + 0x88)) = v4;
    v5 = sub_475467();
    v5->field_8c = (int)idx[16];
    *((unsigned int *)((char *)idx - 4)) = 0;
    *((unsigned int *)&idx[16]) = 1;
    *((unsigned int *)((char *)idx - 4)) = 1;
    *((unsigned int *)((char *)idx - 28)) = sub_491df9(v3, (int)idx[20], v1, (int)idx[24], (int)idx[28]);
    *((unsigned int *)((char *)idx - 4)) = 0;
    *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
    *((unsigned int *)&idx[16]) = 0;
    sub_4927d2();
    return sub_46fd31();
}
