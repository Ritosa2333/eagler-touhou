// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46e7ca; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46e7ca_0 {
    int field_0;
} st_46e7ca_0;

int sub_46e7ca(void)
{
    void* v1;  // ebp
    st_46e7ca_0 **v2;  // eax
    int v3;  // ecx

    v2 = *((int *)((char *)v1 - 20));
    v3 = *(v2)->field_0;
    *((int *)((char *)v1 - 36)) = v3;
    return sub_477b33(v3, v2);
}
