// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x443920; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_443920_2 {
    char padding_0[16];
    struct st_443920_1 *field_10;
    struct st_443920_2 *field_14;
} st_443920_2;

typedef struct st_443920_5 {
    char padding_0[16];
    unsigned int field_10;
    struct st_443920_5 *field_14;
} st_443920_5;

typedef struct st_443920_0 {
    char padding_0[720];
    int field_2d0;
} st_443920_0;

typedef struct st_443920_1 {
    int field_0;
    char padding_4[998];
    unsigned short field_3ea;
} st_443920_1;

unsigned int sub_443920(void)
{
    st_443920_0 *idx;  // edi
    int v2;  // esi
    st_443920_5 *v11;  // eax
    st_443920_5 *iter;  // eax
    int v13;  // eax
    st_443920_2 *v14;  // eax
    st_443920_2 *node;  // eax
    int v16;  // eax
    st_443920_2 *v17;  // eax
    st_443920_2 *iter1;  // eax
    int v19;  // eax
    st_443920_2 *v20;  // eax
    int v3;  // ebp
    st_443920_2 *iter2;  // eax
    int v22;  // eax
    st_443920_2 *v23;  // eax
    st_443920_2 *v24;  // eax
    int v25;  // eax
    st_443920_2 *v26;  // eax
    st_443920_2 *v27;  // eax
    int v28;  // eax
    st_443920_2 *v29;  // eax
    st_443920_2 *v30;  // eax
    int v4;  // ebx
    int v31;  // eax
    st_443920_2 *v32;  // eax
    st_443920_2 *v33;  // eax
    int v34;  // eax
    st_443920_2 *v35;  // eax
    st_443920_2 *v36;  // eax
    int v37;  // eax
    st_443920_2 *v38;  // eax
    st_443920_2 *v39;  // eax
    int v40;  // eax
    st_443920_2 *v5;  // eax
    st_443920_2 *v41;  // eax
    st_443920_2 *v42;  // eax
    int v43;  // eax
    st_443920_2 *v44;  // eax
    st_443920_2 *v45;  // eax
    int v46;  // eax
    st_443920_2 *v47;  // eax
    st_443920_2 *v48;  // eax
    int v49;  // eax
    st_443920_2 *v50;  // eax
    st_443920_2 *v6;  // eax
    st_443920_2 *v51;  // eax
    int v52;  // eax
    st_443920_2 *v53;  // eax
    st_443920_2 *v54;  // eax
    int v55;  // eax
    st_443920_2 *v56;  // eax
    st_443920_2 *v57;  // eax
    int v58;  // eax
    st_443920_2 *v59;  // eax
    st_443920_2 *v60;  // eax
    int v7;  // eax
    int v61;  // eax
    st_443920_2 *v62;  // eax
    st_443920_2 *v63;  // eax
    int v64;  // eax
    unsigned int v65;  // eax
    st_443920_2 *v8;  // eax
    st_443920_2 *v9;  // eax
    int v10;  // eax

    v5 = sub_461920(idx->field_2d0, v2, v3, v4);
    if (!v5)
        idx->field_2d0 = 0;
    v6 = &v5->field_10;
    if (v6)
    {
        do
        {
            if (*((short *)(*((int *)&v6->padding_0[0]) + 1002)) == 59)
            {
                v7 = *((int *)*((int *)&v6->padding_0[0]));
                goto LABEL_443968;
            }
        } while ((v6 = (st_443920_2 *)*((int *)&v6->padding_0[4]), v6));
    }
    v7 = 0;
LABEL_443968:
    if (sub_461920(v7))
        sub_454b80();
    v8 = sub_461920(idx->field_2d0);
    if (!v8)
        idx->field_2d0 = 0;
    v9 = &v8->field_10;
    if (v9)
    {
        do
        {
            if (*((short *)(*((int *)&v9->padding_0[0]) + 1002)) == 60)
            {
                v10 = *((int *)*((int *)&v9->padding_0[0]));
                goto LABEL_4439d9;
            }
        } while ((v9 = (st_443920_2 *)*((int *)&v9->padding_0[4]), v9));
    }
    v10 = 0;
LABEL_4439d9:
    if (sub_461920(v10))
        sub_454b80();
    v11 = sub_461920(idx->field_2d0);
    if (!v11)
        idx->field_2d0 = 0;
    iter = &v11->field_10;
    if (iter)
    {
        do
        {
            if (*((short *)(*((int *)&iter->padding_0[0]) + 1002)) == 69)
            {
                v13 = *((int *)*((int *)&iter->padding_0[0]));
                goto LABEL_443a48;
            }
        } while ((iter = (st_443920_5 *)*((int *)&iter->padding_0[4]), iter));
    }
    v13 = 0;
LABEL_443a48:
    if (sub_461920(v13))
        sub_454b80();
    v14 = sub_461920(idx->field_2d0);
    if (!v14)
        idx->field_2d0 = 0;
    node = &v14->field_10;
    if (node)
    {
        do
        {
            if (*((short *)(*((int *)&node->padding_0[0]) + 1002)) == 70)
            {
                v16 = *((int *)*((int *)&node->padding_0[0]));
                goto LABEL_443ab9;
            }
        } while ((node = (st_443920_2 *)*((int *)&node->padding_0[4]), node));
    }
    v16 = 0;
LABEL_443ab9:
    if (sub_461920(v16))
        sub_454b80();
    v17 = sub_461920(idx->field_2d0);
    if (!v17)
        idx->field_2d0 = 0;
    iter1 = &v17->field_10;
    if (iter1)
    {
        do
        {
            if (*((short *)(*((int *)&iter1->padding_0[0]) + 1002)) == 61)
            {
                v19 = *((int *)*((int *)&iter1->padding_0[0]));
                goto LABEL_443b28;
            }
        } while ((iter1 = (st_443920_2 *)*((int *)&iter1->padding_0[4]), iter1));
    }
    v19 = 0;
LABEL_443b28:
    if (sub_461920(v19))
        sub_454b80();
    v20 = sub_461920(idx->field_2d0);
    if (!v20)
        idx->field_2d0 = 0;
    iter2 = &v20->field_10;
    if (iter2)
    {
        do
        {
            if (*((short *)(*((int *)&iter2->padding_0[0]) + 1002)) == 62)
            {
                v22 = *((int *)*((int *)&iter2->padding_0[0]));
                goto LABEL_443b99;
            }
        } while ((iter2 = (st_443920_2 *)*((int *)&iter2->padding_0[4]), iter2));
    }
    v22 = 0;
LABEL_443b99:
    if (sub_461920(v22))
        sub_454b80();
    v23 = sub_461920(idx->field_2d0);
    if (!v23)
        idx->field_2d0 = 0;
    v24 = &v23->field_10;
    if (v24)
    {
        do
        {
            if (*((short *)(*((int *)&v24->padding_0[0]) + 1002)) == 71)
            {
                v25 = *((int *)*((int *)&v24->padding_0[0]));
                goto LABEL_443c08;
            }
        } while ((v24 = (st_443920_2 *)*((int *)&v24->padding_0[4]), v24));
    }
    v25 = 0;
LABEL_443c08:
    if (sub_461920(v25))
        sub_454b80();
    v26 = sub_461920(idx->field_2d0);
    if (!v26)
        idx->field_2d0 = 0;
    v27 = &v26->field_10;
    if (v27)
    {
        do
        {
            if (*((short *)(*((int *)&v27->padding_0[0]) + 1002)) == 72)
            {
                v28 = *((int *)*((int *)&v27->padding_0[0]));
                goto LABEL_443c79;
            }
        } while ((v27 = (st_443920_2 *)*((int *)&v27->padding_0[4]), v27));
    }
    v28 = 0;
LABEL_443c79:
    if (sub_461920(v28))
        sub_454b80();
    v29 = sub_461920(idx->field_2d0);
    if (!v29)
        idx->field_2d0 = 0;
    v30 = &v29->field_10;
    if (v30)
    {
        do
        {
            if (*((short *)(*((int *)&v30->padding_0[0]) + 1002)) == 63)
            {
                v31 = *((int *)*((int *)&v30->padding_0[0]));
                goto LABEL_443ce8;
            }
        } while ((v30 = (st_443920_2 *)*((int *)&v30->padding_0[4]), v30));
    }
    v31 = 0;
LABEL_443ce8:
    if (sub_461920(v31))
        sub_454b80();
    v32 = sub_461920(idx->field_2d0);
    if (!v32)
        idx->field_2d0 = 0;
    v33 = &v32->field_10;
    if (v33)
    {
        do
        {
            if (*((short *)(*((int *)&v33->padding_0[0]) + 1002)) == 64)
            {
                v34 = *((int *)*((int *)&v33->padding_0[0]));
                goto LABEL_443d59;
            }
        } while ((v33 = (st_443920_2 *)*((int *)&v33->padding_0[4]), v33));
    }
    v34 = 0;
LABEL_443d59:
    if (sub_461920(v34))
        sub_454b80();
    v35 = sub_461920(idx->field_2d0);
    if (!v35)
        idx->field_2d0 = 0;
    v36 = &v35->field_10;
    if (v36)
    {
        do
        {
            if (*((short *)(*((int *)&v36->padding_0[0]) + 1002)) == 73)
            {
                v37 = *((int *)*((int *)&v36->padding_0[0]));
                goto LABEL_443dc8;
            }
        } while ((v36 = (st_443920_2 *)*((int *)&v36->padding_0[4]), v36));
    }
    v37 = 0;
LABEL_443dc8:
    if (sub_461920(v37))
        sub_454b80();
    v38 = sub_461920(idx->field_2d0);
    if (!v38)
        idx->field_2d0 = 0;
    v39 = &v38->field_10;
    if (v39)
    {
        do
        {
            if (*((short *)(*((int *)&v39->padding_0[0]) + 1002)) == 74)
            {
                v40 = *((int *)*((int *)&v39->padding_0[0]));
                goto LABEL_443e39;
            }
        } while ((v39 = (st_443920_2 *)*((int *)&v39->padding_0[4]), v39));
    }
    v40 = 0;
LABEL_443e39:
    if (sub_461920(v40))
        sub_454b80();
    v41 = sub_461920(idx->field_2d0);
    if (!v41)
        idx->field_2d0 = 0;
    v42 = &v41->field_10;
    if (v42)
    {
        do
        {
            if (*((short *)(*((int *)&v42->padding_0[0]) + 1002)) == 65)
            {
                v43 = *((int *)*((int *)&v42->padding_0[0]));
                goto LABEL_443ea8;
            }
        } while ((v42 = (st_443920_2 *)*((int *)&v42->padding_0[4]), v42));
    }
    v43 = 0;
LABEL_443ea8:
    if (sub_461920(v43))
        sub_454b80();
    v44 = sub_461920(idx->field_2d0);
    if (!v44)
        idx->field_2d0 = 0;
    v45 = &v44->field_10;
    if (v45)
    {
        do
        {
            if (*((short *)(*((int *)&v45->padding_0[0]) + 1002)) == 66)
            {
                v46 = *((int *)*((int *)&v45->padding_0[0]));
                goto LABEL_443f19;
            }
        } while ((v45 = (st_443920_2 *)*((int *)&v45->padding_0[4]), v45));
    }
    v46 = 0;
LABEL_443f19:
    if (sub_461920(v46))
        sub_454b80();
    v47 = sub_461920(idx->field_2d0);
    if (!v47)
        idx->field_2d0 = 0;
    v48 = &v47->field_10;
    if (v48)
    {
        do
        {
            if (*((short *)(*((int *)&v48->padding_0[0]) + 1002)) == 75)
            {
                v49 = *((int *)*((int *)&v48->padding_0[0]));
                goto LABEL_443f88;
            }
        } while ((v48 = (st_443920_2 *)*((int *)&v48->padding_0[4]), v48));
    }
    v49 = 0;
LABEL_443f88:
    if (sub_461920(v49))
        sub_454b80();
    v50 = sub_461920(idx->field_2d0);
    if (!v50)
        idx->field_2d0 = 0;
    v51 = &v50->field_10;
    if (v51)
    {
        do
        {
            if (*((short *)(*((int *)&v51->padding_0[0]) + 1002)) == 76)
            {
                v52 = *((int *)*((int *)&v51->padding_0[0]));
                goto LABEL_443ff9;
            }
        } while ((v51 = (st_443920_2 *)*((int *)&v51->padding_0[4]), v51));
    }
    v52 = 0;
LABEL_443ff9:
    if (sub_461920(v52))
        sub_454b80();
    v53 = sub_461920(idx->field_2d0);
    if (!v53)
        idx->field_2d0 = 0;
    v54 = &v53->field_10;
    if (v54)
    {
        do
        {
            if (*((short *)(*((int *)&v54->padding_0[0]) + 1002)) == 67)
            {
                v55 = *((int *)*((int *)&v54->padding_0[0]));
                goto LABEL_444068;
            }
        } while ((v54 = (st_443920_2 *)*((int *)&v54->padding_0[4]), v54));
    }
    v55 = 0;
LABEL_444068:
    if (sub_461920(v55))
        sub_454b80();
    v56 = sub_461920(idx->field_2d0);
    if (!v56)
        idx->field_2d0 = 0;
    v57 = &v56->field_10;
    if (v57)
    {
        do
        {
            if (*((short *)(*((int *)&v57->padding_0[0]) + 1002)) == 0x44)
            {
                v58 = *((int *)*((int *)&v57->padding_0[0]));
                goto LABEL_4440d9;
            }
        } while ((v57 = (st_443920_2 *)*((int *)&v57->padding_0[4]), v57));
    }
    v58 = 0;
LABEL_4440d9:
    if (sub_461920(v58))
        sub_454b80();
    v59 = sub_461920(idx->field_2d0);
    if (!v59)
        idx->field_2d0 = 0;
    v60 = &v59->field_10;
    if (v60)
    {
        do
        {
            if (*((short *)(*((int *)&v60->padding_0[0]) + 1002)) == 77)
            {
                v61 = *((int *)*((int *)&v60->padding_0[0]));
                goto LABEL_444148;
            }
        } while ((v60 = (st_443920_2 *)*((int *)&v60->padding_0[4]), v60));
    }
    v61 = 0;
LABEL_444148:
    if (sub_461920(v61))
        sub_454b80();
    v62 = sub_461920(idx->field_2d0);
    if (!v62)
        idx->field_2d0 = 0;
    v63 = &v62->field_10;
    if (v63)
    {
        do
        {
            if (*((short *)(*((int *)&v63->padding_0[0]) + 1002)) == 78)
            {
                v64 = *((int *)*((int *)&v63->padding_0[0]));
                goto LABEL_4441b9;
            }
        } while ((v63 = (st_443920_2 *)*((int *)&v63->padding_0[4]), v63));
    }
    v64 = 0;
LABEL_4441b9:
    v65 = sub_461920(v64);
    if (!v65)
        return v65;
    return sub_454b80();
}
