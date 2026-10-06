// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47686b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47686b_1 {
    char field_0;
    char padding_1[62];
    char field_3f;
} st_47686b_1;

typedef struct st_47686b_0 {
    struct st_47686b_1 *field_0;
    struct st_47686b_1 *field_4;
} st_47686b_0;

typedef struct SYSTEMTIME {
    unsigned short wYear;
    unsigned short wMonth;
    unsigned short wDayOfWeek;
    unsigned short wDay;
    unsigned short wHour;
    unsigned short wMinute;
    unsigned short wSecond;
    unsigned short wMilliseconds;
} SYSTEMTIME;

typedef struct TIME_ZONE_INFORMATION {
    int Bias;
    Char StandardName[32];
    SYSTEMTIME StandardDate;
    int StandardBias;
    Char DaylightName[32];
    SYSTEMTIME DaylightDate;
    int DaylightBias;
} TIME_ZONE_INFORMATION;

extern unsigned int g_4aad78;
extern unsigned int g_4adae0;
extern unsigned int g_4adaec;
extern TIME_ZONE_INFORMATION g_4b4118;
extern Char g_4b411c;
extern unsigned short g_4b415e;
extern unsigned int g_4b416c;
extern Char g_4b4170;
extern unsigned short g_4b41b2;
extern unsigned int g_4b41c0;
extern unsigned int g_4b41c4;
extern void g_4b41c8;

