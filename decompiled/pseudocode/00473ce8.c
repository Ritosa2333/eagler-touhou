// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x473ce8; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_473ce8_1 {
    unsigned int field_0;
    unsigned int field_4;
} st_473ce8_1;

typedef struct st_473ce8_0 {
    char padding_0[104];
    struct st_473ce8_1 *field_68;
} st_473ce8_0;

extern char g_4aac60;
extern unsigned int g_4ad4b0[2];

void sub_473ce8(unsigned int a0)
{
    void* v1;  // ebp
    st_473ce8_0 *idx;  // edi
    unsigned int *v3;  // ebx
    unsigned int v4;  // eax
    unsigned int *v5;  // ebx
    unsigned int v6;  // ecx
    unsigned int *i;  // esi
    unsigned int *iter;  // edi
    unsigned int v9;  // eax
    st_473ce8_0 *v10;  // esi
    unsigned int v0;  // [bp+0x0]

    sub_46fcec(&g_4aac60, 20);
    *((unsigned int *)((char *)v1 - 32)) = 0xffffffff;
    idx = sub_475467(&g_4aac60, 20);
    *((st_473ce8_0 **)((char *)v1 - 36)) = idx;
    sub_4739a5(&g_4aac60);
    v3 = &idx->field_68->field_0;
    v4 = sub_473a49();
    *((unsigned int *)&v1[8]) = v4;
    if (v4 != v3[1])
    {
        v5 = sub_4777ce(544);
        if (v5)
        {
            v6 = 0x88;
            i = &idx->field_68->field_0;
            for (iter = v5; v6; i += 1)
            {
                v6 -= 1;
                *(iter) = *(i);
                iter += 1;
            }
            *(v5) = 0;
            v9 = sub_473ac5((int)v1[8], v5);
            *((unsigned int *)((char *)v1 - 32)) = v9;
            if (!v9)
            {
                v10 = *((int *)((char *)v1 - 36));
                if (!InterlockedDecrement(v10->field_68) && v10->field_68 != &g_4ad4b0[0])
                    sub_46c941(v10->field_68, &g_4aac60);
                v10->field_68 = v5;
                sub_473d7c();
                return;
            }
            else if (v9 == 0xffffffff)
            {
                if (v5 != &g_4ad4b0[0])
                    sub_46c941(v5, &g_4aac60);
                *((unsigned int *)sub_46e96f()) = 22;
            }
        }
    }
    else
    {
        *((unsigned int *)((char *)v1 - 32)) = 0;
    }
    sub_46fd31(&g_4aac60, 20, v0, a0);
    return;
}
