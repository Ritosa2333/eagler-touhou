// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4227c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4227c0_11 {
    char padding_0[1428];
    unsigned int field_594;
} st_4227c0_11;

typedef struct st_4227c0_2 {
    char padding_0[4];
    unsigned int field_4;
} st_4227c0_2;

typedef struct st_4227c0_8 {
    char padding_0[52];
    int field_34;
} st_4227c0_8;

typedef struct st_4227c0_10 {
    char padding_0[16];
    unsigned int field_10;
} st_4227c0_10;

typedef struct st_4227c0_0 {
    char padding_0[27752];
    int field_6c68;
    char padding_6c6c[172];
    unsigned int field_6d18;
    char padding_6d1c[4];
    int field_6d20;
} st_4227c0_0;

typedef struct st_4227c0_4 {
    char padding_0[8];
    struct st_4227c0_2 *field_8;
    struct st_4227c0_2 *field_c;
} st_4227c0_4;

typedef struct st_4227c0_1 {
    char padding_0[13756];
    char field_35bc;
} st_4227c0_1;

typedef struct st_4227c0_9 {
    char padding_0[102328];
    int field_18fb8;
} st_4227c0_9;

extern unsigned int g_4b0c60;
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern unsigned int g_4b0cbc;
extern unsigned int g_4b0cc0;
extern char g_4b0ce0;
extern st_4227c0_9 *g_4b43b8;
extern st_4227c0_1 *g_4b43bc;
extern st_4227c0_4 *g_4b43d4;
extern int g_4b43dc;
extern st_4227c0_0 *g_4b43e4;
extern st_4227c0_10 *g_4b4518;
extern unsigned int g_4b451c;
extern st_4227c0_4 *g_4b4524;
extern st_4227c0_8 *g_4b452c;
extern unsigned int g_4cee40;
extern unsigned int g_4cee78;
extern unsigned int g_4cf468;
extern unsigned int g_4d48b8;

unsigned int sub_4227c0(unsigned int a0)
{
    unsigned int v1;  // esi
    void* idx;  // edi
    char v3;  // al
    unsigned int v4;  // ecx
    st_4227c0_11 *v5;  // eax
    unsigned int v0;  // [bp-0x5c]

    v0 = v1;
    if (g_4b43e4->field_6d18 & 0x200 && g_4b43e4->field_6d20 < 120)
        return 1;
    if (!(int)idx[20])
    {
        sub_421430();
        if ((char)idx[96] & 8)
        {
            sub_464c40();
            g_4cee40 = ~(g_4cee78 >> 13) & 1 | 2;
            return 1;
        }
        sub_4056e0();
        if (g_4b43bc)
        {
            sub_4057e0();
            sub_405890();
            *((unsigned int *)&idx[96]) = (int)idx[96] | 0x800;
            sub_461970(g_4b43e4->field_6c68);
        }
        else
        {
            *((unsigned int *)&idx[96]) = (int)idx[96] & 0xfffff7ff;
            sub_409630();
            sub_436100();
            sub_40e8b0();
            sub_40e940(g_4b43dc);
            sub_421bf0();
            g_4b0cbc = 0;
            g_4b0cc0 = 0;
            sub_43c590();
            sub_477420(&ebx, 0, 80);
            sub_412990("main", &ebx);
            sub_41d560();
            sub_431ea0();
            sub_40ea10();
            sub_40e810();
            sub_412ee0();
            sub_40e810();
            sub_40e810();
            g_4b43d4->field_8->field_4 = g_4b43d4->field_8->field_4 | 2;
            g_4b43d4->field_c->field_4 = g_4b43d4->field_c->field_4 | 2;
            sub_40e810();
            g_4b4524->field_8->field_4 = g_4b4524->field_8->field_4 | 2;
            g_4b4524->field_c->field_4 = g_4b4524->field_c->field_4 | 2;
            sub_40d990();
            sub_44a0f0();
            if (!(g_4b0ce0 & 32))
                sub_430150(0, g_4b452c->field_34);
            sub_461970(g_4b43b8->field_18fb8);
            sub_411b00(g_4b43b8->field_18fb8);
        }
    }
    else if ((int)idx[20] == 30 && (int)idx[96] & 0x800)
    {
        *((unsigned int *)&idx[96]) = (int)idx[96] & 0xfffff7ff;
        sub_409630();
        sub_436100();
        sub_40e8b0();
        sub_40e940(g_4b43dc);
        sub_421bf0();
        g_4b0cbc = 0;
        g_4b0cc0 = 0;
        sub_43c590();
        sub_477420(&ebx, 0, 80);
        sub_412990("main", &ebx);
        sub_41d560();
        sub_431ea0();
        sub_40ea10();
        sub_40e810();
        sub_412ee0();
        sub_40e810();
        sub_40e810();
        g_4b43d4->field_8->field_4 = g_4b43d4->field_8->field_4 | 2;
        g_4b43d4->field_c->field_4 = g_4b43d4->field_c->field_4 | 2;
        sub_40e810();
        g_4b4524->field_8->field_4 = g_4b4524->field_8->field_4 | 2;
        g_4b4524->field_c->field_4 = g_4b4524->field_c->field_4 | 2;
        sub_40d990();
        sub_44a0f0();
        sub_430240();
        sub_430150(0, g_4b452c->field_34);
        sub_411b00(0, g_4b452c->field_34);
        sub_4067e0(0);
    }
    if ((int)idx[20] == 5)
        g_4cf468 = 2;
    if (g_4b43bc && g_4b43bc->field_35bc & 8 && g_4b43bc)
    {
        sub_402cc0(g_4b43bc);
        sub_46ca4f(g_4b43bc);
    }
    if ((char)(int)idx[96] & 4)
    {
        *((unsigned int *)&idx[96]) = (int)idx[96] | 128;
        return 1;
    }
    sub_464c40();
    if (g_4b0ce0 & 32)
    {
        if (g_4d48b8 & 524547 || (char)idx[96] & 112)
            g_4cee40 = (-(0 < (g_4cee78 & 0x2000)) & 0xfffffffe) + 4;
        if ((int)idx[20] == 3540)
        {
            sub_4529a0(5, 60, 0, 0, 0, 59);
        }
        else if ((int)idx[20] == 3600)
        {
            sub_40f720();
        }
    }
    sub_420e70();
    v3 = (int)idx[96];
    if (v3 & 16)
    {
        return 3;
    }
    else if (v3 & 32)
    {
        return 3;
    }
    else if (v3 & 64)
    {
        return 3;
    }
    else
    {
        if (g_4b4518->field_10 != 1)
        {
            v4 = (g_4b0c94 + g_4b0c90 * 2) * 17908;
            v5 = v4 + g_4b451c + 1428;
            if (*((int *)(v4 + g_4b451c + 1428)) < 0xcdfe5ff)
                *((unsigned int *)&v5->padding_0[0]) = *((int *)&v5->padding_0[0]) + 1;
        }
        g_4b0cbc = g_4b0cbc + 1;
        g_4b0cc0 = g_4b0cc0 + 1;
        g_4b0c60 = g_4b0c60 + 1;
        sub_464a80();
        return 1;
    }
}
