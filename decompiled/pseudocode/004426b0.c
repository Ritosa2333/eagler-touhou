// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4426b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4426b0_2 {
    char padding_0[16];
    struct st_4426b0_1 *field_10;
    struct st_4426b0_2 *field_14;
} st_4426b0_2;

typedef struct st_4426b0_3 {
    char padding_0[16];
    unsigned int field_10;
    struct st_4426b0_3 *field_14;
} st_4426b0_3;

typedef struct st_4426b0_0 {
    char padding_0[716];
    int field_2cc;
} st_4426b0_0;

typedef struct st_4426b0_1 {
    int field_0;
    char padding_4[998];
    unsigned short field_3ea;
} st_4426b0_1;

typedef struct st_4426b0_26 {
    char padding_0[1148];
    unsigned int field_47c;
} st_4426b0_26;

extern char g_4cead0;
extern char g_4cead1;
extern unsigned int g_4d477c;
extern unsigned int g_4d4780;
extern unsigned int g_4d4784;

void sub_4426b0(void)
{
    int v2;  // edi
    int v3;  // esi
    int v12;  // eax
    st_4426b0_2 *v102;  // eax
    st_4426b0_2 *iter;  // eax
    int v104;  // eax
    st_4426b0_26 *idx;  // eax
    st_4426b0_2 *v106;  // eax
    st_4426b0_2 *iter1;  // eax
    int v108;  // eax
    st_4426b0_26 *idx1;  // eax
    st_4426b0_2 *v110;  // eax
    st_4426b0_2 *iter2;  // eax
    st_4426b0_2 *v13;  // eax
    int v112;  // eax
    st_4426b0_26 *idx2;  // eax
    st_4426b0_2 *v114;  // eax
    st_4426b0_2 *v115;  // eax
    int v116;  // eax
    st_4426b0_26 *v117;  // eax
    st_4426b0_2 *v118;  // eax
    st_4426b0_2 *v119;  // eax
    st_4426b0_1 *v120;  // edx
    st_4426b0_2 *v121;  // eax
    st_4426b0_2 *v14;  // eax
    st_4426b0_2 *v122;  // eax
    int v123;  // eax
    st_4426b0_26 *v124;  // eax
    st_4426b0_2 *v125;  // eax
    st_4426b0_2 *v126;  // eax
    int v127;  // eax
    st_4426b0_26 *v128;  // eax
    st_4426b0_2 *v129;  // eax
    st_4426b0_2 *v130;  // eax
    int v131;  // eax
    int v15;  // eax
    st_4426b0_26 *v132;  // eax
    st_4426b0_2 *v133;  // eax
    st_4426b0_2 *node;  // eax
    int v135;  // eax
    st_4426b0_26 *v136;  // eax
    st_4426b0_2 *v16;  // eax
    st_4426b0_2 *v17;  // eax
    int v18;  // eax
    st_4426b0_2 *v19;  // eax
    st_4426b0_2 *v20;  // eax
    int v21;  // eax
    int v4;  // ebp
    st_4426b0_2 *v22;  // eax
    st_4426b0_2 *v23;  // eax
    int v24;  // eax
    st_4426b0_2 *v25;  // eax
    st_4426b0_2 *v26;  // eax
    int v27;  // eax
    st_4426b0_2 *v28;  // eax
    st_4426b0_2 *v29;  // eax
    int v30;  // eax
    st_4426b0_2 *v31;  // eax
    int v5;  // ecx
    st_4426b0_2 *v32;  // eax
    int v33;  // eax
    st_4426b0_2 *v34;  // eax
    st_4426b0_2 *v35;  // eax
    int v36;  // eax
    st_4426b0_2 *v37;  // eax
    st_4426b0_2 *v38;  // eax
    int v39;  // eax
    st_4426b0_2 *v40;  // eax
    st_4426b0_2 *v41;  // eax
    st_4426b0_0 *v6;  // ebx
    int v42;  // eax
    st_4426b0_2 *v43;  // eax
    st_4426b0_2 *v44;  // eax
    int v45;  // eax
    st_4426b0_26 *v46;  // eax
    st_4426b0_2 *v47;  // eax
    st_4426b0_2 *v48;  // eax
    int v49;  // eax
    st_4426b0_26 *v50;  // eax
    st_4426b0_2 *v51;  // eax
    st_4426b0_2 *v7;  // eax
    st_4426b0_2 *v52;  // eax
    int v53;  // eax
    st_4426b0_26 *v54;  // eax
    st_4426b0_2 *v55;  // eax
    st_4426b0_2 *v56;  // eax
    int v57;  // eax
    st_4426b0_26 *index;  // eax
    st_4426b0_2 *v59;  // eax
    st_4426b0_2 *v60;  // eax
    int v61;  // eax
    st_4426b0_2 *v8;  // eax
    st_4426b0_26 *v62;  // eax
    st_4426b0_2 *v63;  // eax
    st_4426b0_2 *v64;  // eax
    int v65;  // eax
    st_4426b0_26 *v66;  // eax
    st_4426b0_2 *v67;  // eax
    st_4426b0_2 *v68;  // eax
    int v69;  // eax
    st_4426b0_26 *v70;  // eax
    st_4426b0_2 *v71;  // eax
    int v9;  // eax
    st_4426b0_2 *v72;  // eax
    st_4426b0_1 *v73;  // edx
    st_4426b0_2 *v74;  // eax
    st_4426b0_2 *v75;  // eax
    int v76;  // eax
    st_4426b0_26 *v77;  // eax
    st_4426b0_2 *v78;  // eax
    st_4426b0_2 *v79;  // eax
    int v80;  // eax
    st_4426b0_26 *v81;  // eax
    st_4426b0_3 *v10;  // eax
    st_4426b0_2 *v82;  // eax
    st_4426b0_2 *v83;  // eax
    int v84;  // eax
    st_4426b0_26 *v85;  // eax
    st_4426b0_2 *v86;  // eax
    st_4426b0_2 *v87;  // eax
    int v88;  // eax
    st_4426b0_26 *v89;  // eax
    st_4426b0_2 *v90;  // eax
    st_4426b0_2 *v91;  // eax
    st_4426b0_3 *v11;  // eax
    int v92;  // eax
    st_4426b0_26 *v93;  // eax
    st_4426b0_2 *v94;  // eax
    st_4426b0_2 *v95;  // eax
    int v96;  // eax
    st_4426b0_26 *v97;  // eax
    st_4426b0_2 *v98;  // eax
    st_4426b0_2 *v99;  // eax
    int v100;  // eax
    st_4426b0_26 *v101;  // eax
    unsigned int v0;  // [bp-0xc]

    g_4d477c = g_4cead0;
    sub_454960(8, 0, v2, v3, v4, v5);
    g_4d4780 = g_4cead1;
    if (g_4cead1)
    {
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            v0 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v0 = nan;
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        g_4d4784 = 0xffffec78 - sub_4931e0();
    }
    else
    {
        g_4d4784 = 0xffffd8f0;
    }
    v7 = sub_461920(v6->field_2cc);
    if (!v7)
        v6->field_2cc = 0;
    v8 = &v7->field_10;
    if (v8)
    {
        do
        {
            if (*((short *)(*((int *)&v8->padding_0[0]) + 1002)) == 29)
            {
                v9 = *((int *)*((int *)&v8->padding_0[0]));
                goto LABEL_442798;
            }
        } while ((v8 = (st_4426b0_2 *)*((int *)&v8->padding_0[4]), v8));
    }
    v9 = 0;
LABEL_442798:
    if (sub_461920(v9))
        sub_454b80();
    v10 = sub_461920(v6->field_2cc);
    if (!v10)
        v6->field_2cc = 0;
    v11 = &v10->field_10;
    if (v11)
    {
        do
        {
            if (*((short *)(*((int *)&v11->padding_0[0]) + 1002)) == 30)
            {
                v12 = *((int *)*((int *)&v11->padding_0[0]));
                goto LABEL_442809;
            }
        } while ((v11 = (st_4426b0_3 *)*((int *)&v11->padding_0[4]), v11));
    }
    v12 = 0;
LABEL_442809:
    if (sub_461920(v12))
        sub_454b80();
    v13 = sub_461920(v6->field_2cc);
    if (!v13)
        v6->field_2cc = 0;
    v14 = &v13->field_10;
    if (v14)
    {
        do
        {
            if (*((short *)(*((int *)&v14->padding_0[0]) + 1002)) == 31)
            {
                v15 = *((int *)*((int *)&v14->padding_0[0]));
                goto LABEL_442888;
            }
        } while ((v14 = (st_4426b0_2 *)*((int *)&v14->padding_0[4]), v14));
    }
    v15 = 0;
LABEL_442888:
    if (sub_461920(v15))
        sub_454b80();
    v16 = sub_461920(v6->field_2cc);
    if (!v16)
        v6->field_2cc = 0;
    v17 = &v16->field_10;
    if (v17)
    {
        do
        {
            if (*((short *)(*((int *)&v17->padding_0[0]) + 1002)) == 33)
            {
                v18 = *((int *)*((int *)&v17->padding_0[0]));
                goto LABEL_4428f8;
            }
        } while ((v17 = (st_4426b0_2 *)*((int *)&v17->padding_0[4]), v17));
    }
    v18 = 0;
LABEL_4428f8:
    if (sub_461920(v18))
        sub_454b80();
    v19 = sub_461920(v6->field_2cc);
    if (!v19)
        v6->field_2cc = 0;
    v20 = &v19->field_10;
    if (v20)
    {
        do
        {
            if (*((short *)(*((int *)&v20->padding_0[0]) + 1002)) == 0x22)
            {
                v21 = *((int *)*((int *)&v20->padding_0[0]));
                goto LABEL_442969;
            }
        } while ((v20 = (st_4426b0_2 *)*((int *)&v20->padding_0[4]), v20));
    }
    v21 = 0;
LABEL_442969:
    if (sub_461920(v21))
        sub_454b80();
    v22 = sub_461920(v6->field_2cc);
    if (!v22)
        v6->field_2cc = 0;
    v23 = &v22->field_10;
    if (v23)
    {
        do
        {
            if (*((short *)(*((int *)&v23->padding_0[0]) + 1002)) == 35)
            {
                v24 = *((int *)*((int *)&v23->padding_0[0]));
                goto LABEL_4429e8;
            }
        } while ((v23 = (st_4426b0_2 *)*((int *)&v23->padding_0[4]), v23));
    }
    v24 = 0;
LABEL_4429e8:
    if (sub_461920(v24))
        sub_454b80();
    v25 = sub_461920(v6->field_2cc);
    if (!v25)
        v6->field_2cc = 0;
    v26 = &v25->field_10;
    if (v26)
    {
        do
        {
            if (*((short *)(*((int *)&v26->padding_0[0]) + 1002)) == 37)
            {
                v27 = *((int *)*((int *)&v26->padding_0[0]));
                goto LABEL_442a58;
            }
        } while ((v26 = (st_4426b0_2 *)*((int *)&v26->padding_0[4]), v26));
    }
    v27 = 0;
LABEL_442a58:
    if (sub_461920(v27))
        sub_454b80();
    v28 = sub_461920(v6->field_2cc);
    if (!v28)
        v6->field_2cc = 0;
    v29 = &v28->field_10;
    if (v29)
    {
        do
        {
            if (*((short *)(*((int *)&v29->padding_0[0]) + 1002)) == 38)
            {
                v30 = *((int *)*((int *)&v29->padding_0[0]));
                goto LABEL_442ac9;
            }
        } while ((v29 = (st_4426b0_2 *)*((int *)&v29->padding_0[4]), v29));
    }
    v30 = 0;
LABEL_442ac9:
    if (sub_461920(v30))
        sub_454b80();
    v31 = sub_461920(v6->field_2cc);
    if (!v31)
        v6->field_2cc = 0;
    v32 = &v31->field_10;
    if (v32)
    {
        do
        {
            if (*((short *)(*((int *)&v32->padding_0[0]) + 1002)) == 39)
            {
                v33 = *((int *)*((int *)&v32->padding_0[0]));
                goto LABEL_442b48;
            }
        } while ((v32 = (st_4426b0_2 *)*((int *)&v32->padding_0[4]), v32));
    }
    v33 = 0;
LABEL_442b48:
    if (sub_461920(v33))
        sub_454b80();
    v34 = sub_461920(v6->field_2cc);
    if (!v34)
        v6->field_2cc = 0;
    v35 = &v34->field_10;
    if (v35)
    {
        do
        {
            if (*((short *)(*((int *)&v35->padding_0[0]) + 1002)) == 41)
            {
                v36 = *((int *)*((int *)&v35->padding_0[0]));
                goto LABEL_442bb8;
            }
        } while ((v35 = (st_4426b0_2 *)*((int *)&v35->padding_0[4]), v35));
    }
    v36 = 0;
LABEL_442bb8:
    if (sub_461920(v36))
        sub_454b80();
    v37 = sub_461920(v6->field_2cc);
    if (!v37)
        v6->field_2cc = 0;
    v38 = &v37->field_10;
    if (v38)
    {
        do
        {
            if (*((short *)(*((int *)&v38->padding_0[0]) + 1002)) == 42)
            {
                v39 = *((int *)*((int *)&v38->padding_0[0]));
                goto LABEL_442c29;
            }
        } while ((v38 = (st_4426b0_2 *)*((int *)&v38->padding_0[4]), v38));
    }
    v39 = 0;
LABEL_442c29:
    if (sub_461920(v39))
        sub_454b80();
    v40 = sub_461920(v6->field_2cc);
    if (!v40)
        v6->field_2cc = 0;
    v41 = &v40->field_10;
    if (v41)
    {
        do
        {
            if (*((short *)(*((int *)&v41->padding_0[0]) + 1002)) == 43)
            {
                v42 = *((int *)*((int *)&v41->padding_0[0]));
                goto LABEL_442ca8;
            }
        } while ((v41 = (st_4426b0_2 *)*((int *)&v41->padding_0[4]), v41));
    }
    v42 = 0;
LABEL_442ca8:
    if (sub_461920(v42))
        sub_454b80();
    if (g_4cead0 < 10)
    {
        v43 = sub_461920(v6->field_2cc);
        if (!v43)
            v6->field_2cc = 0;
        v44 = &v43->field_10;
        if (v44)
        {
            do
            {
                if (*((short *)(*((int *)&v44->padding_0[0]) + 1002)) == 29)
                {
                    v45 = *((int *)*((int *)&v44->padding_0[0]));
                    goto LABEL_442d28;
                }
            } while ((v44 = (st_4426b0_2 *)*((int *)&v44->padding_0[4]), v44));
        }
        v45 = 0;
LABEL_442d28:
        v46 = sub_461920(v45);
        v46->field_47c = v46->field_47c & 0xfffffffd;
        v47 = sub_461920(v6->field_2cc);
        if (!v47)
            v6->field_2cc = 0;
        v48 = &v47->field_10;
        if (v48)
        {
            do
            {
                if (*((short *)(*((int *)&v48->padding_0[0]) + 1002)) == 30)
                {
                    v49 = *((int *)*((int *)&v48->padding_0[0]));
                    goto LABEL_442d78;
                }
            } while ((v48 = (st_4426b0_2 *)*((int *)&v48->padding_0[4]), v48));
        }
        v49 = 0;
LABEL_442d78:
        v50 = sub_461920(v49);
        v50->field_47c = v50->field_47c & 0xfffffffd;
        v51 = sub_461920(v6->field_2cc);
        if (!v51)
            v6->field_2cc = 0;
        v52 = &v51->field_10;
        if (v52)
        {
            do
            {
                if (*((short *)(*((int *)&v52->padding_0[0]) + 1002)) == 33)
                {
                    v53 = *((int *)*((int *)&v52->padding_0[0]));
                    goto LABEL_442dc8;
                }
            } while ((v52 = (st_4426b0_2 *)*((int *)&v52->padding_0[4]), v52));
        }
        v53 = 0;
LABEL_442dc8:
        v54 = sub_461920(v53);
        v54->field_47c = v54->field_47c & 0xfffffffd;
        v55 = sub_461920(v6->field_2cc);
        if (!v55)
            v6->field_2cc = 0;
        v56 = &v55->field_10;
        if (v56)
        {
            do
            {
                if (*((short *)(*((int *)&v56->padding_0[0]) + 1002)) == 0x22)
                {
                    v57 = *((int *)*((int *)&v56->padding_0[0]));
                    goto LABEL_442e18;
                }
            } while ((v56 = (st_4426b0_2 *)*((int *)&v56->padding_0[4]), v56));
        }
        v57 = 0;
LABEL_442e18:
        index = sub_461920(v57);
        index->field_47c = index->field_47c & 0xfffffffd;
    }
    else
    {
        if (g_4cead0 < 100)
        {
            v59 = sub_461920(v6->field_2cc);
            if (!v59)
                v6->field_2cc = 0;
            v60 = &v59->field_10;
            if (v60)
            {
                do
                {
                    if (*((short *)(*((int *)&v60->padding_0[0]) + 1002)) == 29)
                    {
                        v61 = *((int *)*((int *)&v60->padding_0[0]));
                        goto LABEL_442f08;
                    }
                } while ((v60 = (st_4426b0_2 *)*((int *)&v60->padding_0[4]), v60));
            }
            v61 = 0;
LABEL_442f08:
            v62 = sub_461920(v61);
            v62->field_47c = v62->field_47c & 0xfffffffd;
            v63 = sub_461920(v6->field_2cc);
            if (!v63)
                v6->field_2cc = 0;
            v64 = &v63->field_10;
            if (v64)
            {
                do
                {
                    if (*((short *)(*((int *)&v64->padding_0[0]) + 1002)) == 30)
                    {
                        v65 = *((int *)*((int *)&v64->padding_0[0]));
                        goto LABEL_442f58;
                    }
                } while ((v64 = (st_4426b0_2 *)*((int *)&v64->padding_0[4]), v64));
            }
            v65 = 0;
LABEL_442f58:
            v66 = sub_461920(v65);
            v66->field_47c = v66->field_47c | 2;
            v67 = sub_461920(v6->field_2cc);
            if (!v67)
                v6->field_2cc = 0;
            v68 = &v67->field_10;
            if (v68)
            {
                do
                {
                    if (*((short *)(*((int *)&v68->padding_0[0]) + 1002)) == 33)
                    {
                        v69 = *((int *)*((int *)&v68->padding_0[0]));
                        goto LABEL_442fa8;
                    }
                } while ((v68 = (st_4426b0_2 *)*((int *)&v68->padding_0[4]), v68));
            }
            v69 = 0;
LABEL_442fa8:
            v70 = sub_461920(v69);
            v70->field_47c = v70->field_47c & 0xfffffffd;
            v71 = sub_461920(v6->field_2cc);
            if (!v71)
                v6->field_2cc = 0;
            v72 = &v71->field_10;
            if (v72)
            {
                do
                {
                    v73 = *((int *)&v72->padding_0[0]);
                    if (v73->field_3ea == 0x22)
                        goto LABEL_4432c3;
                } while ((v72 = (st_4426b0_2 *)*((int *)&v72->padding_0[4]), v72));
            }
LABEL_442ff6:
            v88 = 0;
        }
        else
        {
            v74 = sub_461920(v6->field_2cc);
            if (!v74)
                v6->field_2cc = 0;
            v75 = &v74->field_10;
            if (v75)
            {
                do
                {
                    if (*((short *)(*((int *)&v75->padding_0[0]) + 1002)) == 29)
                    {
                        v76 = *((int *)*((int *)&v75->padding_0[0]));
                        goto LABEL_4431b8;
                    }
                } while ((v75 = (st_4426b0_2 *)*((int *)&v75->padding_0[4]), v75));
            }
            v76 = 0;
LABEL_4431b8:
            v77 = sub_461920(v76);
            v77->field_47c = v77->field_47c | 2;
            v78 = sub_461920(v6->field_2cc);
            if (!v78)
                v6->field_2cc = 0;
            v79 = &v78->field_10;
            if (v79)
            {
                do
                {
                    if (*((short *)(*((int *)&v79->padding_0[0]) + 1002)) == 30)
                    {
                        v80 = *((int *)*((int *)&v79->padding_0[0]));
                        goto LABEL_443208;
                    }
                } while ((v79 = (st_4426b0_2 *)*((int *)&v79->padding_0[4]), v79));
            }
            v80 = 0;
LABEL_443208:
            v81 = sub_461920(v80);
            v81->field_47c = v81->field_47c | 2;
            v82 = sub_461920(v6->field_2cc);
            if (!v82)
                v6->field_2cc = 0;
            v83 = &v82->field_10;
            if (v83)
            {
                do
                {
                    if (*((short *)(*((int *)&v83->padding_0[0]) + 1002)) == 33)
                    {
                        v84 = *((int *)*((int *)&v83->padding_0[0]));
                        goto LABEL_443258;
                    }
                } while ((v83 = (st_4426b0_2 *)*((int *)&v83->padding_0[4]), v83));
            }
            v84 = 0;
LABEL_443258:
            v85 = sub_461920(v84);
            v85->field_47c = v85->field_47c | 2;
            v86 = sub_461920(v6->field_2cc);
            if (!v86)
                v6->field_2cc = 0;
            v87 = &v86->field_10;
            if (!v87)
                goto LABEL_442ff6;
            while (1)
            {
                v73 = *((int *)&v87->padding_0[0]);
                if (v73->field_3ea == 0x22)
                    break;
                v87 = *((int *)&v87->padding_0[4]);
                if (!v87)
                    goto LABEL_442ff6;
            }
LABEL_4432c3:
            v88 = v73->field_0;
        }
        v89 = sub_461920(v88);
        v89->field_47c = v89->field_47c | 2;
    }
    if (g_4cead1 < 10)
    {
        v90 = sub_461920(v6->field_2cc);
        if (!v90)
            v6->field_2cc = 0;
        v91 = &v90->field_10;
        if (v91)
        {
            do
            {
                if (*((short *)(*((int *)&v91->padding_0[0]) + 1002)) == 37)
                {
                    v92 = *((int *)*((int *)&v91->padding_0[0]));
                    goto LABEL_443058;
                }
            } while ((v91 = (st_4426b0_2 *)*((int *)&v91->padding_0[4]), v91));
        }
        v92 = 0;
LABEL_443058:
        v93 = sub_461920(v92);
        v93->field_47c = v93->field_47c & 0xfffffffd;
        v94 = sub_461920(v6->field_2cc);
        if (!v94)
            v6->field_2cc = 0;
        v95 = &v94->field_10;
        if (v95)
        {
            do
            {
                if (*((short *)(*((int *)&v95->padding_0[0]) + 1002)) == 38)
                {
                    v96 = *((int *)*((int *)&v95->padding_0[0]));
                    goto LABEL_4430a8;
                }
            } while ((v95 = (st_4426b0_2 *)*((int *)&v95->padding_0[4]), v95));
        }
        v96 = 0;
LABEL_4430a8:
        v97 = sub_461920(v96);
        v97->field_47c = v97->field_47c & 0xfffffffd;
        v98 = sub_461920(v6->field_2cc);
        if (!v98)
            v6->field_2cc = 0;
        v99 = &v98->field_10;
        if (v99)
        {
            do
            {
                if (*((short *)(*((int *)&v99->padding_0[0]) + 1002)) == 41)
                {
                    v100 = *((int *)*((int *)&v99->padding_0[0]));
                    goto LABEL_4430f8;
                }
            } while ((v99 = (st_4426b0_2 *)*((int *)&v99->padding_0[4]), v99));
        }
        v100 = 0;
LABEL_4430f8:
        v101 = sub_461920(v100);
        v101->field_47c = v101->field_47c & 0xfffffffd;
        v102 = sub_461920(v6->field_2cc);
        if (!v102)
            v6->field_2cc = 0;
        iter = &v102->field_10;
        if (iter)
        {
            do
            {
                if (*((short *)(*((int *)&iter->padding_0[0]) + 1002)) == 42)
                {
                    v104 = *((int *)*((int *)&iter->padding_0[0]));
                    goto LABEL_443148;
                }
            } while ((iter = (st_4426b0_2 *)*((int *)&iter->padding_0[4]), iter));
        }
        v104 = 0;
LABEL_443148:
        idx = sub_461920(v104);
        idx->field_47c = idx->field_47c & 0xfffffffd;
        return;
    }
    else
    {
        if (g_4cead1 < 100)
        {
            v106 = sub_461920();
            if (!v106)
                v6->field_2cc = 0;
            iter1 = &v106->field_10;
            if (iter1)
            {
                do
                {
                    if (*((short *)(*((int *)&iter1->padding_0[0]) + 1002)) == 37)
                    {
                        v108 = *((int *)*((int *)&iter1->padding_0[0]));
                        goto LABEL_443338;
                    }
                } while ((iter1 = (st_4426b0_2 *)*((int *)&iter1->padding_0[4]), iter1));
            }
            v108 = 0;
LABEL_443338:
            idx1 = sub_461920(v108);
            idx1->field_47c = idx1->field_47c & 0xfffffffd;
            v110 = sub_461920(v6->field_2cc);
            if (!v110)
                v6->field_2cc = 0;
            iter2 = &v110->field_10;
            if (iter2)
            {
                do
                {
                    if (*((short *)(*((int *)&iter2->padding_0[0]) + 1002)) == 38)
                    {
                        v112 = *((int *)*((int *)&iter2->padding_0[0]));
                        goto LABEL_443388;
                    }
                } while ((iter2 = (st_4426b0_2 *)*((int *)&iter2->padding_0[4]), iter2));
            }
            v112 = 0;
LABEL_443388:
            idx2 = sub_461920(v112);
            idx2->field_47c = idx2->field_47c | 2;
            v114 = sub_461920(v6->field_2cc);
            if (!v114)
                v6->field_2cc = 0;
            v115 = &v114->field_10;
            if (v115)
            {
                do
                {
                    if (*((short *)(*((int *)&v115->padding_0[0]) + 1002)) == 41)
                    {
                        v116 = *((int *)*((int *)&v115->padding_0[0]));
                        goto LABEL_4433d8;
                    }
                } while ((v115 = (st_4426b0_2 *)*((int *)&v115->padding_0[4]), v115));
            }
            v116 = 0;
LABEL_4433d8:
            v117 = sub_461920(v116);
            v117->field_47c = v117->field_47c & 0xfffffffd;
            v118 = sub_461920(v6->field_2cc);
            if (!v118)
                v6->field_2cc = 0;
            v119 = &v118->field_10;
            if (v119)
            {
                do
                {
                    v120 = *((int *)&v119->padding_0[0]);
                    if (v120->field_3ea == 42)
                        goto LABEL_4435a3;
                } while ((v119 = (st_4426b0_2 *)*((int *)&v119->padding_0[4]), v119));
            }
LABEL_443426:
            v135 = 0;
        }
        else
        {
            v121 = sub_461920();
            if (!v121)
                v6->field_2cc = 0;
            v122 = &v121->field_10;
            if (v122)
            {
                do
                {
                    if (*((short *)(*((int *)&v122->padding_0[0]) + 1002)) == 37)
                    {
                        v123 = *((int *)*((int *)&v122->padding_0[0]));
                        goto LABEL_443488;
                    }
                } while ((v122 = (st_4426b0_2 *)*((int *)&v122->padding_0[4]), v122));
            }
            v123 = 0;
LABEL_443488:
            v124 = sub_461920(v123);
            v124->field_47c = v124->field_47c | 2;
            v125 = sub_461920(v6->field_2cc);
            if (!v125)
                v6->field_2cc = 0;
            v126 = &v125->field_10;
            if (v126)
            {
                do
                {
                    if (*((short *)(*((int *)&v126->padding_0[0]) + 1002)) == 38)
                    {
                        v127 = *((int *)*((int *)&v126->padding_0[0]));
                        goto LABEL_4434d8;
                    }
                } while ((v126 = (st_4426b0_2 *)*((int *)&v126->padding_0[4]), v126));
            }
            v127 = 0;
LABEL_4434d8:
            v128 = sub_461920(v127);
            v128->field_47c = v128->field_47c | 2;
            v129 = sub_461920(v6->field_2cc);
            if (!v129)
                v6->field_2cc = 0;
            v130 = &v129->field_10;
            if (v130)
            {
                do
                {
                    if (*((short *)(*((int *)&v130->padding_0[0]) + 1002)) == 41)
                    {
                        v131 = *((int *)*((int *)&v130->padding_0[0]));
                        goto LABEL_443528;
                    }
                } while ((v130 = (st_4426b0_2 *)*((int *)&v130->padding_0[4]), v130));
            }
            v131 = 0;
LABEL_443528:
            v132 = sub_461920(v131);
            v132->field_47c = v132->field_47c | 2;
            v133 = sub_461920(v6->field_2cc);
            if (!v133)
                v6->field_2cc = 0;
            node = &v133->field_10;
            if (!node)
                goto LABEL_443426;
            while (1)
            {
                v120 = *((int *)&node->padding_0[0]);
                if (v120->field_3ea == 42)
                    break;
                node = *((int *)&node->padding_0[4]);
                if (!node)
                    goto LABEL_443426;
            }
LABEL_4435a3:
            v135 = v120->field_0;
        }
        v136 = sub_461920(v135);
        v136->field_47c = v136->field_47c | 2;
        return;
    }
}
