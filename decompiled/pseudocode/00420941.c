// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x420941; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_420941_1 {
    char padding_0[92];
    unsigned int field_5c;
} st_420941_1;

typedef struct st_420941_0 {
    char padding_0[72];
    int field_48;
} st_420941_0;

extern st_420941_0 *g_4b43dc;

int sub_420941(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5, unsigned int a6, unsigned int a7)
{
    st_420941_1 *v1;  // ebp
    char v0;  // [bp+0x30]

    sub_4615a0(g_4b43dc->field_48, &v0, 14, 0);
    v1->field_5c = a7;
}
