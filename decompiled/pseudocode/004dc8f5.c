// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dc8f5; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dc8f5_0 {
    char padding_0[608];
    int field_260;
    char padding_264[4];
    unsigned int field_268;
} st_4dc8f5_0;


int sub_4dc8f5(st_4dc8f5_0 *a0, unsigned int a1, unsigned int a2)
{
    unsigned int v0;  // edx
    st_4dc8f5_0 *v1;  // esi
    unsigned int v2;  // eax
    st_4dc8f5_0 *v3;  // esi
    unsigned int v4;  // edx
    unsigned int v6;  // eax
    unsigned int v7;  // eax
    int v8;  // eax

    v0 = 0;
    v1 = &a0->field_268;
    do
    {
        *((unsigned int *)&v1->padding_0[0]) = v0;
        v2 = sub_4dcb63(v1);
        v3 = &v1->padding_0[4];
        v0 = v4 + (1 << (*((char *)(&v1->padding_0[v2] + L"I\x86\x00蘌耀")) & 31));
        v1 = v3;
    } while (v2 + 1 < 58);
    v6 = sub_4dc680(&a0->padding_0[160], v4, 28, sub_4dc680(&a0->padding_0[16], v0, 721, a2));
    v7 = sub_4dc680(&a0->padding_0[304], v4, 8, v6);
    v8 = sub_4dc680(&a0->padding_0[448], v4, 19, v7);
    a0->field_260 = v8;
    return v8 + 757;
}
