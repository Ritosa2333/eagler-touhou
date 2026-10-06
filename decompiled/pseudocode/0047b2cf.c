// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47b2cf; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47b2cf_0 {
    char padding_0[8];
    unsigned int field_8;
} st_47b2cf_0;

extern st_47b2cf_0 *g_4d6320;
extern st_47b2cf_0 *g_4d6420;

void sub_47b2cf(void)
{
    st_47b2cf_0 **iter;  // esi
    st_47b2cf_0 *v2;  // edi
    st_47b2cf_0 *i;  // eax
    unsigned int v4;  // edi

    iter = &g_4d6320;
    do
    {
        v2 = *(iter);
        if (v2)
        {
            for (i = &v2[0xaa].field_8; v2 < i; i = &(*(iter))[0xaa].field_8)
            {
                if (v2->field_8)
                    DeleteCriticalSection(v2 + 1);
                v2 = &v2[5].padding_0[4];
            }
            sub_46c941(*(iter), v4);
            *(iter) = NULL;
        }
    } while ((iter += 4, iter < &g_4d6420));
    return;
}
