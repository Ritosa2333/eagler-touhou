// AUTOMATIC PSEUDOCODE; NOT ORIGINAL SOURCE
// ecl_high_dispatch 0x4154d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417

typedef struct st_4154d0_44 {
    struct st_4154d0_42 *field_0;
    char padding_4[124];
    unsigned int field_80;
} st_4154d0_44;

typedef struct st_4154d0_28 {
    char padding_0[224];
    unsigned int field_e0;
} st_4154d0_28;

typedef struct st_4154d0_42 {
    char padding_0[20];
    struct st_4154d0_43 *field_14;
} st_4154d0_42;

typedef struct st_4154d0_53 {
    char padding_0[304];
    unsigned int field_130;
} st_4154d0_53;

typedef struct st_4154d0_3 {
    char padding_0[4];
    unsigned short field_4;
    char padding_6[18];
    char field_18;
    char padding_19[3];
    unsigned int field_1c;
} st_4154d0_3;

typedef struct st_4154d0_43 {
    unsigned int field_0;
} st_4154d0_43;

typedef struct st_4154d0_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    char padding_c[36];
    unsigned int field_30;
} st_4154d0_0;

typedef struct st_4154d0_7 {
    char padding_0[48];
    unsigned int field_30;
    char padding_34[5764];
    unsigned int field_16b8;
} st_4154d0_7;

typedef struct st_4154d0_40 {
    char padding_0[4];
    short field_4;
} st_4154d0_40;

typedef struct st_4154d0_5 {
    char padding_0[4];
    struct st_4154d0_4 *field_4;
} st_4154d0_5;

typedef struct st_4154d0_4 {
    unsigned int field_0;
    struct st_4154d0_3 *field_4;
} st_4154d0_4;

typedef struct st_4154d0_47 {
    char padding_0[104];
    unsigned int field_68;
} st_4154d0_47;

typedef struct st_4154d0_48 {
    char padding_0[1136];
    unsigned int field_470;
} st_4154d0_48;

typedef struct st_4154d0_29 {
    char padding_0[44];
    unsigned int field_2c;
    char padding_30[1100];
    unsigned int field_47c;
} st_4154d0_29;

typedef struct st_4154d0_30 {
    char padding_0[64];
    unsigned int field_40;
    unsigned int field_44;
    char padding_48[1076];
    unsigned int field_47c;
} st_4154d0_30;

typedef struct st_4154d0_16 {
    char padding_0[44];
    unsigned int field_2c;
    char padding_30[1024];
    void* field_430;
    unsigned int field_434;
    struct st_4154d0_7 *field_438;
    char padding_43c[64];
    unsigned int field_47c;
} st_4154d0_16;

typedef struct st_4154d0_6 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    void* field_430;
    unsigned int field_434;
    struct st_4154d0_7 *field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_4154d0_6;

typedef struct st_4154d0_31 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_4154d0_31;

typedef struct st_4154d0_46 {
    char padding_0[112];
    unsigned int field_70;
} st_4154d0_46;

typedef struct st_4154d0_36 {
    char padding_0[1164];
    unsigned int field_48c;
} st_4154d0_36;

typedef struct st_4154d0_45 {
    char padding_0[116];
    unsigned int field_74;
} st_4154d0_45;

typedef struct st_4154d0_21 {
    void* field_0;
    unsigned int field_4;
    struct st_4154d0_7 *field_8;
} st_4154d0_21;

typedef struct LIST_ENTRY {
    struct LIST_ENTRY *Flink;
    struct LIST_ENTRY *Blink;
} LIST_ENTRY;

typedef struct CRITICAL_SECTION_DEBUG {
    unsigned short Type;
    unsigned short CreatorBackTraceIndex;
    struct CRITICAL_SECTION *CriticalSection;
    LIST_ENTRY ProcessLocksList;
    unsigned int EntryCount;
    unsigned int ContentionCount;
    unsigned int Flags;
    unsigned short CreatorBackTraceIndexHigh;
    unsigned short Identifier;
} CRITICAL_SECTION_DEBUG;

typedef struct st_4154d0_2 {
    char padding_0[4212];
    unsigned int field_1074;
    unsigned int field_1078;
    unsigned int field_107c;
} st_4154d0_2;

typedef struct CRITICAL_SECTION {
    struct CRITICAL_SECTION_DEBUG *DebugInfo;
    int LockCount;
    int RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    UIntPtr SpinCount;
} CRITICAL_SECTION;

typedef struct st_4154d0_39 {
    char padding_0[27892];
    unsigned int field_6cf4;
    char padding_6cf8[56];
    unsigned int field_6d30;
} st_4154d0_39;

typedef struct st_4154d0_1 {
    char padding_0[28];
    struct st_4154d0_2 *field_1c;
    char padding_20[28];
    unsigned int field_3c;
} st_4154d0_1;

typedef struct st_4154d0_41 {
    char padding_0[124];
    unsigned int field_7c;
} st_4154d0_41;

typedef struct st_4154d0_38 {
    char padding_0[96];
    unsigned int field_60;
} st_4154d0_38;

extern char g_400000;
extern unsigned int g_4976ce;
extern unsigned int g_4ad138;
extern unsigned int g_4af0fc[4];
extern unsigned int g_4af118[4];
extern unsigned int g_4af120[4];
extern unsigned int g_4b0ca8;
extern int g_4b0ccc;
extern int g_4b0e88;
extern unsigned int g_4b2ed0;
extern st_4154d0_38 *g_4b43c8;
extern st_4154d0_41 *g_4b43cc;
extern st_4154d0_1 *g_4b43dc;
extern st_4154d0_39 *g_4b43e4;
extern unsigned int g_4ce8d0;
extern unsigned int g_4ce8d4;
extern unsigned int g_4ce8d8;
extern unsigned int g_4ce8dc;
extern unsigned int g_4ce8e0;
extern int g_4ceb38;
extern int g_4ceb78;
extern int g_4cebb8;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