void sub_47686b(void)
{
    void* v2;  // ebp
    char *v3;  // esi
    int *v12;  // eax
    unsigned int *v13;  // eax
    unsigned int v4;  // eax
    unsigned int v5;  // eax
    unsigned int v6;  // eax
    st_47686b_0 *v7;  // edi
    char *iter;  // esi
    unsigned int v9;  // eax
    unsigned int v10;  // eax
    unsigned int v11;  // eax
    char *v0;  // [bp-0xc]

    sub_46fcec(&g_4aad78, 44);
    *((unsigned int *)((char *)v2 - 56)) = 0;
    memset(v2 - 44, 0, 20);
    sub_46ec9a(7);
    *((unsigned int *)((char *)v2 - 4)) = 0;
    *((unsigned int *)((char *)v2 - 32)) = sub_477413();
    if (sub_477324(v2 - 28))
        sub_470dc6(0, 0, 0, 0, 0);
    if (sub_4772b2(v2 - 36))
        sub_470dc6(0, 0, 0, 0, 0);
    if (sub_4772eb(v2 - 40))
        sub_470dc6(0, 0, 0, 0, 0);
    *((unsigned int *)((char *)v2 - 52)) = sub_484896();
    g_4b41c4 = 0;
    g_4adaec = 0xffffffff;
    g_4adae0 = 0xffffffff;
    v3 = sub_48af42("TZ");
    *((char **)((char *)v2 - 60)) = v3;
    if (!(v3 && *(v3)))
    {
        if (*((int *)&g_4b41c8))
        {
            sub_46c941(*((int *)&g_4b41c8));
            *((unsigned int *)&g_4b41c8) = 0;
        }
        if (GetTimeZoneInformation(&g_4b4118.Bias) != 0xffffffff)
        {
            g_4b41c4 = 1;
            v6 = g_4b4118.Bias * 60;
            *((unsigned int *)((char *)v2 - 28)) = v6;
            if (g_4b415e)
                *((unsigned int *)((char *)v2 - 28)) = v6 + g_4b416c * 60;
            if (g_4b41b2 && g_4b41c0)
            {
                *((unsigned int *)((char *)v2 - 36)) = 1;
                *((unsigned int *)((char *)v2 - 40)) = (g_4b41c0 - g_4b416c) * 60;
            }
            else
            {
                *((unsigned int *)((char *)v2 - 36)) = 0;
                *((unsigned int *)((char *)v2 - 40)) = 0;
            }
            if (WideCharToMultiByte(*((int *)((char *)v2 - 52)), 0, &g_4b411c, -0x1, *((int *)*((int *)((char *)v2 - 32))), 63, NULL, v2 - 48) && !*((int *)((char *)v2 - 48)))
                *((char *)(*((int *)*((int *)((char *)v2 - 32))) + 63)) = 0;
            else
                *((char *)*((int *)*((int *)((char *)v2 - 32)))) = 0;
            if (WideCharToMultiByte(*((int *)((char *)v2 - 52)), 0, &g_4b4170, -0x1, *((int *)(*((int *)((char *)v2 - 32)) + 4)), 63, NULL, v2 - 48) && !*((int *)((char *)v2 - 48)))
                *((char *)(*((int *)(*((int *)((char *)v2 - 32)) + 4)) + 63)) = 0;
            else
                *((char *)*((int *)(*((int *)((char *)v2 - 32)) + 4))) = 0;
        }
    }
    else if (!*((int *)&g_4b41c8))
    {
LABEL_47694e:
        v4 = sub_475860(v3);
        *((unsigned int *)&g_4b41c8) = sub_4777ce(v4 + 1);
        if (*((int *)&g_4b41c8))
        {
            v0 = v3;
            v5 = sub_475860(v3);
            if (sub_47a2b9(*((int *)&g_4b41c8), v5 + 1))
            {
                sub_470dc6(0, 0, 0, 0, 0);
                goto LABEL_476a8e;
            }
        }
    }
    else if (sub_472ac0(v3, *((int *)&g_4b41c8)) && *((int *)&g_4b41c8))
    {
        sub_46c941(*((int *)&g_4b41c8));
        goto LABEL_47694e;
    }
    *((unsigned int *)((char *)v2 - 44)) = 1;
LABEL_476a8e:
    sub_47685a(*((int *)((char *)v2 - 28)));
    sub_476838(*((int *)((char *)v2 - 36)));
    sub_476849(*((int *)((char *)v2 - 40)));
    *((unsigned int *)((char *)v2 - 4)) = 0xfffffffe;
    sub_476b17();
    if (!*((int *)((char *)v2 - 44)))
    {
        v7 = *((int *)((char *)v2 - 32));
        if (sub_4830fd(v7->field_0, 64, v3, 3))
            sub_470dc6(0, 0, 0, 0, 0);
        iter = v3 + 3;
        if (*(iter) == 45)
        {
            *((unsigned int *)((char *)v2 - 56)) = 1;
            iter += 1;
        }
        for (*((unsigned int *)((char *)v2 - 28)) = sub_46d114(iter) * 3600; *(iter) == 43 || *(iter) >= 48 && *(iter) <= 57; iter += 1);
        if (*(iter) == 58)
        {
            v9 = sub_46d114(iter + 1);
            for (*((unsigned int *)((char *)v2 - 28)) = *((int *)((char *)v2 - 28)) + v9 * 60; *(iter) >= 48 && *(iter) <= 57; iter += 1);
            if (*(iter) == 58)
            {
                v10 = sub_46d114(iter + 1);
                for (*((unsigned int *)((char *)v2 - 28)) = *((int *)((char *)v2 - 28)) + v10; *(iter) >= 48 && *(iter) <= 57; iter += 1);
            }
        }
        if (*((int *)((char *)v2 - 56)))
            *((int *)((char *)v2 - 28)) = -(*((int *)((char *)v2 - 28)));
        v11 = *(iter);
        *((unsigned int *)((char *)v2 - 36)) = v11;
        if (!v11)
        {
            v7->field_4->field_0 = 0;
        }
        else if (sub_4830fd(v7->field_4, 64, iter, 3))
        {
            sub_470dc6(0, 0, 0, 0, 0);
        }
        v12 = sub_47740d();
        *(v12) = *((int *)((char *)v2 - 28));
        v13 = sub_477401();
        *(v13) = *((int *)((char *)v2 - 36));
    }
    sub_46fd31();
    return;
}
