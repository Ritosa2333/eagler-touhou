// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x475481; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_475481_1 {
    unsigned int field_0;
} st_475481_1;

typedef struct st_475481_0 {
    char padding_0[36];
    int field_24;
    char padding_28[4];
    int field_2c;
    char padding_30[4];
    int field_34;
    char padding_38[4];
    int field_3c;
    int field_40;
    int field_44;
    int field_48;
    char padding_4c[16];
    int field_5c;
    char padding_60[8];
    int field_68;
    struct st_475481_1 *field_6c;
} st_475481_0;

extern int g_49d5c8;
extern int g_4aad50;
extern int g_4ad4b0;
extern unsigned int g_4ad9e0;
extern char g_4adab8;

void sub_475481(unsigned int a0)
{
    void* v0;  // ebp
    st_475481_0 *idx;  // esi
    int v2;  // edi
    unsigned int *v3;  // edi

    sub_46fcec(&g_4aad50, 8);
    idx = (int)v0[8];
    if (idx)
    {
        if (idx->field_24)
            sub_46c941(idx->field_24);
        if (idx->field_2c)
            sub_46c941(idx->field_2c);
        if (idx->field_34)
            sub_46c941(idx->field_34);
        if (idx->field_3c)
            sub_46c941(idx->field_3c);
        if (idx->field_40)
            sub_46c941(idx->field_40);
        if (idx->field_44)
            sub_46c941(idx->field_44);
        if (idx->field_48)
            sub_46c941(idx->field_48);
        if (idx->field_5c != &g_49d5c8)
            sub_46c941(idx->field_5c);
        sub_46ec9a(13);
        *((unsigned int *)((char *)v0 - 4)) = 0;
        v2 = idx->field_68;
        if (v2 && !InterlockedDecrement(v2) && v2 != &g_4ad4b0)
            sub_46c941(v2);
        *((unsigned int *)((char *)v0 - 4)) = 0xfffffffe;
        sub_47559b();
        sub_46ec9a(12);
        *((unsigned int *)((char *)v0 - 4)) = 1;
        v3 = &idx->field_6c->field_0;
        if (v3)
        {
            sub_474084(v3);
            if (v3 != *((int *)&g_4adab8) && v3 != &g_4ad9e0 && !*(v3))
                sub_473eac(v3);
        }
        *((unsigned int *)((char *)v0 - 4)) = 0xfffffffe;
        sub_4755a7();
        sub_46c941(idx);
    }
    sub_46fd31();
    return;
}
