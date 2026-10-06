// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47de2f; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47de2f_0 {
    char padding_0[4];
    char field_4;
} st_47de2f_0;


char * sub_47de2f(st_47de2f_0 *a0, unsigned int a1, char *a2, char *a3)
{
    char *iter;  // eax

    iter = a2;
    if (iter < a3)
    {
        *(iter) = a0->field_4;
        iter += 1;
    }
    return iter;
}
