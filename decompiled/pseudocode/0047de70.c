// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47de70; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47de70_1 {
    struct st_47de70_0 *field_0;
} st_47de70_1;

typedef struct st_47de70_0 {
    unsigned int field_0;
} st_47de70_0;

typedef struct st_47de70_2 {
    char padding_0[4];
    struct st_47de70_1 *field_4;
} st_47de70_2;

unsigned int sub_47de70(st_47de70_2 *a0)
{
    if (!a0->field_4)
    {
        return 0;
    }
    else if (a0->field_4->field_0)
    {
        goto *((void *)(*((int *)a0->field_4->field_0->field_0)));
    }
    else
    {
        return 0;
    }
}
