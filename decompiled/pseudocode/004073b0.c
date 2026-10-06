// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4073b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4073b0_0 {
    char padding_0[20];
    unsigned int field_14;
} st_4073b0_0;

int sub_4073b0(void)
{
    st_4073b0_0 *v9;  // ecx
    unsigned int v10;  // edx
    unsigned int v20;  // ftop
    unsigned int v21;  // ftop
    unsigned int v23;  // 4136
    unsigned int v24;  // ftop
    unsigned int v25;  // ftop
    unsigned int v27;  // 4136
    unsigned int v28;  // ftop
    unsigned int v11;  // 4101
    unsigned int v29;  // ftop
    unsigned int v30;  // ftop
    unsigned int v31;  // ftop
    unsigned int v33;  // ftop
    unsigned int v34;  // ftop
    unsigned int v35;  // ftop
    unsigned int v37;  // ftop
    unsigned int v38;  // ftop
    unsigned int v12;  // ftop
    unsigned int v40;  // 4136
    unsigned int v41;  // ftop
    unsigned int v42;  // ftop
    unsigned int v44;  // 4136
    unsigned int v45;  // ftop
    unsigned int v46;  // ftop
    unsigned int v47;  // ftop
    unsigned int v48;  // ftop
    unsigned int v13;  // ftop
    unsigned short v50;  // ftop
    unsigned short v51;  // ftop
    unsigned short v52;  // ftop
    unsigned short v54;  // ftop
    unsigned short v55;  // ftop
    unsigned short v56;  // ftop
    unsigned short v58;  // ftop
    unsigned int v59;  // 4139
    unsigned short v60;  // ftop
    unsigned short v61;  // ftop
    unsigned int v63;  // 4139
    unsigned int v15;  // ftop
    unsigned int v16;  // 4689
    unsigned int v17;  // ftop
    unsigned int v18;  // ftop
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    unsigned int v5;  // [bp-0xc]
    unsigned int v6;  // [bp-0x8]
    unsigned int v7;  // [bp-0x4]

    v10 = *((int *)&v9[1].padding_0[0]);
    if (v10 == v9->field_14)
        return;
    if ((v10 & 0x80000003) < 0)
    {
        v11 = _ccall(4, 18, ((v10 & 0x80000003) - 1 | 0xfffffffc) + 1, 0, 0);
        if (!(v11 & 1))
            return;
    }
    else if (v10 & 0x80000003)
    {
        return;
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
    v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v13 = v12 - 3;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v15 = v13 - 2;
    /* unsupported instruction */
    /* unsupported instruction */
    v16 = _ccall(10, 13, (char)((((unsigned short)v13 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
    if (v16 & 1)
    {
        v17 = v15 - 1;
        if (/* unsupported instruction */)
        {
            v18 = v17 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v18 = v17 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        v15 = v18 + 1;
        if ((char)((((unsigned short)v15 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
        {
            v20 = v15 - 1;
            if (/* unsupported instruction */)
            {
                v21 = v20 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v21 = v20 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            v15 = v21 + 1;
            v23 = _ccall(10, 13, (char)((((unsigned short)v15 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            if (v23 & 1)
            {
                v24 = v15 - 1;
                if (/* unsupported instruction */)
                {
                    v25 = v24 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v25 = v24 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                /* unsupported instruction */
                v15 = v25 + 1;
                v27 = _ccall(10, 13, (char)((((unsigned short)v15 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            }
        }
    }
    v28 = v15 - 1;
    if (/* unsupported instruction */)
    {
        v29 = v28 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v29 = v28 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v30 = v29 - 1;
    if (/* unsupported instruction */)
    {
        v31 = v30 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v31 = v30 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
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
    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v33 = v31 + 2;
    if ((char)((((unsigned short)v33 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
    {
        v34 = v33 - 1;
        if (/* unsupported instruction */)
        {
            v35 = v34 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v35 = v34 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        v33 = v35 + 1;
        if ((char)((((unsigned short)v33 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
        {
            v37 = v33 - 1;
            if (/* unsupported instruction */)
            {
                v38 = v37 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v38 = v37 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            v33 = v38 + 1;
            v40 = _ccall(10, 13, (char)((((unsigned short)v33 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            if (v40 & 1)
            {
                v41 = v33 - 1;
                if (/* unsupported instruction */)
                {
                    v42 = v41 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v42 = v41 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                /* unsupported instruction */
                v33 = v42 + 1;
                v44 = _ccall(10, 13, (char)((((unsigned short)v33 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            }
        }
    }
    v45 = v33 - 1;
    if (/* unsupported instruction */)
    {
        v46 = v45 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v46 = v45 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v47 = v46 - 1;
    if (/* unsupported instruction */)
    {
        v48 = v47 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v48 = v47 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
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
    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v50 = (unsigned short)v48 + 3;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    if ((char)(((v50 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
    {
        v51 = v50 - 0;
        if (/* unsupported instruction */)
        {
            v52 = v51 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v52 = v51 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        v54 = v52 + 1;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)(((v54 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            goto LABEL_407576;
        v55 = v54 - 0;
        if (/* unsupported instruction */)
        {
            v56 = v55 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v56 = v55 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v58 = v56 + 2;
        v59 = _ccall(10, 13, (char)(((v58 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (v59 & 1)
        {
            v60 = v58 - 1;
            if (/* unsupported instruction */)
            {
                v61 = v60 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v61 = v60 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            v63 = _ccall(10, 13, (char)(((v61 + 2 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            return;
        }
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
LABEL_407576:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    return;
}