int sub_4154d0(void* a0)
{
    unsigned long v82;  // ldt
    unsigned long v83;  // gdt
    unsigned short v92;  // ax
    unsigned int v173;  // 4237
    unsigned int v175;  // ftop
    unsigned int v176;  // ftop
    unsigned int v178;  // ecx
    unsigned int v179;  // ecx
    void* idx;  // esi
    unsigned short v181;  // ftop
    void* idx1;  // edi
    unsigned int v184;  // eax
    unsigned int iter;  // ftop
    unsigned int *idx2;  // esi
    unsigned int v188;  // ftop
    unsigned short v190;  // ftop
    unsigned int v94;  // ecx
    unsigned short v191;  // ftop
    unsigned short v193;  // ftop
    unsigned int v194;  // 4154
    unsigned short v195;  // ftop
    unsigned short v196;  // ftop
    unsigned short v198;  // ftop
    unsigned short v200;  // ftop
    void* v202;  // edi
    unsigned int v203;  // ecx
    unsigned int v204;  // edx
    unsigned short v205;  // ftop
    unsigned int v207;  // 4158
    unsigned short v208;  // ftop
    unsigned short v209;  // ftop
    void* v95;  // esi
    unsigned short v211;  // ftop
    unsigned short v212;  // ftop
    unsigned short v214;  // ftop
    void* v216;  // edi
    unsigned int v217;  // ftop
    unsigned int v96;  // ecx
    unsigned int v219;  // 4178
    unsigned int v220;  // ftop
    unsigned int v223;  // ebx
    void* v224;  // eax
    void* v225;  // edi
    unsigned int v226;  // ftop
    unsigned int *iter1;  // edi
    unsigned int v228;  // 4174
    unsigned int v229;  // ftop
    unsigned int v230;  // ftop
    unsigned int v232;  // ftop
    unsigned int v233;  // ftop
    void* v234;  // esi
    unsigned int v235;  // ecx
    unsigned int v236;  // edx
    unsigned int v237;  // ftop
    unsigned int v239;  // 4158
    unsigned short v240;  // ftop
    unsigned short v241;  // ftop
    unsigned short v243;  // ftop
    unsigned short v244;  // ftop
    unsigned short v246;  // ftop
    unsigned short v247;  // ftop
    unsigned int v98;  // edx
    unsigned short v249;  // ftop
    unsigned short v250;  // ftop
    unsigned short v252;  // ftop
    void* v254;  // edi
    unsigned int v255;  // ftop
    unsigned int v257;  // 4174
    void* v99;  // esi
    unsigned int v258;  // ftop
    unsigned int v260;  // ftop
    unsigned int v262;  // ftop
    void* v265;  // esi
    unsigned int v266;  // eax
    unsigned int v267;  // ecx
    unsigned short v84;  // fs
    unsigned int v100;  // ecx
    unsigned int v268;  // edx
    unsigned int v269;  // eax
    void* v270;  // ecx
    void* v271;  // eax
    void* v272;  // edi
    unsigned int v274;  // ftop
    unsigned int v275;  // ftop
    unsigned int v277;  // ftop
    unsigned int *node;  // edi
    unsigned int v279;  // ftop
    unsigned int v280;  // ftop
    unsigned int v282;  // ftop
    unsigned int v283;  // ftop
    unsigned int v285;  // ftop
    unsigned short v287;  // ftop
    unsigned int v288;  // 4280
    unsigned int v290;  // 4143
    unsigned short v292;  // ftop
    unsigned int v293;  // 4235
    unsigned int v295;  // eax
    unsigned int v102;  // eax
    unsigned int v296;  // eax
    unsigned int v297;  // edi
    unsigned int v298;  // edi
    unsigned int v299;  // eax
    unsigned int v300;  // edi
    unsigned int v301;  // edi
    unsigned int v302;  // eax
    unsigned int v303;  // eax
    unsigned int v304;  // 4098
    void* v103;  // esi
    unsigned int v305;  // eax
    unsigned int v306;  // ecx
    int v307;  // esi
    st_4154d0_5 *v308;  // eax
    unsigned int *v312;  // eax
    st_4154d0_4 *v313;  // eax
    unsigned int v314;  // edi
    unsigned int v104;  // ecx
    void* v315;  // esi
    void* v316;  // eax
    unsigned int v317;  // eax
    st_4154d0_36 *v318;  // esi
    st_4154d0_36 *iter2;  // edi
    unsigned int v320;  // ecx
    void* v321;  // eax
    void* v322;  // ebx
    unsigned int v323;  // fc3210
    void* v324;  // eax
    unsigned int *v105;  // edi
    void* v325;  // esi
    unsigned int v326;  // esi
    unsigned int v327;  // esi
    unsigned int v328;  // eax
    unsigned int v329;  // esi
    void* v330;  // edi
    unsigned int v332;  // 4169
    void* v333;  // esi
    void* v334;  // esi
    void* v335;  // esi
    unsigned int v336;  // eax
    unsigned int v337;  // esi
    void* v339;  // esi
    void* v340;  // esi
    unsigned int v341;  // eax
    void* v342;  // eax
    unsigned int v343;  // eax
    unsigned int v344;  // eax
    void* v106;  // esi
    int v345;  // cc_dep1
    int v346;  // cc_dep2
    unsigned short v347;  // ax
    unsigned int v348;  // eax
    unsigned int v349;  // edi
    unsigned int v350;  // eax
    unsigned int v351;  // esi
    unsigned int v352;  // eax
    unsigned int v353;  // eax
    unsigned int v354;  // eax
    unsigned int v107;  // ecx
    void* v355;  // esi
    unsigned int v356;  // eax
    unsigned int v357;  // edi
    unsigned int *v358;  // esi
    void* v359;  // esi
    void* v360;  // esi
    int v361;  // esi
    unsigned int v362;  // eax
    unsigned int v363;  // eax
    unsigned long long v85;  // 4217
    unsigned int *v108;  // edi
    unsigned int v365;  // edi
    char v366;  // cl
    char *v367;  // edx
    unsigned int v369;  // eax
    unsigned int v371;  // eax
    void* v373;  // esi
    unsigned int v374;  // ecx
    unsigned int *v375;  // edi
    void* v377;  // esi
    unsigned int v378;  // ecx
    void* v380;  // esi
    unsigned int v381;  // ecx
    unsigned int *v382;  // edi
    void* v384;  // esi
    unsigned int v385;  // ecx
    void* v387;  // esi
    unsigned int v388;  // ecx
    unsigned int *v389;  // edi
    void* v391;  // esi
    unsigned int v392;  // ecx
    st_4154d0_44 *v393;  // esi
    unsigned int *v394;  // esi
    unsigned int v109;  // edx
    st_4154d0_45 *v395;  // esi
    st_4154d0_46 *v396;  // esi
    st_4154d0_47 *v397;  // esi
    st_4154d0_48 *v398;  // esi
    unsigned int *v399;  // eax
    unsigned int *v400;  // eax
    void* v110;  // esi
    unsigned int *v401;  // eax
    unsigned int *v402;  // eax
    unsigned int *v403;  // eax
    unsigned int *v404;  // eax
    unsigned int *v405;  // eax
    unsigned int *v406;  // eax
    unsigned int *v407;  // eax
    unsigned int *v408;  // eax
    unsigned int *v409;  // eax
    unsigned int v410;  // edi
    unsigned int v111;  // ecx
    unsigned int v411;  // eax
    unsigned int v412;  // eax
    unsigned int v413;  // eax
    unsigned int v414;  // eax
    unsigned int v415;  // eax
    unsigned int v417;  // fc3210
    unsigned int v418;  // 4144
    unsigned int *v112;  // edi
    unsigned int v419;  // eax
    unsigned int v420;  // eax
    unsigned long long v421;  // 4120
    unsigned int v113;  // eax
    unsigned int v114;  // eax
    void* v86;  // ebx
    int v116;  // eax
    st_4154d0_28 *v117;  // esi
    st_4154d0_29 *v118;  // esi
    st_4154d0_30 *v119;  // esi
    unsigned int v120;  // esi
    void* v121;  // esi
    unsigned int v122;  // eax
    unsigned int v123;  // eax
    st_4154d0_53 *v124;  // esi
    st_4154d0_31 *v125;  // ebx
    st_4154d0_0 *v87;  // esi
    unsigned int v126;  // eax
    st_4154d0_53 *v127;  // esi
    void* v128;  // edi
    st_4154d0_6 *v129;  // ebx
    unsigned int *v130;  // ecx
    unsigned int v131;  // eax
    st_4154d0_53 *v132;  // esi
    st_4154d0_31 *v133;  // ebx
    int v134;  // eax
    unsigned int v135;  // eax
    unsigned int v88;  // edi
    unsigned int v137;  // eax
    st_4154d0_53 *v138;  // esi
    void* v139;  // edi
    st_4154d0_6 *v140;  // ebx
    unsigned int v141;  // eax
    st_4154d0_53 *v142;  // esi
    unsigned int *v143;  // ecx
    unsigned int v144;  // esi
    st_4154d0_16 *v145;  // esi
    unsigned long long v89;  // 4221
    st_4154d0_21 *v146;  // eax
    st_4154d0_28 *v147;  // esi
    unsigned int v148;  // ecx
    unsigned int v149;  // ecx
    int v150;  // edx
    unsigned int v151;  // eax
    unsigned int v152;  // eax
    st_4154d0_28 *v153;  // esi
    unsigned int v154;  // eax
    st_4154d0_28 *v155;  // esi
    void* v90;  // ebx
    unsigned int v156;  // eax
    unsigned int v157;  // eax
    void* v158;  // edi
    unsigned short v159;  // ftop
    unsigned int v161;  // 4174
    unsigned short v162;  // ftop
    void* v164;  // edi
    st_4154d0_3 *v91;  // edi
    void* index;  // edi
    unsigned int v166;  // ftop
    unsigned int v167;  // fc3210
    unsigned int v168;  // 4186
    void* v170;  // edi
    unsigned int v171;  // ftop
    unsigned int v172;  // fc3210
    char *v0;  // [bp-0x350]
    int v1;  // [bp-0x344]
    unsigned int v2;  // [bp-0x340]
    unsigned int v3;  // [bp-0x33c]
    unsigned int v4;  // [bp-0x338]
    unsigned int v5;  // [bp-0x334]
    unsigned int v6;  // [bp-0x330]
    unsigned int v7;  // [bp-0x32c]
    unsigned int v8;  // [bp-0x328]
    unsigned int v9;  // [bp-0x324]
    st_4154d0_0 *v10;  // [bp-0x320], Other Possible Types: int, unsigned int
    void* v11;  // [bp-0x31c], Other Possible Types: unsigned int
    unsigned int *v12;  // [bp-0x318], Other Possible Types: void*, unsigned int
    int <0x4154d0[is_4]|Stack bp-0x318, 1 B>;  // [bp-0x318]
    st_4154d0_40 *v13;  // [bp-0x314], Other Possible Types: void*, char, unsigned int
    st_4154d0_3 *v14;  // [bp-0x310], Other Possible Types: void*, unsigned int
    char *v15;  // [bp-0x30c], Other Possible Types: void*, unsigned int
    void* v16;  // [bp-0x308], Other Possible Types: unsigned int
    void* v17;  // [bp-0x304], Other Possible Types: unsigned int *, unsigned long, unsigned int
    void* v18;  // [bp-0x300], Other Possible Types: unsigned int
    unsigned int *v19;  // [bp-0x2fc], Other Possible Types: unsigned int
    unsigned int v20;  // [bp-0x2f8]
    void* v21;  // [bp-0x2f4], Other Possible Types: unsigned int
    void* v22;  // [bp-0x2f0], Other Possible Types: unsigned int
    void* v23;  // [bp-0x2ec], Other Possible Types: unsigned int
    unsigned int v24;  // [bp-0x2e8]
    void* v25;  // [bp-0x2e4], Other Possible Types: unsigned int
    unsigned int v26;  // [bp-0x2e0]
    char v27;  // [bp-0x2dd]
    unsigned int v28;  // [bp-0x2dc]
    void* v29;  // [bp-0x2d8], Other Possible Types: unsigned int
    unsigned int v30;  // [bp-0x2d4]
    void* v31;  // [bp-0x2d0], Other Possible Types: unsigned int
    unsigned long v32;  // [bp-0x2cc], Other Possible Types: unsigned int
    void* v33;  // [bp-0x2c8], Other Possible Types: unsigned int
    unsigned int v34;  // [bp-0x2c4]
    char v35;  // [bp-0x2c0]
    char v36;  // [bp-0x2b8]
    char v37;  // [bp-0x2b4]
    char v38;  // [bp-0x2a4], Other Possible Types: unsigned int
    unsigned int v39;  // [bp-0x2a0]
    unsigned int v40;  // [bp-0x29c]
    unsigned int v41;  // [bp-0x298]
    unsigned int v42;  // [bp-0x294]
    unsigned int v43;  // [bp-0x290]
    unsigned short v44;  // [bp-0x28c], Other Possible Types: unsigned int
    unsigned short v45;  // [bp-0x28a]
    unsigned int v46;  // [bp-0x288]
    unsigned int v47;  // [bp-0x284]
    unsigned short v48;  // [bp-0x280], Other Possible Types: unsigned int
    unsigned short v49;  // [bp-0x27e]
    unsigned long v50;  // [bp-0x27c], Other Possible Types: unsigned int
    unsigned int v51;  // [bp-0x278]
    char v52;  // [bp-0x274], Other Possible Types: unsigned int
    unsigned int v53;  // [bp-0x270]
    unsigned int v54;  // [bp-0x26c]
    unsigned int v55;  // [bp-0x268]
    unsigned int v56;  // [bp-0x264]
    unsigned int v57;  // [bp-0x260]
    unsigned int v58;  // [bp-0x25c]
    unsigned int v59;  // [bp-0x254], Other Possible Types: unsigned short
    unsigned short v60;  // [bp-0x252]
    char v61;  // [bp-0x24c]
    char v62;  // [bp-0xd0], Other Possible Types: unsigned int
    unsigned int v63;  // [bp-0xcc]
    unsigned int v64;  // [bp-0xc8]
    unsigned int v65;  // [bp-0xc4]
    unsigned int v66;  // [bp-0xc0]
    unsigned int v67;  // [bp-0xbc]
    char v68;  // [bp-0xb8]
    unsigned int v69;  // [bp-0xb4]
    char v70;  // [bp-0xb0], Other Possible Types: unsigned int
    char v71;  // [bp-0xac], Other Possible Types: unsigned int
    unsigned int v72;  // [bp-0xa8]
    unsigned int v73;  // [bp-0xa4]
    char v74;  // [bp-0xa0], Other Possible Types: unsigned int
    unsigned short v422;  // [bp-0x9c], Other Possible Types: unsigned int
    char v75;  // [bp-0x98]
    unsigned int v76;  // [bp-0x34]
    unsigned int v77;  // [bp-0x18]
    unsigned int v78;  // [bp-0x10]
    unsigned int v79;  // [bp-0xc]
    unsigned int v80;  // [bp-0x8]

    v80 = 0xffffffff;
    v79 = &g_4976ce;
    v85 = _ccall(v82, v83, v84, 0);
    v78 = *((int *)(unsigned int)v85);
    v77 = g_4ad138 ^ &<0x4154d0[is_4]|Stack bp-0x318, 1 B>;
    v11 = v86;
    v10 = v87;
    v9 = v88;
    v8 = g_4ad138 ^ &v9;
    v89 = _ccall(v82, v83, v84, 0);
    *((unsigned int **)(unsigned int)v89) = &v78;
    v90 = a0;
    v91 = *((int *)(*((int *)((int)v90[5964] + 4)) + 4));
    v92 = v91->field_4;
    v21 = v90;
    v14 = v91;
    switch (v92)
    {
    case 256: case 280:
LABEL_41555d:
        sub_477420(&v74 + 4, 0, 80);
        /* unsupported instruction */
        /* unsupported instruction */
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v7 = v179;
        /* unsupported instruction */
        if (!((int)v90[5816] & 0x2000000))
        {
            sub_41a9b0(1, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            /* unsupported instruction */
            /* unsupported instruction */
            v5 = v179;
            v73 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_41a9b0(2, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            /* unsupported instruction */
            /* unsupported instruction */
            v72 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        else
        {
            sub_41a9b0(1, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            /* unsupported instruction */
            /* unsupported instruction */
            v5 = v179;
            v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_41a9b0(2, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            /* unsupported instruction */
            /* unsupported instruction */
            v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_402410(2, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v1 = &g_4ceb78;
            sub_46c6d4(&v71, &v13, &g_4cebb8, &g_4ceb78, &g_4ceb38, &g_4b0e88);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v90 = v11;
            v65 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v66 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v67 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        v69 = sub_41a970(3);
        v66 = sub_41a970(4);
        v95 = v90 + 572;
        v96 = 12;
        iter1 = &v70;
        for (v66 = sub_41a970(5); v96; v95 += 4)
        {
            v96 -= 1;
            *(iter1) = *((int *)v95);
            iter1 += 1;
        }
        goto LABEL_4156cc;
    case 258:
        *((unsigned int *)&v90[544]) = sub_468e20(0);
        goto LABEL_41962b;
    case 259:
        v12 = sub_468e20(0);
        v116 = sub_468e20(1);
        v117 = v90 + v12 * 4 + 224;
        sub_461a70(*((int *)((char *)v90 + 4 * v12 + 224)));
        *((unsigned int *)&v117->padding_0[0]) = 0;
        if (v116 >= 0)
        {
            *((int *)&v117->padding_0[0]) = *((int *)sub_41c6a0(&v36, sub_468e20(1)));
            *((unsigned int *)&v90[552]) = sub_468e20(1);
            *((int *)&v90[548]) = (int)v90[544];
            sub_461c50();
            if (0)
                goto LABEL_415bc0;
            goto LABEL_415bd8;
        }
        break;
    case 260:
LABEL_4158bd:
        sub_477420(&v74 + 4, 0, 80);
        /* unsupported instruction */
        /* unsupported instruction */
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_41a9b0(1, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        v5 = v179;
        v73 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_41a9b0(2, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        v72 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v422 = sub_41a970(3);
        v72 = sub_41a970(4);
        v1 = 5;
        v106 = v90 + 572;
        v107 = 12;
        v108 = &v75;
        for (v72 = sub_41a970(5); v107; v106 += 4)
        {
            v107 -= 1;
            *(v108) = *((int *)v106);
            v108 += 1;
        }
        v74 = 1;
LABEL_4156cc:
        sub_412990(v1 + 20, &v62);
        goto LABEL_41962b;
    case 261:
LABEL_415997:
        sub_477420(&v74 + 4, 0, 80);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_41a9b0(1, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        v73 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v5 = v179;
        /* unsupported instruction */
        sub_41a9b0(2, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        v72 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v422 = sub_41a970(3);
        v72 = sub_41a970(4);
        v109 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v72 = sub_41a970(5);
        v110 = v90 + 572;
        v111 = 12;
        for (v112 = &v75; v111; v110 += 4)
        {
            v111 -= 1;
            *(v112) = *((int *)v110);
            v112 += 1;
        }
        v74 = 1;
        sub_412990(v109 + 20, &v68);
        goto LABEL_41962b;
    case 262:
        v12 = sub_468e20(0);
        v147 = v90 + v12 * 4 + 224;
        v10 = sub_468e20(1);
        sub_461a70(*((int *)((char *)v90 + 4 * v12 + 224)));
        *((unsigned int *)&v147->padding_0[0]) = 0;
        *((int *)&v147->padding_0[0]) = *((int *)sub_41c6a0(&v35, v10));
        sub_461c50(&v35, v10);
        if (!v8)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v90[5612]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v90[5616]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        if ((char)v90[5816] & 32)
            sub_461d80();
        if (!v8)
        {
            v148 = (int)v90[544];
            *((unsigned int *)&v90[5816]) = (int)v90[5816] | 0x80000;
            *((unsigned int *)&v90[556]) = 0;
            *((unsigned int *)&v90[560]) = 0;
            *((int *)&v90[552]) = 0;
            *((unsigned int *)&v90[548]) = v148;
            goto LABEL_41962b;
        }
        break;
    case 263:
        if (!((int)v90[5816] & 0x2000000))
        {
            v11 = sub_468e20(1);
            v123 = sub_468e20(0);
            v124 = *((int *)&g_4b43dc[1].padding_0[4 * v123]);
            if (g_4cee78 & 0x8000)
            {
                EnterCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 + 1;
            }
            v124->field_130 = v124->field_130 + 1;
            v125 = sub_4621c0();
            v125->field_480 = v125->field_480 | 1;
            v125->field_20 = 9;
            if (v90 == 0xffffffcc)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v125->field_430 = v15;
                v125->field_434 = v16;
                v125->field_438 = v17;
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v125->field_430 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v125->field_434 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v125->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            sub_454d10(v125, v10);
            sub_4612d0(v125, v10);
            if (g_4cee78 & 0x8000)
            {
                LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 - 1;
                goto LABEL_41962b;
            }
        }
        else
        {
            v11 = sub_468e20(1);
            v126 = sub_468e20(0);
            v127 = *((int *)&g_4b43dc[1].padding_0[4 * v126]);
            v128 = v90 + 52;
            if (g_4cee78 & 0x8000)
            {
                EnterCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 + 1;
            }
            v127->field_130 = v127->field_130 + 1;
            v129 = sub_4621c0();
            v129->field_480 = v129->field_480 | 1;
            v129->field_20 = 9;
            if (!v128)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v130 = v17;
                v129->field_430 = v15;
                v129->field_434 = v16;
            }
            else
            {
                v129->field_430 = *((int *)v128);
                v129->field_434 = (int)v128[4];
                v130 = (int)v128[8];
            }
            v129->field_438 = v130;
            sub_454d10(v129, v10);
            sub_4612d0(v129, v10);
            goto LABEL_415ede;
        }
    case 264:
        sub_468e20(0);
        sub_41c6a0(&v13, sub_468e20(1));
        goto LABEL_41962b;
    case 265:
        if (!g_4b43dc->field_1c)
            goto LABEL_41555d;
        goto LABEL_41962b;
    case 266:
        if (!g_4b43dc->field_1c)
            goto LABEL_4156f6;
        goto LABEL_41962b;
    case 267:
        if (!g_4b43dc->field_1c)
            goto LABEL_4158bd;
        goto LABEL_41962b;
    case 268:
        if (!g_4b43dc->field_1c)
            goto LABEL_415997;
        goto LABEL_41962b;
    case 269:
        v152 = sub_468e20(0);
        v153 = v90 + v152 * 4 + 224;
        v11 = v152;
        sub_461a70(*((int *)((char *)v90 + 4 * v11 + 224)));
        *((unsigned int *)&v153->padding_0[0]) = 0;
        *((int *)&v153->padding_0[0]) = *((int *)sub_41c6a0(&v35, (int)v90[556] + 5));
        sub_461c50(&v35, (int)v90[556] + 5);
        if (!v8)
            goto LABEL_415bc0;
        goto LABEL_415bd8;
    case 270:
LABEL_4157c3:
        sub_477420(&v74 + 4, 0, 80);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_41a9b0(1, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        v5 = v179;
        v73 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_41a9b0(2, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        v3 = v179;
        v72 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_41a9b0(3, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        v71 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v73 = sub_41a970(4);
        v70 = sub_41a970(5);
        v70 = sub_41a970(6);
        v102 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v0 = &v66;
        v103 = v90 + 572;
        v104 = 12;
        for (v105 = &v74; v104; v103 += 4)
        {
            v104 -= 1;
            *(v105) = *((int *)v103);
            v105 += 1;
        }
        v73 |= 1;
        sub_412990(v102 + 20);
        goto LABEL_41962b;
    case 271:
        if (!g_4b43dc->field_1c)
            goto LABEL_4157c3;
        goto LABEL_41962b;
    case 272:
        if (!((int)v90[5816] & 0x2000000))
        {
            v11 = sub_468e20(1);
            v131 = sub_468e20(0);
            v132 = *((int *)&g_4b43dc[1].padding_0[4 * v131]);
            if (g_4cee78 & 0x8000)
            {
                EnterCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 + 1;
            }
            v132->field_130 = v132->field_130 + 1;
            v133 = sub_4621c0();
            v133->field_480 = v133->field_480 | 1;
            v133->field_20 = 9;
            if (v90 == 0xffffffcc)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v133->field_430 = v15;
                v133->field_434 = v16;
                v133->field_438 = v17;
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v133->field_430 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v133->field_434 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v133->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            sub_454d10(v133, v10);
            sub_461250(v133, v10);
LABEL_415ede:
            if (g_4cee78 & 0x8000)
            {
                LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 - 1;
                goto LABEL_41962b;
            }
        }
        else
        {
            v134 = sub_468e20(1);
            v135 = sub_468e20(0);
            sub_4615a0(*((int *)&g_4b43dc[1].padding_0[4 * v135]), &v13, v134, 0);
            goto LABEL_41962b;
        }
    case 273:
        if (!((int)v90[5816] & 0x2000000))
        {
            v11 = sub_468e20(1);
            v137 = sub_468e20(0);
            v138 = *((int *)&g_4b43dc[1].padding_0[4 * v137]);
            v139 = v90 + 52;
            if (g_4cee78 & 0x8000)
            {
                EnterCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 + 1;
            }
            v138->field_130 = v138->field_130 + 1;
            v140 = sub_4621c0();
            v140->field_480 = v140->field_480 | 1;
            v140->field_20 = 9;
            if (!v139)
                goto LABEL_416175;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v140->field_430 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v140->field_434 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v140->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        else
        {
            v11 = sub_468e20(1);
            v141 = sub_468e20(0);
            v142 = *((int *)&g_4b43dc[1].padding_0[4 * v141]);
            v139 = v90 + 52;
            if (g_4cee78 & 0x8000)
            {
                EnterCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 + 1;
            }
            v142->field_130 = v142->field_130 + 1;
            v140 = sub_4621c0();
            v140->field_480 = v140->field_480 | 1;
            v140->field_20 = 9;
            if (!v139)
            {
LABEL_416175:
                /* unsupported instruction */
                /* unsupported instruction */
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v143 = v17;
                v140->field_430 = v15;
                v140->field_434 = v16;
            }
            else
            {
                v140->field_430 = *((int *)v139);
                v140->field_434 = (int)v139[4];
                v143 = (int)v139[8];
            }
            v140->field_438 = v143;
        }
        sub_454d10(v140, v10);
        v144 = *((int *)sub_4612d0(v140, v10));
        if (g_4cee78 & 0x8000)
        {
            LeaveCriticalSection(&g_4cf1d0.DebugInfo);
            g_4cf221 = g_4cf221 - 1;
        }
        v9 = v144;
        v145 = sub_461c50();
        if (!(v17[1454] & 0x2000000))
        {
            v146 = sub_422c40();
            v145->field_430 = v146->field_0;
            v145->field_434 = v146->field_4;
            v145->field_438 = v146->field_8;
        }
        else
        {
            v145->field_430 = *((int *)v139);
            v145->field_434 = (int)v139[4];
            v145->field_438 = (int)v139[8];
        }
        sub_468ec0();
        v145->field_2c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v145->field_47c = v145->field_47c | 4;
        goto LABEL_41962b;
    case 274:
        v154 = sub_468e20(0);
        v155 = v90 + v154 * 4 + 224;
        v11 = v154;
        sub_461a70(*((int *)((char *)v90 + 4 * v11 + 224)));
        *((unsigned int *)&v155->padding_0[0]) = 0;
        v156 = sub_468e20(1);
        *((int *)&v155->padding_0[0]) = *((int *)sub_41c6a0(&v36, (int)v90[556] + v156 + 5));
        sub_461c50(&v36, (int)v90[556] + v156 + 5);
LABEL_415bc0:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v90[5612]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v90[5616]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
LABEL_415bd8:
        if ((char)v90[5816] & 32)
        {
            sub_461d80();
            goto LABEL_41962b;
        }
        break;
    case 275:
        v157 = sub_468e20(0);
        sub_468e20(1);
        sub_461970(*((int *)((char *)v90 + 4 * v157 + 224)));
        goto LABEL_41962b;
    case 276:
        sub_461a70((int)v90[224]);
        *((unsigned int *)&v90[224]) = 0;
        v149 = *((int *)sub_41c6a0(&v37, (int)v90[556]));
        v150 = (int)v90[556];
        v151 = (int)v90[544];
        *((unsigned int *)&v90[5816]) = (int)v90[5816] & 0xfff7ffff;
        *((unsigned int *)&v90[224]) = v149;
        *((unsigned int *)&v90[560]) = 0;
        *((int *)&v90[552]) = v150;
        *((unsigned int *)&v90[548]) = v151;
        goto LABEL_41962b;
    case 277:
        sub_468e20(0);
        v118 = sub_461c50(0);
        if (v118)
        {
            sub_468ec0();
            v118->field_2c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v118->field_47c = v118->field_47c | 4;
            goto LABEL_41962b;
        }
        break;
    case 278:
        sub_468e20(0);
        v119 = sub_461c50(0);
        if (v119)
        {
            sub_468ec0();
            v119->field_40 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v119->field_47c = v119->field_47c | 8;
            sub_468ec0();
            v119->field_44 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v119->field_47c = v119->field_47c | 8;
            goto LABEL_41962b;
        }
        break;
    case 279:
        v120 = sub_468e20(0);
        sub_468ec0(0);
        *((int *)((char *)v90 + 12 * v120 + 288)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v121 = v90 + v120 * 12;
        sub_468ec0();
        *((int *)&v121[292]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v121[296]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        goto LABEL_41962b;
    case 281:
        v122 = sub_468e20(1);
        *((unsigned int *)((char *)v90 + 4 * sub_468e20(0) + 480)) = v122;
        goto LABEL_41962b;
    case 300: case 302:
        v158 = v90 + 104;
        if (v92 != 300)
            v158 = v90 + 156;
        sub_468ec0();
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v159 = v175 - 0;
        /* unsupported instruction */
        /* unsupported instruction */
        v161 = _ccall(10, 13, (char)(((v159 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v161 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)v158) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v162 = v159 + 1;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v162 = v159 + 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)(((v162 - 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            *((int *)&v158[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        *((unsigned int *)&v158[48]) = (int)v158[48] & 0xfffffffc;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((void* *)&v90[52]) = v17;
        *((unsigned int *)&v90[56]) = v18;
        v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((unsigned int **)&v90[60]) = v19;
        sub_413700();
        goto LABEL_41962b;
    case 301: case 303:
        if (v92 == 301)
            v13 = v90 + 104;
        else
            v13 = v90 + 156;
        index = v90 + 652;
        if (v92 != 301)
            index = v90 + 728;
        sub_468ec0();
        v23 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        if (sub_468e20(0) <= 0)
        {
            *((unsigned int *)&index[0x44]) = 0;
            goto LABEL_41962b;
        }
        else
        {
            *((unsigned int *)&index[0x44]) = sub_468e20(0);
            *((unsigned int *)&index[24]) = g_4ce8d0;
            *((unsigned int *)&index[28]) = g_4ce8d4;
            *((unsigned int *)&index[32]) = g_4ce8d8;
            *((unsigned int *)&index[36]) = g_4ce8d0;
            *((unsigned int *)&index[40]) = g_4ce8d4;
            *((unsigned int *)&index[44]) = g_4ce8d8;
            /* unsupported instruction */
            /* unsupported instruction */
            v166 = v175 - 0;
            /* unsupported instruction */
            /* unsupported instruction */
            *((unsigned int *)&index[72]) = sub_468e20(1);
            v167 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500;
            *((unsigned int *)index) = v10->field_0;
            *((unsigned int *)&index[4]) = v10->field_4;
            *((unsigned int *)&index[8]) = v10->field_8;
            v168 = _ccall(10, 13, (char)((((unsigned short)v166 & 7) * 0x800 | (unsigned short)v167 & 0x4700) >> 8) & 5, 0, 0);
            if (!(v168 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v166 -= 0;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            if ((char)((((unsigned short)(v166 - 0) & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((unsigned int *)&index[12]) = v14;
            v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((unsigned int *)&index[16]) = v15;
            v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            *((unsigned int *)&index[20]) = v16;
            sub_406340();
            v10->field_30 = v10->field_30 & 0xfffffffc;
            goto LABEL_41962b;
        }
    case 304: case 306: case 328: case 330:
        if (v92 != 304 && v92 != 328)
            v14 = v90 + 156;
        else
            v14 = v90 + 104;
        sub_468ec0();
        v21 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v176 = v175 + 1;
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)((((unsigned short)v176 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc12e847e00000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            if ((int)v90[5816] & 0x40000)
            {
                v178 = 304;
                if (v91->field_4 == 304 || v91->field_4 == 306)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v7 = 304;
                    v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v6 = v179;
                    v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v176 += 2;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
            }
            idx = v12;
            v5 = v178;
            /* unsupported instruction */
            v181 = (unsigned short)v176 + 1;
            sub_408910(idx, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        }
        else
        {
            idx = v14;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v181 = (unsigned short)v176 + 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)(((v181 - 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc12e847e00000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            *((unsigned int *)&idx[48]) = (int)idx[48] & 0xfffffffc;
            *((int *)&idx[24]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            goto LABEL_41962b;
        }
        else
        {
            *((unsigned int *)&idx[48]) = (int)idx[48] & 0xfffffffc;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            goto LABEL_41962b;
        }
    case 305: case 307: case 329: case 331:
        if (v92 != 305 && v92 != 329)
            v21 = v90 + 156;
        else
            v21 = v90 + 104;
        if (v92 == 305 || (idx1 = v90 + 864, v92 == 329))
            idx1 = v90 + 804;
        sub_468ec0();
        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v22 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        if (sub_468e20(0) <= 0)
        {
            *((unsigned int *)&idx1[52]) = 0;
            goto LABEL_41962b;
        }
        else
        {
            v184 = sub_468e20(1);
            iter = v175 + 1;
            /* unsupported instruction */
            /* unsupported instruction */
            *((unsigned int *)&idx1[56]) = v184;
            if (v184 != 7)
            {
                if (!((char)((((unsigned short)iter & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc12e847e00000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                {
                    if ((int)v90[5816] & 0x40000 && ((short)v12[4] == 305 || (short)v12[4] == 307))
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v5 = 305;
                        v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v4 = v179;
                        v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        iter += 2;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    }
                    idx2 = v17;
                }
                else
                {
                    idx2 = v19;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    iter -= 0;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v188 = iter - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)((((unsigned short)v188 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc12e847e00000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v188 -= 0;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v190 = v188 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v191 = iter - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v193 = v191 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v194 = _ccall(10, 13, (char)(((v191 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                if (!(v194 & 1))
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v195 = v193 + 1;
                }
                else
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v195 = v193 + 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                v196 = v195 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v198 = v196 + 1;
                if (!((char)(((v196 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v190 = v198 + 1;
                    idx2 = v19;
                }
                else
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v190 = v198 + 1;
                    idx2 = v19;
                }
            }
            v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v21 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v200 = v190 - 1;
            if ((char)(((v200 & 7) * 0x800 | (unsigned short)(CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x400921fb60000000) * 0x100 & 0x4500) & 0x4700) >> 8) & 1)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else if (!((char)(((v200 & 7) * 0x800 | (unsigned short)(CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500) & 0x4700) >> 8) & 65))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            *((unsigned int *)&idx1[52]) = sub_468e20(0);
            *((unsigned int *)&idx1[16]) = g_4ce8dc;
            *((unsigned int *)&idx1[20]) = g_4ce8e0;
            *((unsigned int *)&idx1[24]) = g_4ce8dc;
            *((unsigned int *)&idx1[28]) = g_4ce8e0;
            *((unsigned int **)idx1) = v19;
            *((st_4154d0_0 **)&idx1[8]) = v10;
            *((unsigned int *)&idx1[4]) = v20;
            *((void* *)&idx1[12]) = v11;
            sub_41c350(0);
            idx2[12] = idx2[12] & 0xfffffffc;
            goto LABEL_41962b;
        }
    case 308: case 310:
        v202 = v90 + 104;
        if (v92 != 308)
            v202 = v90 + 156;
        sub_468ec0();
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v22 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v23 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        if (!((char)v202[48] & 1))
        {
            v203 = (int)v202[4];
            v204 = (int)v202[8];
            *((int *)&v202[12]) = *((int *)v202);
            *((unsigned int *)&v202[16]) = v203;
            *((unsigned int *)&v202[20]) = v204;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v205 = (unsigned short)v175 + 2;
        /* unsupported instruction */
        /* unsupported instruction */
        v207 = _ccall(10, 13, (char)(((v205 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v207 & 1))
        {
            v7 = v203;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_408910(v202, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v208 = v205 + 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v208 = v205 + 1;
        }
        v209 = v208 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)(((v209 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            *((int *)&v202[24]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v211 = v209 + 1;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v211 = v209 + 1;
        }
        v212 = v211 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)(((v212 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            *((int *)&v202[32]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v214 = v212 + 1;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v214 = v212 + 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)(((v214 - 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            *((int *)&v202[36]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        *((unsigned int *)&v202[48]) = (int)v202[48] | 1;
        sub_464eb0();
        sub_413700();
        goto LABEL_41962b;
    case 309: case 311:
        v14 = (v92 == 309 ? v90 + 104 : v90 + 156);
        v23 = (v92 == 309 ? v90 + 804 : v90 + 864);
        v216 = v90 + 924;
        if (v92 != 309)
            v216 = v90 + 984;
        sub_468ec0();
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v28 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v22 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v217 = v175 + 1;
        /* unsupported instruction */
        /* unsupported instruction */
        v219 = _ccall(10, 13, (char)((((unsigned short)v217 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v219 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v217 -= 0;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        v25 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v220 = v217 - 0;
        /* unsupported instruction */
        /* unsupported instruction */
        if ((char)((((unsigned short)v220 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v220 -= 0;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v22 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if ((char)((((unsigned short)(v220 - 0) & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v28 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v29 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v30 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v31 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v12 = sub_468e20(0);
        v223 = sub_468e20(1);
        if (v11 <= 0)
        {
            *((unsigned int *)&v21[52]) = 0;
            *((void* *)&v216[52]) = NULL;
            goto LABEL_41962b;
        }
        else
        {
            v224 = v21;
            *((void* *)&v224[52]) = v11;
            *((unsigned int *)&v224[16]) = g_4ce8dc;
            *((unsigned int *)&v224[20]) = g_4ce8e0;
            *((unsigned int *)&v224[24]) = g_4ce8dc;
            *((unsigned int *)&v224[28]) = g_4ce8e0;
            *((unsigned int *)v224) = v13;
            *((void* *)&v224[4]) = v14;
            *((unsigned int *)&v224[56]) = v223;
            *((unsigned int *)&v224[8]) = v22;
            *((void* *)&v224[12]) = v23;
            sub_41c350();
            *((void* *)&v216[52]) = v11;
            *((unsigned int *)&v216[16]) = g_4ce8dc;
            *((unsigned int *)&v216[20]) = g_4ce8e0;
            *((unsigned int *)&v216[24]) = g_4ce8dc;
            *((unsigned int *)&v216[28]) = g_4ce8e0;
            *((unsigned int *)v216) = v28;
            *((unsigned int *)&v216[4]) = v29;
            *((unsigned int *)&v216[56]) = v223;
            *((unsigned int *)&v216[8]) = v25;
            *((unsigned int *)&v216[12]) = v26;
            sub_41c350();
            *((unsigned int *)&v14[48]) = (int)v14[48] | 1;
            sub_464eb0();
            sub_413700();
            goto LABEL_41962b;
        }
    case 312: case 313:
        if (v92 == 312)
            v12 = v90 + 104;
        else
            v12 = v90 + 156;
        v272 = v90 + 804;
        if (v92 != 312)
            v272 = v90 + 864;
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
        /* unsupported instruction */
        v274 = v233 - 1;
        if (!((char)((((unsigned short)v274 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v275 = v274 + 1;
            sub_409310();
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v277 = v274 + 1;
            if (!((char)((((unsigned short)v277 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                sub_409310();
                /* unsupported instruction */
                /* unsupported instruction */
                v7 = v179;
                /* unsupported instruction */
                /* unsupported instruction */
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v275 = v277 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v279 = v277;
                if (!((char)((((unsigned short)v279 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                {
                    sub_409310();
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v280 = v279 + 1;
                    if (!(int)((sub_4643d0() & 0xffff) % 3))
                        goto LABEL_417693;
                    goto LABEL_4176a1;
                }
                else
                {
                    sub_409310();
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v280 = v279 + 1;
                    if (!(int)((sub_4643d0() & 0xffff) % 3))
                        goto LABEL_4176a1;
LABEL_417693:
                    v275 = v280 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
            }
        }
        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v280 = v275 + 1;
LABEL_4176a1:
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
        /* unsupported instruction */
        v282 = v280 - 1;
        if (!((char)((((unsigned short)v282 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v283 = v282 - 0;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v285 = v282 + 1;
            if ((char)((((unsigned short)v285 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                goto LABEL_4176fb;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v283 = v285 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v285 = v283 + 1;
LABEL_4176fb:
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
        /* unsupported instruction */
        v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v287 = v285 - 2;
        v288 = _ccall(10, 13, (char)(((v287 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fa99999a0000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v288 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v290 = _ccall(10, 13, (char)(((v287 + 2 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xbff921fb60000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            if (!(v290 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v292 = v287;
            v293 = _ccall(10, 13, (char)(((v292 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 4587366580439587226) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            if (!(v293 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                if (!((char)(((v292 + 2 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                }
                else
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                }
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((unsigned int *)&v272[56]) = sub_41a900();
        /* unsupported instruction */
        /* unsupported instruction */
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((unsigned int *)&v272[52]) = sub_41a900();
        sub_41c300();
        sub_41c320();
        *((unsigned int *)v272) = v16;
        *((unsigned int *)&v272[4]) = v17;
        *((unsigned int *)&v272[8]) = v14;
        *((unsigned int *)&v272[12]) = v15;
        sub_41c350();
        *((unsigned int *)&v11[48]) = (int)v11[48] & 0xfffffffc;
        goto LABEL_41962b;
    case 314:
        v269 = g_4b43dc->field_1c;
        v270 = v90 + 104;
        goto LABEL_4174d2;
    case 315:
        v269 = g_4b43dc->field_1c;
        v270 = v90 + 156;
LABEL_4174d2:
        v271 = v269 + 4212;
        goto LABEL_4174d7;
    case 316: case 317:
        v164 = v90 + 104;
        if (v92 != 316)
            v164 = v90 + 156;
        sub_468ec0();
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v22 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)v164) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v164[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v164[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_413700();
        goto LABEL_41962b;
    case 321: case 323:
        v254 = v90 + 104;
        if (v92 != 321)
            v254 = v90 + 156;
        v23 = (v92 == 321 ? v90 + 804 : v90 + 864);
        v14 = (v92 == 321 ? v90 + 924 : v90 + 984);
        v22 = (v92 == 321 ? v90 + 1044 : v90 + 1104);
        sub_468ec0();
        v28 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v30 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v255 = v233 + 3;
        /* unsupported instruction */
        /* unsupported instruction */
        v257 = _ccall(10, 13, (char)((((unsigned short)v255 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v257 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v255 -= 0;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v32 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        v33 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v28 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v29 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v258 = v255 - 0;
        /* unsupported instruction */
        /* unsupported instruction */
        if ((char)((((unsigned short)v258 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v258 -= 0;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v260 = v258 - 0;
        /* unsupported instruction */
        /* unsupported instruction */
        if ((char)((((unsigned short)v260 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v260 -= 0;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v30 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v31 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v262 = v260 - 0;
        /* unsupported instruction */
        /* unsupported instruction */
        if ((char)((((unsigned short)v262 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v262 -= 0;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if ((char)((((unsigned short)(v262 - 0) & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v25 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v265 = sub_41a900();
        v13 = sub_41a900();
        if (v265 <= 0)
        {
            *((void* *)&v23[52]) = NULL;
            *((void* *)&v14[52]) = NULL;
            *((void* *)&v22[52]) = NULL;
            goto LABEL_41962b;
        }
        else
        {
            *((void* *)&v23[52]) = v265;
            sub_41c300();
            sub_41c320();
            *((unsigned int *)&v23[56]) = v13;
            *((unsigned int *)v23) = v28;
            *((unsigned int *)&v23[8]) = v32;
            *((void* *)&v23[4]) = v29;
            *((void* *)&v23[12]) = v33;
            sub_41c350();
            *((void* *)&v14[52]) = v265;
            sub_41c300();
            sub_41c320();
            *((unsigned int *)v14) = v30;
            *((unsigned int *)&v14[56]) = v13;
            *((void* *)&v14[12]) = v18;
            *((void* *)&v14[4]) = v31;
            *((unsigned int *)&v14[8]) = v17;
            sub_41c350();
            *((void* *)&v22[52]) = v265;
            sub_41c300();
            sub_41c320();
            *((unsigned int *)&v22[56]) = v13;
            *((unsigned int *)v22) = v15;
            *((void* *)&v22[4]) = v16;
            *((unsigned int *)&v22[8]) = v24;
            *((void* *)&v22[12]) = v25;
            sub_41c350();
            v266 = *((int *)v254);
            v267 = (int)v254[4];
            v268 = (int)v254[8];
            *((unsigned int *)&v254[48]) = (int)v254[48] | 3;
            *((unsigned int *)&v254[12]) = v266;
            *((unsigned int *)&v254[16]) = v267;
            *((unsigned int *)&v254[20]) = v268;
            sub_464eb0();
            sub_413700();
            goto LABEL_41962b;
        }
    case 324:
        v113 = sub_468e20(0);
        *((unsigned int *)&v90[5816]) = (int)v90[5816] ^ (v113 * 0x40000 ^ (int)v90[5816]) & 0x40000;
        goto LABEL_41962b;
    case 325: case 326:
        if (v92 == 325)
            v13 = v90 + 104;
        else
            v13 = v90 + 156;
        v170 = v90 + 652;
        if (v92 != 325)
            v170 = v90 + 728;
        sub_468ec0();
        v23 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v32 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v33 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v34 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v171 = v175 + 4;
        /* unsupported instruction */
        /* unsupported instruction */
        v172 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500;
        *((unsigned int *)&v170[0x44]) = sub_468e20(0);
        *((unsigned int *)&v170[24]) = v16;
        *((unsigned int *)&v170[28]) = v17;
        *((unsigned int *)&v170[32]) = v18;
        *((unsigned int *)&v170[36]) = v31;
        *((unsigned int *)&v170[40]) = v32;
        *((unsigned int *)&v170[44]) = v33;
        *((unsigned int *)&v170[72]) = 8;
        *((unsigned int *)v170) = *(v12);
        *((unsigned int *)&v170[4]) = v12[1];
        *((unsigned int *)&v170[8]) = v12[2];
        v173 = _ccall(10, 13, (char)((((unsigned short)v171 & 7) * 0x800 | (unsigned short)v172 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v173 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v171 -= 0;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v21 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if ((char)((((unsigned short)(v171 - 0) & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int *)&v170[12]) = v16;
        v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int *)&v170[16]) = v17;
        v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((unsigned int *)&v170[20]) = v18;
        sub_406340();
        v12[12] = v12[12] & 0xfffffffc;
        goto LABEL_41962b;
    case 327:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v90[104]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v90[108]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v90[112]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int *)&v90[156]) = g_4ce8d0;
        *((unsigned int *)&v90[160]) = g_4ce8d4;
        *((int *)&v90[180]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)&v90[128]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((unsigned int *)&v90[164]) = g_4ce8d8;
        *((unsigned int *)&v90[168]) = g_4ce8d0;
        *((unsigned int *)&v90[172]) = g_4ce8d4;
        *((unsigned int *)&v90[176]) = g_4ce8d8;
        *((unsigned int *)&v90[116]) = g_4ce8d0;
        *((unsigned int *)&v90[120]) = g_4ce8d4;
        *((unsigned int *)&v90[124]) = g_4ce8d8;
        *((unsigned int *)&v90[0xcc]) = (int)v90[0xcc] & 0xfffffffc;
        *((unsigned int *)&v90[152]) = (int)v90[152] & 0xfffffffc;
        *((unsigned int *)&v90[0xcc]) = (int)v90[0xcc] & 0xfffffffd;
        *((unsigned int *)&v90[152]) = (int)v90[152] & 0xfffffffd;
        *((unsigned int *)&v90[720]) = 0;
        *((unsigned int *)&v90[796]) = 0;
        *((unsigned int *)&v90[856]) = 0;
        *((unsigned int *)&v90[916]) = 0;
        *((void* *)&v90[976]) = NULL;
        *((unsigned int *)&v90[1036]) = 0;
        *((unsigned int *)&v90[1096]) = 0;
        *((unsigned int *)&v90[1156]) = 0;
        goto LABEL_41962b;
    case 400:
        sub_468ec0();
        *((int *)&v90[208]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        *((int *)&v90[212]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        goto LABEL_41962b;
    case 401:
        sub_468ec0();
        *((int *)&v90[216]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        *((int *)&v90[220]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        goto LABEL_41962b;
    case 402:
        v296 = sub_41a900();
        *((unsigned int *)&v90[5816]) = (int)v90[5816] | v296;
        if ((char)v90[5816] & 32)
        {
            v297 = 16;
            do
            {
                v298 = v297;
                sub_461d80();
                v297 = v298 - 1;
            } while (v298 != 1);
        }
        break;
    case 403:
        v299 = sub_41a900();
        *((unsigned int *)&v90[5816]) = (int)v90[5816] & ~(v299);
        if (!((char)v90[5816] & 32))
        {
            v300 = 16;
            do
            {
                v301 = v300;
                sub_461d40();
                v300 = v301 - 1;
            } while (v301 != 1);
        }
        break;
    case 404:
        *((unsigned int *)&v90[5816]) = (int)v90[5816] | 0x10000;
        sub_468ec0();
        *((int *)&v90[5620]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        *((int *)&v90[5624]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        *((int *)&v90[5628]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        *((int *)&v90[0x1600]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        goto LABEL_41962b;
    case 405:
        *((unsigned int *)&v90[5816]) = (int)v90[5816] & 0xfffeffff;
        goto LABEL_41962b;
    case 406:
        sub_477420(v90 + 5668, 0, 76);
        goto LABEL_41962b;
    case 407:
        v302 = sub_41a900();
        *((unsigned int *)((char *)v90 + 4 * v302 + 5664)) = sub_41a900();
        goto LABEL_41962b;
    case 408:
        sub_468ec0();
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        *((int *)&v90[5744]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v90[5748]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        goto LABEL_41962b;
    case 409:
        sub_412100();
        goto LABEL_41962b;
    case 410:
        *((unsigned int *)&v90[5664]) = sub_41a900();
        goto LABEL_41962b;
    case 411:
        v303 = sub_41a900();
        v304 = (int)v90[5816];
        *((unsigned int *)&v90[5640]) = v303;
        *((unsigned int *)&v90[5644]) = v303;
        if (v304 & 0x400000)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v7 = v179;
            /* unsupported instruction */
            sub_4126f0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            /* unsupported instruction */
            /* unsupported instruction */
            v6 = v179;
            /* unsupported instruction */
            sub_4126f0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            /* unsupported instruction */
            /* unsupported instruction */
            v5 = v179;
            /* unsupported instruction */
            sub_4126f0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            /* unsupported instruction */
            /* unsupported instruction */
            v4 = v179;
            /* unsupported instruction */
            sub_4126f0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        }
        v305 = (int)v90[5816];
        v306 = (int)v90[5640];
        *((unsigned int *)&v90[5648]) = v306;
        if (v305 & 0x400000)
            *((unsigned int *)&v90[5816]) = v305 | 0x20000000;
        *((unsigned int *)&v90[5652]) = v306 * 7;
        goto LABEL_41962b;
    case 412:
        v307 = sub_41a900();
        sub_4124e0(0);
        if (v307 < 0)
        {
            if ((int)v90[5816] & 0x400000)
                (&g_4b43dc->field_1c)[v90[5828]] = 0;
            *((unsigned int *)&v90[5816]) = (int)v90[5816] & 0xffbfffff;
            goto LABEL_41962b;
        }
        else
        {
            v308 = (int)v90[5964];
            *((unsigned long *)&v90[5816]) = (int)v90[5816] | &g_400000;
            (&g_4b43dc->field_1c)[v307] = v308;
            *((int *)&v90[5828]) = v307;
            goto LABEL_41962b;
        }
    case 414:
        sub_41a900();
        v361 = sub_41a900();
        v362 = sub_41a900();
        *((int *)(v362 * 16 + (int)v90[5964] + 9996)) = v361;
        if (v361 >= 0)
        {
            sub_41a680();
            goto LABEL_41962b;
        }
        break;
    case 415:
        sub_4067e0(sub_41a900());
        goto LABEL_41962b;
    case 416:
        /* unsupported instruction */
        /* unsupported instruction */
        v7 = v94;
        /* unsupported instruction */
        sub_41a900((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        goto LABEL_41962b;
    case 417:
        sub_4529a0(1, sub_41a900(sub_41a900(sub_41a900(0, 70))));
        goto LABEL_41962b;
    case 419:
        if (g_4b43e4->field_6d30 && !sub_412720())
            goto LABEL_41962d;
        break;
    case 420:
        if (g_4b43dc->field_1c)
            goto LABEL_41962d;
        break;
    case 421:
        v363 = sub_41a900();
        *((st_4154d0_3 **)(v363 * 16 + (int)v90[5964] + 10008)) = &v91->field_18;
        goto LABEL_41962b;
    case 423:
        sub_40e5e0();
        *((unsigned int *)&v90[5660]) = (int)v90[5660] & 0xfffffffe;
        goto LABEL_41962b;
    case 424:
        sub_41a900();
        sub_41c550();
        goto LABEL_41962b;
    case 426:
        sub_468ec0();
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v90[5832]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        goto LABEL_41962b;
    case 427:
        sub_468ec0();
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v24 = (int)v90[5644];
        sub_41a900();
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v7 = v179;
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_41a900((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        sub_4126f0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        goto LABEL_41962b;
    case 429:
        if (g_4b0ccc < 0x200)
            goto LABEL_4191a2;
        break;
    case 430:
        if (g_4b0ccc >= 600)
        {
            v399 = sub_41a960();
            sub_41a950();
            *(v399) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            goto LABEL_41962b;
        }
        else if (g_4b0ccc >= 200)
        {
            v400 = sub_41a960();
            sub_41a950();
            *(v400) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            goto LABEL_41962b;
        }
        else if (g_4b0ccc >= -0xc8)
        {
            v401 = sub_41a960();
            sub_41a950();
            *(v401) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            goto LABEL_41962b;
        }
        else if (g_4b0ccc < -0x190)
        {
LABEL_4191a2:
            v402 = sub_41a960();
            sub_41a950();
            *(v402) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            goto LABEL_41962b;
        }
        else
        {
            v403 = sub_41a960();
            sub_41a950();
            *(v403) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            goto LABEL_41962b;
        }
    case 431:
        sub_41a950();
        v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_41a950();
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v313 = sub_41a960();
        /* unsupported instruction */
        /* unsupported instruction */
        break;
    case 432:
        if (g_4b0ccc >= 0x200)
            goto LABEL_417bc7;
LABEL_4192a9:
        v409 = sub_41a910();
        *(v409) = sub_41a900();
        goto LABEL_41962b;
    case 433:
        if (g_4b0ccc >= 600)
            goto LABEL_417bff;
        if (g_4b0ccc >= 200)
        {
            v406 = sub_41a910();
            *(v406) = sub_41a900();
            goto LABEL_41962b;
        }
        else if (g_4b0ccc >= -0xc8)
        {
            v407 = sub_41a910();
            *(v407) = sub_41a900();
            goto LABEL_41962b;
        }
        else
        {
            if (g_4b0ccc < -0x190)
                goto LABEL_4192a9;
            v408 = sub_41a910();
            *(v408) = sub_41a900();
            goto LABEL_41962b;
        }
    case 434:
        v410 = sub_41a900();
        v411 = sub_41a900();
        v412 = (g_4b0ccc + 0x400) * (v411 - v410);
        *((unsigned int *)sub_41a910()) = ((int)(v412 + ((int)(v412) >> 31 & 0x7ff)) >> 11) + v410;
        goto LABEL_41962b;
    case 435:
        switch (g_4b0ca8)
        {
        case 0:
            break;
        case 1:
LABEL_417bc7:
            v404 = sub_41a910();
            *(v404) = sub_41a900();
            goto LABEL_41962b;
        case 2:
            v312 = sub_41a910();
            *(v312) = sub_41a900();
            goto LABEL_41962b;
        case 3: case 4:
LABEL_417bff:
            v405 = sub_41a910();
            *(v405) = sub_41a900();
            goto LABEL_41962b;
        default:
            goto LABEL_41962b;
        }
    case 436:
        switch (g_4b0ca8)
        {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3: case 4:
            sub_468ec0();
            v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v313 = sub_4690b0();
            /* unsupported instruction */
            /* unsupported instruction */
            break;
        default:
            goto LABEL_41962b;
        }
    case 440:
        v413 = sub_41a900();
        g_4b43e4->field_6cf4 = v413;
        goto LABEL_41962b;
    case 441:
        sub_4067e0(sub_41a900());
        goto LABEL_41962b;
    case 442:
        g_4b43cc->field_7c = g_4b43cc->field_7c | 8;
        goto LABEL_41962b;
    case 443:
        sub_4127b0();
        goto LABEL_41962b;
    case 444:
        v371 = sub_41a900();
        *((unsigned int *)&v90[5816]) = (int)v90[5816] ^ (v371 * 0x4000000 ^ (int)v90[5816]) & 0x4000000;
        goto LABEL_41962b;
    case 445:
        sub_428670();
        goto LABEL_41962b;
    case 446:
        v414 = sub_41a900();
        *((unsigned int *)&v90[5816]) = (int)v90[5816] ^ (v414 * 0x8000000 ^ (int)v90[5816]) & 0x8000000;
        v415 = sub_41a900();
        *((unsigned int *)&v90[5816]) = (int)v90[5816] & 0xefffffff;
        *((unsigned int *)&v90[5820]) = v415;
        *((int *)&v90[5824]) = (int)v90[556];
        goto LABEL_41962b;
    case 447:
        sub_41a950();
        g_4b2ed0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        goto LABEL_41962b;
    case 448:
        if (!g_4b0ca8)
        {
            v15 = sub_41a900();
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else if (g_4b0ca8 == 1)
        {
            v15 = sub_41a900();
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else if (g_4b0ca8 == 2)
        {
            v15 = sub_41a900();
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v15 = sub_41a900();
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v21 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v313 = *((int *)((int)v90[5964] + 4));
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        break;
    case 449:
        v419 = sub_41a900();
        *((unsigned int *)&v90[5816]) = (int)v90[5816] ^ (v419 * 0x40000000 ^ (int)v90[5816]) & 0x40000000;
        goto LABEL_41962b;
    case 450:
        *((unsigned int *)&v90[564]) = sub_41a900();
        goto LABEL_41962b;
    case 451:
        sub_414dc0(sub_41a900());
        goto LABEL_41962b;
    case 452:
        *((unsigned int *)&v90[568]) = sub_468e20(0);
        goto LABEL_41962b;
    case 453:
        *((unsigned int *)&v90[5772]) = sub_468e20(0);
        goto LABEL_41962b;
    case 454:
        sub_421740();
        goto LABEL_41962b;
    case 455:
        sub_41a900();
        v420 = sub_412500();
        *((unsigned int *)sub_41a910()) = v420;
        goto LABEL_41962b;
    case 456:
        sub_41a900();
        sub_412530();
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)sub_41a960()) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v313 = sub_41a960();
        /* unsupported instruction */
        /* unsupported instruction */
        break;
    case 500:
        v314 = sub_41a900();
        v315 = v314 * 532 + v90;
        sub_477420(v315 + 1164, 0, 532);
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned short *)&v315[1668]) = 1;
        *((int *)&v315[1180]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned short *)&v315[1670]) = 1;
        *((int *)&v315[1188]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((unsigned int *)&v315[1680]) = 22;
        *((unsigned int *)&v315[1684]) = 40;
        *((unsigned int *)&v315[1676]) = 131;
        v316 = v90 + v314 * 12;
        *((int *)&v316[5420]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)((char *)v90 + 12 * v314 + 5424)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)&v316[5516]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)((char *)v90 + 12 * v314 + 5520)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)&v316[5524]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        goto LABEL_41962b;
    case 501:
        /* unsupported instruction */
        /* unsupported instruction */
        v323 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fecccccc0000000) * 0x100 & 0x4500;
        /* unsupported instruction */
        v324 = sub_41a900() * 532 + v90;
        if (!((char)((((unsigned short)v233 & 7) * 0x800 | (unsigned short)v323 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((unsigned int *)&v324[1168]) = v17;
            *((unsigned int *)&v324[1172]) = v18;
            v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v324[1176]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((unsigned int *)&v324[1168]) = v17;
            *((unsigned int *)&v324[1172]) = v18;
            v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            *((unsigned int *)&v324[1176]) = v19;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        g_4b43c8->field_60 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_40b5e0();
        /* unsupported instruction */
        /* unsupported instruction */
        g_4b43c8->field_60 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        goto LABEL_41962b;
    case 502:
        v325 = sub_41a900() * 532 + v90;
        *((unsigned short *)&v325[1164]) = sub_41a900();
        *((unsigned short *)&v325[1166]) = sub_41a900();
        goto LABEL_41962b;
    case 503:
        v326 = sub_41a900();
        sub_468ec0();
        *((int *)((char *)v90 + 12 * v326 + 5420)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        *((int *)((char *)v90 + 12 * v326 + 5424)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        goto LABEL_41962b;
    case 504:
        v333 = sub_41a900() * 532 + v90;
        sub_468ec0();
        *((int *)&v333[1180]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        *((int *)&v333[1184]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        goto LABEL_41962b;
    case 505:
        v334 = sub_41a900() * 532 + v90;
        sub_468ec0();
        *((int *)&v334[1188]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        *((int *)&v334[1192]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        goto LABEL_41962b;
    case 506:
        v335 = sub_41a900() * 532 + v90;
        goto LABEL_4180c5;
    case 507:
        v354 = sub_41a900();
        *((unsigned short *)(532 * v354 + (char *)v90 + 1672)) = sub_41a900();
        goto LABEL_41962b;
    case 508:
        v355 = sub_41a900() * 532 + v90;
        *((unsigned int *)&v355[1680]) = sub_41a900();
        *((unsigned int *)&v355[1684]) = sub_41a900();
        goto LABEL_41962b;
    case 509:
        v356 = sub_41a900();
        v357 = sub_41a900();
        v15 = v356 * 532;
        v358 = v90 + v357 * 24 + v15;
        v358[305] = sub_41a900();
        v358[304] = sub_41a900();
        v358[302] = sub_41a900();
        v358[303] = sub_41a900();
        sub_41a950();
        *((int *)((char *)v90 + 24 * v357 + v15 + 1200)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_41a950();
        v358[301] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        goto LABEL_41962b;
    case 510:
        sub_40cf60(1);
        sub_428750(1);
        goto LABEL_41962b;
    case 511:
        v15 = sub_41a900();
        v317 = sub_41a900();
        v318 = v317 * 532 + v90 + 1164;
        iter2 = v15 * 532 + v90 + 1164;
        for (v320 = 133; v320; v318 = &v318->padding_0[4])
        {
            v320 -= 1;
            *((int *)&iter2->padding_0[0]) = *((int *)&v318->padding_0[0]);
            iter2 = &iter2->padding_0[4];
        }
        v321 = v90 + v317 * 12;
        v322 = v90 + v15 * 12;
        *((int *)&v322[5420]) = (int)v321[5420];
        *((int *)&v322[5424]) = (int)v321[5424];
        *((int *)&v322[5428]) = (int)v321[5428];
        v271 = v321 + 5516;
        v270 = v322 + 5516;
LABEL_4174d7:
        *((int *)v270) = *((int *)v271);
        *((int *)&v270[4]) = (int)v271[4];
        *((int *)&v270[8]) = (int)v271[8];
        goto LABEL_41962b;
    case 512:
        sub_41a950();
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v5 = v179;
        /* unsupported instruction */
        sub_40caa0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 1, 0);
        /* unsupported instruction */
        /* unsupported instruction */
        v2 = v179;
        /* unsupported instruction */
        sub_4286f0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 1, 1);
        goto LABEL_41962b;
    case 513:
        sub_41a950();
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v5 = v179;
        /* unsupported instruction */
        sub_40caa0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0, 0);
        /* unsupported instruction */
        /* unsupported instruction */
        v2 = v179;
        /* unsupported instruction */
        sub_4286f0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0, 1);
        goto LABEL_41962b;
    case 514:
        v340 = sub_41a900() * 532 + v90;
        if (g_4b0ccc >= 0x200)
        {
            sub_41a950();
            *((int *)&v340[1188]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_41a950();
            *((int *)&v340[1192]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            goto LABEL_41962b;
        }
        else
        {
            if (g_4b0ccc < -0x200)
                goto LABEL_41828d;
            goto LABEL_418266;
        }
    case 515:
        v340 = sub_41a900() * 532 + v90;
        if (g_4b0ccc >= 600)
        {
            sub_41a950();
            goto LABEL_418275;
        }
        if (g_4b0ccc >= 200)
        {
            sub_41a950();
            goto LABEL_418275;
        }
        else if (g_4b0ccc >= -0xc8)
        {
            sub_41a950();
LABEL_418275:
            *((int *)&v340[1188]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_41a950();
            *((int *)&v340[1192]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            goto LABEL_41962b;
        }
        else if (g_4b0ccc >= -0x258)
        {
LABEL_418266:
            sub_41a950();
            goto LABEL_418275;
        }
        else
        {
LABEL_41828d:
            sub_41a950();
            *((int *)&v340[1188]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_41a950();
            *((int *)&v340[1192]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            goto LABEL_41962b;
        }
    case 516:
        v341 = sub_41a900();
        sub_41a950();
        v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_41a950();
        v28 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_41a950();
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_41a950();
        v30 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v342 = v341 * 532 + v90;
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
        *((int *)&v342[1188]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
        *((int *)&v342[1192]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        goto LABEL_41962b;
    case 517:
        v343 = sub_41a900();
        if (g_4b0ccc < 0x200)
        {
            v344 = v343 * 532;
            v345 = g_4b0ccc;
            v346 = -0x200;
            goto LABEL_41841b;
        }
        break;
    case 518:
        v343 = sub_41a900();
        if (g_4b0ccc >= 600)
        {
            v335 = v343 * 532 + v90;
            v347 = sub_41a900();
        }
        else if (g_4b0ccc >= 200)
        {
            v335 = v343 * 532 + v90;
            v347 = sub_41a900();
        }
        else if (g_4b0ccc >= -0xc8)
        {
            v335 = v343 * 532 + v90;
            v347 = sub_41a900();
        }
        else
        {
            v344 = v343 * 532;
            v345 = g_4b0ccc;
            v346 = -0x258;
LABEL_41841b:
            v335 = v344 + v90;
            if (v345 < v346)
            {
LABEL_4180c5:
                v347 = sub_41a900();
            }
            else
            {
                v347 = sub_41a900();
            }
        }
        *((unsigned short *)&v335[1668]) = v347;
        *((unsigned short *)&v335[1670]) = sub_41a900();
        goto LABEL_41962b;
    case 519:
        v348 = sub_41a900();
        v349 = sub_41a900();
        v24 = sub_41a900();
        v15 = sub_41a900();
        v350 = sub_41a900();
        v351 = v348 * 532;
        v352 = (g_4b0ccc + 0x400) * (v15 - v349);
        *((unsigned short *)(v351 + (char *)v90 + 1668)) = ((int)(v352 + ((int)(v352) >> 31 & 0x7ff)) >> 11) + v349;
        v353 = (g_4b0ccc + 0x400) * (v350 - v24);
        *((unsigned short *)(v351 + (char *)v90 + 1670)) = ((int)(v353 + ((int)(v353) >> 31 & 0x7ff)) >> 11) + v24;
        goto LABEL_41962b;
    case 520:
        /* unsupported instruction */
        /* unsupported instruction */
        v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v32 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_41a950();
        /* unsupported instruction */
        /* unsupported instruction */
        v7 = v179;
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_41a950((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        v6 = v179;
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_4088c0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v313 = sub_41a960((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        break;
    case 521:
        v336 = sub_41a900();
        sub_468ec0();
        v337 = v336 * 532;
        v21 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)(v337 + (char *)v90 + 1188)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v21 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)(v337 + (char *)v90 + 1192)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        goto LABEL_41962b;
    case 522:
        v339 = sub_41a900() * 532 + v90;
        *((unsigned short *)&v339[1668]) = sub_41a900();
        *((unsigned short *)&v339[1670]) = sub_41a900();
        goto LABEL_41962b;
    case 523:
        v327 = sub_41a900();
        sub_468ec0();
        v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_41c580((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v90 + 12 * v327 + 5420)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v90 + 12 * v327 + 5424)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        goto LABEL_41962b;
    case 524:
        v328 = sub_41a900();
        sub_468ec0();
        *((int *)(532 * v328 + (char *)v90 + 1196)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        goto LABEL_41962b;
    case 525:
        v329 = sub_41a900();
        v330 = v90 + v329 * 12;
        sub_468ec0();
        *((int *)&v330[5516]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        *((int *)((char *)v90 + 12 * v329 + 5520)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v332 = _ccall(10, 13, (char)((((unsigned short)v233 + 2 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc08ef00000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v332 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v330[5524]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            goto LABEL_41962b;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v330[5524]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            goto LABEL_41962b;
        }
    case 526:
        if ((int)v90[5968])
            sub_402f20();
        *((unsigned int *)&v90[5968]) = 0;
        sub_41a950();
        *((int *)&v90[5972]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v90[5976]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v7 = v179;
        /* unsupported instruction */
        *((unsigned int *)&v90[5980]) = sub_41a900();
        sub_4061b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        v6 = v179;
        /* unsupported instruction */
        sub_4061b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        v417 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (int)v90[5972]) * 0x100 & 0x4500;
        /* unsupported instruction */
        v418 = _ccall(10, 13, (char)((((unsigned short)v233 + 1 & 7) * 0x800 | (unsigned short)v417 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v418 & 1))
        {
            v13 = sub_46c9ea(24);
            v78 = 0;
            if (v13)
            {
                *((unsigned int *)&v90[5968]) = sub_40ff10(0x11);
                goto LABEL_41962b;
            }
            else
            {
                *((unsigned int *)&v90[5968]) = 0;
                goto LABEL_41962b;
            }
        }
        break;
    case 527:
        sub_41a900();
        sub_4055d0();
        goto LABEL_41962b;
    case 528:
        v114 = sub_468e20(0);
        g_4b43dc->field_3c = g_4b43dc->field_3c ^ (g_4b43dc->field_3c ^ v114) & 1;
        goto LABEL_41962b;
    case 529:
        *((unsigned int *)&v90[5992]) = g_4af0fc[sub_41a900()];
        goto LABEL_41962b;
    case 530:
        *((unsigned int *)&v90[5996]) = g_4af118[sub_41a900()];
        goto LABEL_41962b;
    case 531:
        *((unsigned int *)&v90[6000]) = g_4af120[sub_41a900()];
        goto LABEL_41962b;
    case 532:
        sub_41a950();
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v5 = v179;
        /* unsupported instruction */
        sub_40caa0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 1, 1);
        /* unsupported instruction */
        /* unsupported instruction */
        v2 = v179;
        /* unsupported instruction */
        sub_4286f0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 1, 1);
        goto LABEL_41962b;
    case 533:
        sub_41a950();
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v5 = v179;
        /* unsupported instruction */
        sub_40caa0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0, 1);
        /* unsupported instruction */
        /* unsupported instruction */
        v2 = v179;
        /* unsupported instruction */
        sub_4286f0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0, 1);
        goto LABEL_41962b;
    case 534:
        v295 = sub_41a900();
        g_4af0fc[v295]();
        goto LABEL_41962b;
    case 535:
        sub_40e730(sub_468e20(0));
        sub_43e250(v90 + 52, -0x1);
        goto LABEL_41962b;
    case 600:
        v359 = sub_41a900() * 532 + v90;
        sub_41a950();
        *((int *)&v359[1168]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_41a950();
        *((int *)&v359[1172]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_41a950();
        *((int *)&v359[1176]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_41a950();
        *((int *)&v359[1632]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        goto LABEL_41962b;
    case 601:
        v360 = sub_41a900() * 532 + v90;
        *((unsigned int *)&v360[1648]) = sub_41a900();
        *((unsigned int *)&v360[1652]) = sub_41a900();
        *((unsigned int *)&v360[1656]) = sub_41a900();
        *((unsigned int *)&v360[1660]) = sub_41a900();
        *((unsigned int *)&v360[1664]) = sub_41a900();
        goto LABEL_41962b;
    case 602:
        sub_477420(&v38, 0, 488);
        v15 = sub_41a900() * 532 + v90;
        v373 = v15 + 1200;
        v374 = 108;
        for (v375 = &v52; v374; v373 += 4)
        {
            v374 -= 1;
            *(v375) = *((int *)v373);
            v375 += 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)((((unsigned short)v233 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fecccccc0000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v40 = v19;
            v40 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v40 = v19;
        }
        v377 = v15;
        /* unsupported instruction */
        /* unsupported instruction */
        v39 = v18;
        v378 = _INSERT(v39, 0, (short)v377[1166]);
        v38 = v17;
        v48 = (short)v377[1164];
        v49 = v378;
        v7 = v378;
        /* unsupported instruction */
        sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        v40 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v50 |= 1;
        v46 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v42 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v64 = (int)v377[1680];
        v41 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v65 = (int)v377[1684];
        /* unsupported instruction */
        /* unsupported instruction */
        v43 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v44 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v48 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_428450(0);
        goto LABEL_41962b;
    case 603:
        sub_4128e0();
        v15 = sub_41a900() * 532 + v90;
        v380 = v15 + 1200;
        v381 = 108;
        for (v382 = &v61; v381; v380 += 4)
        {
            v381 -= 1;
            *(v382) = *((int *)v380);
            v382 += 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)((((unsigned short)v233 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fecccccc0000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v40 = v19;
            v40 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v40 = v19;
        }
        v384 = v15;
        /* unsupported instruction */
        /* unsupported instruction */
        v39 = v18;
        v385 = _INSERT(v39, 0, (short)v384[1166]);
        v38 = v17;
        v59 = (short)v384[1164];
        v60 = v385;
        v7 = v385;
        /* unsupported instruction */
        sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        v43 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v50 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v52 = (int)v384[1652];
        v47 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v51 = (int)v384[1648];
        v46 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v53 = (int)v384[1656];
        v48 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v54 = (int)v384[1660];
        v58 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v59 = (int)v384[1664] | 2;
        v55 = (int)v384[1680];
        v56 = (int)v384[1684];
        v57 = sub_41a900((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        sub_428450(1);
        goto LABEL_41962b;
    case 604:
        sub_41a900();
        v394 = sub_412960();
        if (v394)
        {
            sub_41a950();
            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_41a950();
            v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v394[20] = v17;
            v394[21] = v18;
            v394[22] = v19;
            goto LABEL_41962b;
        }
        break;
    case 605:
        sub_41a900();
        if (sub_412960())
        {
            sub_41a950();
            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_41a950();
            v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_412900();
            goto LABEL_41962b;
        }
        break;
    case 606:
        sub_41a900();
        v395 = sub_412960();
        if (v395)
        {
            sub_41a950();
            v395->field_74 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            goto LABEL_41962b;
        }
        break;
    case 607:
        sub_41a900();
        v396 = sub_412960();
        if (v396)
        {
            sub_41a950();
            v396->field_70 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            goto LABEL_41962b;
        }
        break;
    case 608:
        sub_41a900();
        v397 = sub_412960();
        if (v397)
        {
            sub_41a950();
            v397->field_68 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            goto LABEL_41962b;
        }
        break;
    case 609:
        sub_41a900();
        v398 = sub_412960();
        if (v398)
        {
            sub_41a950();
            v398->field_470 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            goto LABEL_41962b;
        }
        break;
    case 610:
        sub_41a900();
        v393 = sub_412960();
        if (v393)
        {
            do
            {
                v393->field_0->field_14(0, 0);
                v393->field_80 = 0;
                sub_41a900(0, 0);
                v393 = sub_412960();
            } while (v393);
        }
        break;
    case 611:
        sub_477420(&v38, 0, 480);
        v15 = sub_41a900() * 532 + v90;
        v387 = v15 + 1200;
        v388 = 108;
        for (v389 = &v50; v388; v387 += 4)
        {
            v388 -= 1;
            *(v389) = *((int *)v387);
            v389 += 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)((((unsigned short)v233 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fecccccc0000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v40 = v19;
            v40 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v40 = v19;
        }
        v391 = v15;
        /* unsupported instruction */
        /* unsupported instruction */
        v39 = v18;
        v392 = _INSERT(v39, 0, (short)v391[1166]);
        v38 = v17;
        v44 = (short)v391[1164];
        v45 = v392;
        v7 = v392;
        /* unsupported instruction */
        sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        v40 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v42 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v47 |= 1;
        v41 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v46 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v44 = (int)v391[1648];
        v62 = (int)v391[1680];
        v63 = (int)v391[1684];
        sub_428450(2);
        goto LABEL_41962b;
    case 257:
LABEL_4156f6:
        sub_477420(&v74 + 4, 0, 80);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_41a9b0(1, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        v73 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v5 = v179;
        /* unsupported instruction */
        sub_41a9b0(2, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        v72 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v422 = sub_41a970(3);
        v72 = sub_41a970(4);
        v98 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v72 = sub_41a970(5);
        v99 = v90 + 572;
        v100 = 12;
        for (node = &v75; v100; v99 += 4)
        {
            v100 -= 1;
            *(node) = *((int *)v99);
            node += 1;
        }
        sub_412990(v98 + 20, &v68);
    case 282: case 283: case 284: case 285: case 286: case 287: case 288: case 289: case 290: case 291: case 292: case 293: case 294: case 295: case 296: case 297: case 298: case 299: case 332: case 333: case 334: case 335: case 336: case 337: case 338: case 339: case 340: case 341: case 342: case 343: case 344: case 345: case 346: case 347: case 348: case 349: case 350: case 351: case 352: case 353: case 354: case 355: case 356: case 357: case 358: case 359: case 360: case 361: case 362: case 363: case 364: case 365: case 366: case 367: case 368: case 369: case 370: case 371: case 372: case 373: case 374: case 375: case 376: case 377: case 378: case 379: case 380: case 381: case 382: case 383: case 384: case 385: case 386: case 387: case 388: case 389: case 390: case 391: case 392: case 393: case 394: case 395: case 396: case 397: case 398: case 399: case 457: case 458: case 459: case 460: case 461: case 462: case 463: case 464: case 465: case 466: case 467: case 468: case 469: case 470: case 471: case 472: case 473: case 474: case 475: case 476: case 477: case 478: case 479: case 480: case 481: case 482: case 483: case 484: case 485: case 486: case 487: case 488: case 489: case 490: case 491: case 492: case 493: case 494: case 495: case 496: case 497: case 498: case 499: case 536: case 537: case 538: case 539: case 540: case 541: case 542: case 543: case 544: case 545: case 546: case 547: case 548: case 549: case 550: case 551: case 552: case 553: case 554: case 555: case 556: case 557: case 558: case 559: case 560: case 561: case 562: case 563: case 564: case 565: case 566: case 567: case 568: case 569: case 570: case 571: case 572: case 573: case 574: case 575: case 576: case 577: case 578: case 579: case 580: case 581: case 582: case 583: case 584: case 585: case 586: case 587: case 588: case 589: case 590: case 591: case 592: case 593: case 594: case 595: case 596: case 597: case 598: case 599:
LABEL_41962b:
        goto LABEL_41962d;
    case 318: case 319:
        v225 = v90 + 104;
        if (v92 != 318)
            v225 = v90 + 156;
        sub_468ec0();
        v28 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v226 = v175 - 0;
        /* unsupported instruction */
        /* unsupported instruction */
        v228 = _ccall(10, 13, (char)((((unsigned short)v226 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v228 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v225[12]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v229 = v226 + 1;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v229 = v226 + 1;
        }
        v230 = v229 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v232 = v230 + 1;
        if (!((char)((((unsigned short)v230 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            *((int *)&v225[16]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v233 = v232 + 1;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v233 = v232 + 1;
        }
        sub_464eb0();
        sub_413700();
        v91 = v14;
        v90 = v21;
    case 320: case 322:
        v234 = v90 + 104;
        if (v91->field_4 != 320)
            v234 = v90 + 156;
        sub_468ec0();
        v28 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v22 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v23 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_468ec0();
        v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        if (!((char)v234[48] & 1))
        {
            v235 = (int)v234[4];
            v236 = (int)v234[8];
            *((int *)&v234[12]) = *((int *)v234);
            *((unsigned int *)&v234[16]) = v235;
            *((unsigned int *)&v234[20]) = v236;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v237 = v233 + 4;
        /* unsupported instruction */
        /* unsupported instruction */
        v239 = _ccall(10, 13, (char)((((unsigned short)v237 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v239 & 1))
        {
            v7 = v235;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_408910(v234, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v240 = (unsigned short)v237 + 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v240 = (unsigned short)v237 + 1;
        }
        v241 = v240 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)(((v241 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            *((int *)&v234[24]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v243 = v241 + 1;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v243 = v241 + 1;
        }
        v244 = v243 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)(((v244 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            *((int *)&v234[32]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v246 = v244 + 1;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v246 = v244 + 1;
        }
        v247 = v246 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)(((v247 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            *((int *)&v234[36]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v249 = v247 + 1;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v249 = v247 + 1;
        }
        v250 = v249 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)(((v250 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            v5 = v235;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_41c5e0(v234, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v252 = v250 + 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v252 = v250 + 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)(((v252 - 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            *((int *)&v234[44]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        *((unsigned int *)&v234[48]) = (int)v234[48] | 3;
        sub_464eb0();
        sub_413700();
        goto LABEL_41962b;
    case 418:
        sub_41a900();
        sub_41fbe0();
        sub_40cf60(0);
        sub_428750(0);
    case 425:
        sub_414c60();
        goto LABEL_41962b;
    case 422: case 428: case 437: case 438: case 439:
        v365 = 0;
        v366 = 0x77;
        v27 = 7;
        v24 = v14->field_1c;
        if (v14->field_1c > 0)
        {
            v367 = -(&v67) + v91;
            v15 = v367;
            while (1)
            {
                *((char *)&v422 + v365) = *((char *)(v367 + (char *)&v422 + v365)) ^ v366;
                v366 += v27;
                v365 += 1;
                v27 += 16;
                if (v365 >= v24)
                    break;
                v367 = v15;
            }
        }
        sub_468e20(0);
        v369 = v13->field_4 - 437;
        sub_468e20(2);
        sub_40e060(sub_468e20(1));
        *((unsigned int *)&v90[5660]) = (int)v90[5660] | 1;
        *((unsigned int *)&v90[5652]) = (int)v90[5640] * 7;
    case 413:
        sub_4067e0(0);
        goto LABEL_41962b;
    default:
        goto LABEL_41962b;
    }
    v313->field_0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
LABEL_41962d:
    v421 = _ccall(v82, v83, v84, 0);
    *((unsigned int *)(unsigned int)v421) = v76;
    return sub_46c932();
}
