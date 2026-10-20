// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44f370; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44f370_0 {
    unsigned int field_0;
    char padding_4[12];
    char field_10;
} st_44f370_0;

typedef struct st_44f370_1 {
    char padding_0[288];
    unsigned int field_120;
} st_44f370_1;

extern char g_4b50c0;
extern unsigned int g_4ce8cc;

void sub_44f370(void)
{
    unsigned int iter;  // edi
    st_44f370_1 *v4;  // eax
    unsigned int v5;  // ebp
    unsigned int v6;  // esi
    st_44f370_0 *v7;  // ecx
    unsigned int v0;  // [bp-0x4]
    unsigned int v1;  // [bp-0x4]

    iter = &(&g_4b50c0)[g_4ce8cc];
    v0 = 32;
    do
    {
        v1 = v0;
        v4 = *((int *)iter);
        if (v4)
        {
            v5 = 0;
            if (*((int *)&v4->padding_0[268]) > NULL)
            {
                v6 = 0;
                do
                {
                    v7 = v4->field_120 + v6;
                    if (v7->field_10 & 1 && v7->field_0)
                    {
                        (*((int *)(*((int *)*((int *)(v4->field_120 + v6))) + 8)))(*((int *)(v4->field_120 + v6)));
                        *((unsigned int *)(v6 + *((int *)(*((int *)iter) + 288)))) = 0;
                    }
                } while ((v4 = (st_44f370_1 *)*((int *)iter), v5 += 1, v6 += 20, v5 < *((int *)(*((int *)iter) + 268))));
            }
        }
    } while ((iter += 4, v0 = v1 - 1, v1 != 1));
    return;
}
