// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4862d4; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4862d4(void)
{
    unsigned int v2;  // edi
    unsigned int v9;  // esi
    unsigned int v99;  // 4125
    unsigned int v100;  // esi
    unsigned int v101;  // eax
    unsigned int v102;  // 4125
    unsigned int v103;  // esi
    unsigned int v104;  // eax
    unsigned int v105;  // 4125
    unsigned int v106;  // esi
    unsigned int v107;  // eax
    unsigned int v108;  // esi
    unsigned int v10;  // eax
    unsigned int v109;  // 4110
    void* v110;  // ecx
    void* v111;  // edx
    unsigned int v112;  // eax
    unsigned int v113;  // esi
    unsigned int v114;  // 4125
    unsigned int v115;  // esi
    unsigned int v116;  // eax
    unsigned int v117;  // 4125
    unsigned int v118;  // esi
    unsigned int v11;  // 4125
    unsigned int v119;  // eax
    unsigned int v120;  // 4125
    unsigned int v121;  // esi
    unsigned int v122;  // eax
    unsigned int v123;  // esi
    unsigned int v124;  // 4110
    unsigned int v125;  // esi
    unsigned int v126;  // eax
    unsigned int v127;  // 4125
    unsigned int v128;  // esi
    unsigned int v12;  // esi
    unsigned int v129;  // eax
    unsigned int v130;  // 4125
    unsigned int v131;  // esi
    unsigned int v132;  // eax
    unsigned int v133;  // 4125
    unsigned int v134;  // esi
    unsigned int v135;  // eax
    unsigned int v136;  // esi
    unsigned int v137;  // 4110
    unsigned int v138;  // esi
    unsigned int v13;  // eax
    unsigned int v139;  // eax
    unsigned int v140;  // 4125
    unsigned int v141;  // esi
    unsigned int v142;  // eax
    unsigned int v143;  // 4125
    unsigned int v144;  // esi
    unsigned int v145;  // eax
    unsigned int v146;  // 4125
    unsigned int v147;  // esi
    unsigned int v148;  // eax
    unsigned int v14;  // 4125
    unsigned int v149;  // esi
    unsigned int v150;  // 4110
    unsigned int v151;  // esi
    unsigned int v152;  // eax
    unsigned int v153;  // 4125
    unsigned int v154;  // esi
    unsigned int v155;  // eax
    unsigned int v156;  // 4125
    unsigned int v157;  // esi
    unsigned int v158;  // eax
    unsigned int v15;  // esi
    unsigned int v159;  // 4125
    unsigned int v160;  // esi
    unsigned int v161;  // eax
    unsigned int v162;  // esi
    unsigned int v163;  // 4110
    unsigned int v164;  // esi
    unsigned int v165;  // eax
    unsigned int v166;  // 4125
    unsigned int v167;  // esi
    unsigned int v168;  // eax
    unsigned int v16;  // eax
    unsigned int v169;  // 4125
    unsigned int v170;  // esi
    unsigned int v171;  // eax
    unsigned int v172;  // 4125
    unsigned int v173;  // esi
    unsigned int v174;  // eax
    unsigned int v175;  // esi
    unsigned int v176;  // 4110
    unsigned int v177;  // esi
    unsigned int v178;  // eax
    unsigned int v17;  // esi
    unsigned int v179;  // 4125
    unsigned int v180;  // esi
    unsigned int v181;  // eax
    unsigned int v182;  // 4125
    unsigned int v183;  // esi
    unsigned int v184;  // eax
    unsigned int v185;  // 4125
    unsigned int v186;  // esi
    unsigned int v187;  // eax
    unsigned int v188;  // esi
    unsigned int v18;  // 4110
    unsigned int v189;  // 4110
    unsigned int v190;  // esi
    unsigned int v191;  // eax
    unsigned int v192;  // 4125
    unsigned int v193;  // esi
    unsigned int v194;  // eax
    unsigned int v195;  // 4125
    unsigned int v196;  // esi
    unsigned int v197;  // eax
    unsigned int v198;  // 4125
    unsigned int i;  // edi
    unsigned int v19;  // esi
    unsigned int v199;  // eax
    unsigned int v200;  // ecx
    unsigned int v201;  // eax
    unsigned int v202;  // 4110
    unsigned int v203;  // esi
    unsigned int v204;  // eax
    unsigned int v205;  // 4125
    unsigned int v206;  // esi
    unsigned int v207;  // eax
    unsigned int v208;  // 4125
    unsigned int v20;  // eax
    unsigned int v209;  // esi
    unsigned int v210;  // eax
    unsigned int v211;  // 4125
    unsigned int v212;  // esi
    unsigned int v213;  // eax
    unsigned int v214;  // esi
    unsigned int v215;  // 4110
    unsigned int v216;  // eax
    unsigned int v217;  // esi
    unsigned int v218;  // 4125
    unsigned int v21;  // 4125
    unsigned int v219;  // esi
    unsigned int v220;  // eax
    unsigned int v221;  // 4125
    unsigned int v222;  // esi
    unsigned int v223;  // eax
    unsigned int v224;  // 4125
    unsigned int v225;  // esi
    unsigned int v226;  // eax
    unsigned int v227;  // esi
    unsigned int v228;  // 4110
    unsigned int v22;  // esi
    unsigned int v229;  // esi
    unsigned int v230;  // eax
    unsigned int v231;  // 4125
    unsigned int v232;  // esi
    unsigned int v233;  // eax
    unsigned int v234;  // 4125
    unsigned int v235;  // esi
    unsigned int v236;  // eax
    unsigned int v237;  // 4125
    unsigned int v238;  // esi
    unsigned int v23;  // eax
    unsigned int v239;  // eax
    unsigned int v240;  // esi
    unsigned int v241;  // 4110
    unsigned int v242;  // esi
    unsigned int v243;  // eax
    unsigned int v244;  // 4125
    unsigned int v245;  // esi
    unsigned int v246;  // eax
    unsigned int v247;  // 4125
    unsigned int v248;  // esi
    unsigned int v24;  // 4125
    unsigned int v249;  // eax
    unsigned int v250;  // 4125
    unsigned int v251;  // esi
    unsigned int v252;  // eax
    unsigned int v253;  // esi
    unsigned int v254;  // 4110
    unsigned int v255;  // esi
    unsigned int v256;  // eax
    unsigned int v257;  // 4125
    unsigned int v258;  // esi
    unsigned int v25;  // esi
    unsigned int v259;  // eax
    unsigned int v260;  // 4125
    unsigned int v261;  // esi
    unsigned int v262;  // eax
    unsigned int v263;  // 4125
    unsigned int v264;  // esi
    unsigned int v265;  // eax
    unsigned int v266;  // esi
    unsigned int v267;  // 4110
    unsigned int v268;  // esi
    unsigned int v26;  // eax
    unsigned int v269;  // eax
    unsigned int v270;  // 4125
    unsigned int v271;  // esi
    unsigned int v272;  // eax
    unsigned int v273;  // 4125
    unsigned int v274;  // esi
    unsigned int v275;  // eax
    unsigned int v276;  // 4125
    unsigned int v277;  // esi
    unsigned int v278;  // eax
    unsigned int v27;  // 4125
    unsigned int v279;  // esi
    unsigned int v280;  // 4110
    unsigned int v281;  // esi
    unsigned int v282;  // eax
    unsigned int v283;  // 4125
    unsigned int v284;  // esi
    unsigned int v285;  // eax
    unsigned int v286;  // 4125
    unsigned int v287;  // esi
    unsigned int v288;  // eax
    unsigned int v28;  // esi
    unsigned int v289;  // 4125
    unsigned int v290;  // esi
    unsigned int v291;  // eax
    unsigned int v292;  // esi
    unsigned int v293;  // 4110
    unsigned int v294;  // esi
    unsigned int v295;  // eax
    unsigned int v296;  // 4125
    unsigned int v297;  // esi
    unsigned int v298;  // eax
    unsigned int v29;  // eax
    unsigned int v299;  // 4125
    unsigned int v300;  // esi
    unsigned int v301;  // eax
    unsigned int v302;  // 4125
    unsigned int v303;  // esi
    unsigned int v304;  // eax
    unsigned int v305;  // esi
    unsigned int v306;  // 4110
    unsigned int v307;  // eax
    unsigned int v308;  // esi
    unsigned int v30;  // esi
    unsigned int v309;  // 4125
    unsigned int v310;  // esi
    unsigned int v311;  // eax
    unsigned int v312;  // 4125
    unsigned int v313;  // esi
    unsigned int v314;  // eax
    unsigned int v315;  // 4125
    unsigned int v316;  // esi
    unsigned int v317;  // eax
    unsigned int v318;  // esi
    unsigned int v31;  // 4110
    unsigned int v319;  // 4110
    unsigned int v320;  // esi
    unsigned int v321;  // eax
    unsigned int v322;  // 4125
    unsigned int v323;  // esi
    unsigned int v324;  // eax
    unsigned int v325;  // 4125
    unsigned int v326;  // esi
    unsigned int v327;  // eax
    unsigned int v328;  // 4125
    unsigned int v32;  // esi
    unsigned int v329;  // esi
    unsigned int v330;  // eax
    unsigned int v331;  // esi
    unsigned int v332;  // 4110
    unsigned int v333;  // esi
    unsigned int v334;  // eax
    unsigned int v335;  // 4125
    unsigned int v336;  // esi
    unsigned int v337;  // eax
    unsigned int v338;  // 4125
    unsigned int v33;  // eax
    unsigned int v339;  // esi
    unsigned int v340;  // eax
    unsigned int v341;  // 4125
    unsigned int v342;  // esi
    unsigned int v343;  // eax
    unsigned int v344;  // esi
    unsigned int v345;  // 4110
    unsigned int v346;  // esi
    unsigned int v347;  // eax
    unsigned int v348;  // 4125
    unsigned int v34;  // 4125
    unsigned int v349;  // esi
    unsigned int v350;  // eax
    unsigned int v351;  // 4125
    unsigned int v352;  // esi
    unsigned int v353;  // eax
    unsigned int v354;  // 4125
    unsigned int v355;  // esi
    unsigned int v356;  // eax
    unsigned int v357;  // esi
    unsigned int v358;  // 4110
    unsigned int v35;  // esi
    unsigned int v359;  // esi
    unsigned int v360;  // eax
    unsigned int v361;  // 4125
    unsigned int v362;  // esi
    unsigned int v363;  // eax
    unsigned int v364;  // 4125
    unsigned int v365;  // esi
    unsigned int v366;  // eax
    unsigned int v367;  // 4125
    unsigned int v368;  // esi
    unsigned int v36;  // eax
    unsigned int v369;  // eax
    unsigned int v370;  // esi
    unsigned int v371;  // 4110
    unsigned int v372;  // esi
    unsigned int v373;  // eax
    unsigned int v374;  // 4125
    unsigned int v375;  // esi
    unsigned int v376;  // eax
    unsigned int v377;  // 4125
    unsigned int v378;  // esi
    unsigned int v37;  // 4125
    unsigned int v379;  // eax
    unsigned int v380;  // 4125
    unsigned int v381;  // esi
    unsigned int v382;  // eax
    unsigned int v383;  // esi
    unsigned int v384;  // 4110
    unsigned int v385;  // eax
    unsigned int v386;  // esi
    unsigned int v387;  // 4125
    unsigned int v388;  // esi
    unsigned int v38;  // esi
    unsigned int v389;  // eax
    unsigned int v390;  // 4125
    unsigned int v391;  // esi
    unsigned int v392;  // eax
    unsigned int v393;  // 4125
    unsigned int v394;  // esi
    unsigned int v395;  // eax
    unsigned int v396;  // esi
    unsigned int v397;  // 4110
    unsigned int v398;  // esi
    void* v4;  // ecx
    unsigned int v39;  // eax
    unsigned int v399;  // eax
    unsigned int v400;  // 4125
    unsigned int v401;  // esi
    unsigned int v402;  // eax
    unsigned int v403;  // 4125
    unsigned int v404;  // esi
    unsigned int v405;  // eax
    unsigned int v406;  // 4125
    unsigned int v407;  // esi
    unsigned int v408;  // eax
    unsigned int v40;  // 4125
    unsigned int v409;  // esi
    unsigned int v410;  // 4110
    unsigned int v411;  // esi
    unsigned int v412;  // eax
    unsigned int v413;  // 4125
    unsigned int v414;  // esi
    unsigned int v415;  // eax
    unsigned int v416;  // 4125
    unsigned int v417;  // esi
    unsigned int v418;  // eax
    unsigned int v41;  // esi
    unsigned int v419;  // 4125
    unsigned int v420;  // esi
    unsigned int v421;  // eax
    unsigned int v422;  // esi
    unsigned int v423;  // 4110
    unsigned int v424;  // esi
    unsigned int v425;  // eax
    unsigned int v426;  // 4125
    unsigned int v427;  // esi
    unsigned int v428;  // eax
    unsigned int v42;  // eax
    unsigned int v429;  // 4125
    unsigned int v430;  // esi
    unsigned int v431;  // eax
    unsigned int v432;  // 4125
    unsigned int v433;  // esi
    unsigned int v434;  // eax
    unsigned int v435;  // esi
    unsigned int v436;  // 4110
    unsigned int v437;  // esi
    unsigned int v438;  // eax
    unsigned int v43;  // esi
    unsigned int v439;  // 4125
    unsigned int v440;  // esi
    unsigned int v441;  // eax
    unsigned int v442;  // 4125
    unsigned int v443;  // esi
    unsigned int v444;  // eax
    unsigned int v445;  // 4125
    unsigned int v446;  // esi
    unsigned int v447;  // eax
    unsigned int v448;  // esi
    unsigned int v44;  // 4110
    unsigned int v449;  // 4110
    unsigned int v450;  // esi
    unsigned int v451;  // eax
    unsigned int v452;  // 4125
    unsigned int v453;  // esi
    unsigned int v454;  // eax
    unsigned int v455;  // 4125
    unsigned int v456;  // esi
    unsigned int v457;  // eax
    unsigned int v458;  // 4125
    unsigned int v45;  // esi
    unsigned int v459;  // esi
    unsigned int v460;  // eax
    unsigned int v461;  // esi
    unsigned int v462;  // 4110
    unsigned int v463;  // esi
    unsigned int v464;  // eax
    unsigned int v465;  // 4125
    unsigned int v466;  // esi
    unsigned int v467;  // eax
    unsigned int v468;  // 4125
    unsigned int v46;  // eax
    unsigned int v469;  // esi
    unsigned int v470;  // eax
    unsigned int v471;  // 4125
    unsigned int v472;  // esi
    unsigned int v473;  // eax
    unsigned int v474;  // esi
    unsigned int v475;  // 4110
    unsigned int v476;  // esi
    unsigned int v477;  // eax
    unsigned int v478;  // 4125
    unsigned int v47;  // 4125
    unsigned int v479;  // esi
    unsigned int v480;  // eax
    unsigned int v481;  // 4110
    unsigned int v48;  // esi
    unsigned int v49;  // eax
    unsigned int v50;  // 4125
    unsigned int v51;  // esi
    unsigned int v52;  // eax
    unsigned int v53;  // 4125
    unsigned int v54;  // esi
    unsigned int v55;  // eax
    unsigned int v56;  // esi
    unsigned int v57;  // 4110
    unsigned int v58;  // esi
    void* v5;  // edx
    unsigned int v59;  // eax
    unsigned int v60;  // 4125
    unsigned int v61;  // esi
    unsigned int v62;  // eax
    unsigned int v63;  // 4125
    unsigned int v64;  // esi
    unsigned int v65;  // eax
    unsigned int v66;  // 4125
    unsigned int v67;  // esi
    unsigned int v68;  // eax
    unsigned int v6;  // esi
    unsigned int v69;  // esi
    unsigned int v70;  // 4110
    unsigned int v71;  // esi
    unsigned int v72;  // eax
    unsigned int v73;  // 4125
    unsigned int v74;  // esi
    unsigned int v75;  // eax
    unsigned int v76;  // 4125
    unsigned int v77;  // esi
    unsigned int v78;  // eax
    unsigned int v7;  // eax
    unsigned int v79;  // 4125
    unsigned int v80;  // esi
    unsigned int v81;  // eax
    unsigned int v82;  // esi
    unsigned int v83;  // 4110
    unsigned int v84;  // esi
    unsigned int v85;  // eax
    unsigned int v86;  // 4125
    unsigned int v87;  // esi
    unsigned int v88;  // eax
    unsigned int v8;  // 4125
    unsigned int v89;  // 4125
    unsigned int v90;  // esi
    unsigned int v91;  // eax
    unsigned int v92;  // 4125
    unsigned int v93;  // esi
    unsigned int v94;  // eax
    unsigned int v95;  // esi
    unsigned int v96;  // 4110
    unsigned int v97;  // esi
    unsigned int v98;  // eax
    unsigned int v0;  // [bp-0x8]

    for (v0 = v2; i >= 32; i -= 32)
    {
        if (*((int *)v4) != *((int *)v5))
        {
            v6 = (char)*((int *)v4);
            v7 = *((char *)v5);
            if (v6 != v7)
            {
                v8 = _ccall(15, 15, v6 - v7, 0, 0);
                if ((v8 & 1) * 2 != 1)
                    return;
            }
            v9 = (char)v4[1];
            v10 = (char)v5[1];
            if (v9 != v10)
            {
                v11 = _ccall(15, 15, v9 - v10, 0, 0);
                if ((v11 & 1) * 2 != 1)
                    return;
            }
            v12 = (char)v4[2];
            v13 = (char)v5[2];
            if (v12 != v13)
            {
                v14 = _ccall(15, 15, v12 - v13, 0, 0);
                if ((v14 & 1) * 2 != 1)
                    return;
            }
            v15 = (char)v4[3];
            v16 = (char)v5[3];
            v17 = v15 - v16;
            if (v15 != v16)
            {
                v18 = _ccall(15, 15, v17, 0, 0);
                v17 = (v18 & 1) * 2 - 1;
            }
        }
        else
        {
            v17 = 0;
        }
        if (v17)
            return;
        if ((int)v4[4] != (int)v5[4])
        {
            v19 = (char)(int)v4[4];
            v20 = (char)v5[4];
            if (v19 != v20)
            {
                v21 = _ccall(15, 15, v19 - v20, 0, 0);
                if ((v21 & 1) * 2 != 1)
                    return;
            }
            v22 = (char)v4[5];
            v23 = (char)v5[5];
            if (v22 != v23)
            {
                v24 = _ccall(15, 15, v22 - v23, 0, 0);
                if ((v24 & 1) * 2 != 1)
                    return;
            }
            v25 = (char)v4[6];
            v26 = (char)v5[6];
            if (v25 != v26)
            {
                v27 = _ccall(15, 15, v25 - v26, 0, 0);
                if ((v27 & 1) * 2 != 1)
                    return;
            }
            v28 = (char)v4[7];
            v29 = (char)v5[7];
            v30 = v28 - v29;
            if (v28 != v29)
            {
                v31 = _ccall(15, 15, v30, 0, 0);
                v30 = (v31 & 1) * 2 - 1;
            }
        }
        else
        {
            v30 = 0;
        }
        if (v30)
            return;
        if ((int)v4[8] != (int)v5[8])
        {
            v32 = (char)(int)v4[8];
            v33 = (char)v5[8];
            if (v32 != v33)
            {
                v34 = _ccall(15, 15, v32 - v33, 0, 0);
                if ((v34 & 1) * 2 != 1)
                    return;
            }
            v35 = (char)v4[9];
            v36 = (char)v5[9];
            if (v35 != v36)
            {
                v37 = _ccall(15, 15, v35 - v36, 0, 0);
                if ((v37 & 1) * 2 != 1)
                    return;
            }
            v38 = (char)v4[10];
            v39 = (char)v5[10];
            if (v38 != v39)
            {
                v40 = _ccall(15, 15, v38 - v39, 0, 0);
                if ((v40 & 1) * 2 != 1)
                    return;
            }
            v41 = (char)v4[11];
            v42 = (char)v5[11];
            v43 = v41 - v42;
            if (v41 != v42)
            {
                v44 = _ccall(15, 15, v43, 0, 0);
                v43 = (v44 & 1) * 2 - 1;
            }
        }
        else
        {
            v43 = 0;
        }
        if (v43)
            return;
        if ((int)v4[12] != (int)v5[12])
        {
            v45 = (char)(int)v4[12];
            v46 = (char)v5[12];
            if (v45 != v46)
            {
                v47 = _ccall(15, 15, v45 - v46, 0, 0);
                if ((v47 & 1) * 2 != 1)
                    return;
            }
            v48 = (char)v4[13];
            v49 = (char)v5[13];
            if (v48 != v49)
            {
                v50 = _ccall(15, 15, v48 - v49, 0, 0);
                if ((v50 & 1) * 2 != 1)
                    return;
            }
            v51 = (char)v4[14];
            v52 = (char)v5[14];
            if (v51 != v52)
            {
                v53 = _ccall(15, 15, v51 - v52, 0, 0);
                if ((v53 & 1) * 2 != 1)
                    return;
            }
            v54 = (char)v4[15];
            v55 = (char)v5[15];
            v56 = v54 - v55;
            if (v54 != v55)
            {
                v57 = _ccall(15, 15, v56, 0, 0);
                v56 = (v57 & 1) * 2 - 1;
            }
        }
        else
        {
            v56 = 0;
        }
        if (v56)
            return;
        if ((int)v4[16] != (int)v5[16])
        {
            v58 = (char)(int)v4[16];
            v59 = (char)v5[16];
            if (v58 != v59)
            {
                v60 = _ccall(15, 15, v58 - v59, 0, 0);
                if ((v60 & 1) * 2 != 1)
                    return;
            }
            v61 = (char)v4[0x11];
            v62 = (char)v5[0x11];
            if (v61 != v62)
            {
                v63 = _ccall(15, 15, v61 - v62, 0, 0);
                if ((v63 & 1) * 2 != 1)
                    return;
            }
            v64 = (char)v4[18];
            v65 = (char)v5[18];
            if (v64 != v65)
            {
                v66 = _ccall(15, 15, v64 - v65, 0, 0);
                if ((v66 & 1) * 2 != 1)
                    return;
            }
            v67 = (char)v4[19];
            v68 = (char)v5[19];
            v69 = v67 - v68;
            if (v67 != v68)
            {
                v70 = _ccall(15, 15, v69, 0, 0);
                v69 = (v70 & 1) * 2 - 1;
            }
        }
        else
        {
            v69 = 0;
        }
        if (v69)
            return;
        if ((int)v4[20] != (int)v5[20])
        {
            v71 = (char)(int)v4[20];
            v72 = (char)v5[20];
            if (v71 != v72)
            {
                v73 = _ccall(15, 15, v71 - v72, 0, 0);
                if ((v73 & 1) * 2 != 1)
                    return;
            }
            v74 = (char)v4[21];
            v75 = (char)v5[21];
            if (v74 != v75)
            {
                v76 = _ccall(15, 15, v74 - v75, 0, 0);
                if ((v76 & 1) * 2 != 1)
                    return;
            }
            v77 = (char)v4[22];
            v78 = (char)v5[22];
            if (v77 != v78)
            {
                v79 = _ccall(15, 15, v77 - v78, 0, 0);
                if ((v79 & 1) * 2 != 1)
                    return;
            }
            v80 = (char)v4[23];
            v81 = (char)v5[23];
            v82 = v80 - v81;
            if (v80 != v81)
            {
                v83 = _ccall(15, 15, v82, 0, 0);
                v82 = (v83 & 1) * 2 - 1;
            }
        }
        else
        {
            v82 = 0;
        }
        if (v82)
            return;
        if ((int)v4[24] != (int)v5[24])
        {
            v84 = (char)(int)v4[24];
            v85 = (char)v5[24];
            if (v84 != v85)
            {
                v86 = _ccall(15, 15, v84 - v85, 0, 0);
                if ((v86 & 1) * 2 != 1)
                    return;
            }
            v87 = (char)v4[25];
            v88 = (char)v5[25];
            if (v87 != v88)
            {
                v89 = _ccall(15, 15, v87 - v88, 0, 0);
                if ((v89 & 1) * 2 != 1)
                    return;
            }
            v90 = (char)v4[26];
            v91 = (char)v5[26];
            if (v90 != v91)
            {
                v92 = _ccall(15, 15, v90 - v91, 0, 0);
                if ((v92 & 1) * 2 != 1)
                    return;
            }
            v93 = (char)v4[27];
            v94 = (char)v5[27];
            v95 = v93 - v94;
            if (v93 != v94)
            {
                v96 = _ccall(15, 15, v95, 0, 0);
                v95 = (v96 & 1) * 2 - 1;
            }
        }
        else
        {
            v95 = 0;
        }
        if (v95)
            return;
        if ((int)v4[28] != (int)v5[28])
        {
            v97 = (char)(int)v4[28];
            v98 = (char)v5[28];
            if (v97 != v98)
            {
                v99 = _ccall(15, 15, v97 - v98, 0, 0);
                if ((v99 & 1) * 2 != 1)
                    return;
            }
            v100 = (char)v4[29];
            v101 = (char)v5[29];
            if (v100 != v101)
            {
                v102 = _ccall(15, 15, v100 - v101, 0, 0);
                if ((v102 & 1) * 2 != 1)
                    return;
            }
            v103 = (char)v4[30];
            v104 = (char)v5[30];
            if (v103 != v104)
            {
                v105 = _ccall(15, 15, v103 - v104, 0, 0);
                if ((v105 & 1) * 2 != 1)
                    return;
            }
            v106 = (char)v4[31];
            v107 = (char)v5[31];
            v108 = v106 - v107;
            if (v106 != v107)
            {
                v109 = _ccall(15, 15, v108, 0, 0);
                v108 = (v109 & 1) * 2 - 1;
            }
        }
        else
        {
            v108 = 0;
        }
        if (v108)
            return;
        v4 += 32;
        v5 += 32;
    }
    v110 = v4 + i;
    v111 = v5 + i;
    switch (i)
    {
    case 1:
        if (*((char *)v110 - 1) == *((char *)v111 - 1))
            return;
        return;
    case 2:
        if (*((short *)((char *)v110 - 2)) == *((short *)((char *)v111 - 2)))
            goto LABEL_486b12;
        else
            goto LABEL_4876c5;
    case 3:
        v476 = *((char *)v110 - 3);
        v477 = *((char *)v111 - 3);
        if (v476 != v477)
        {
            v478 = _ccall(15, 15, v476 - v477, 0, 0);
            if ((v478 & 1) * 2 != 1)
                return;
        }
        break;
    case 4:
        if (*((int *)((char *)v110 - 4)) != *((int *)((char *)v111 - 4)))
        {
            v190 = (char)*((int *)((char *)v110 - 4));
            v191 = *((char *)v111 - 4);
            if (v190 != v191)
            {
                v192 = _ccall(15, 15, v190 - v191, 0, 0);
                if ((v192 & 1) * 2 != 1)
                    return;
            }
            v193 = *((char *)v110 - 3);
            v194 = *((char *)v111 - 3);
            if (v193 != v194)
            {
                v195 = _ccall(15, 15, v193 - v194, 0, 0);
                if ((v195 & 1) * 2 != 1)
                    return;
            }
            v196 = *((char *)v110 - 2);
            v197 = *((char *)v111 - 2);
            if (v196 != v197)
            {
                v198 = _ccall(15, 15, v196 - v197, 0, 0);
                if ((v198 & 1) * 2 != 1)
                    return;
            }
            v199 = *((char *)v110 - 1);
            v200 = *((char *)v111 - 1);
            v201 = v199 - v200;
            if (v199 != v200)
            {
                v202 = _ccall(15, 15, v201, 0, 0);
                v201 = (v202 & 1) * 2 - 1;
            }
        }
        else
        {
            v201 = 0;
        }
        if (v201)
            return;
        break;
    case 5:
        if (*((int *)((char *)v110 - 5)) != *((int *)((char *)v111 - 5)))
        {
            v281 = (char)*((int *)((char *)v110 - 5));
            v282 = *((char *)v111 - 5);
            if (v281 != v282)
            {
                v283 = _ccall(15, 15, v281 - v282, 0, 0);
                if ((v283 & 1) * 2 != 1)
                    return;
            }
            v284 = *((char *)v110 - 4);
            v285 = *((char *)v111 - 4);
            if (v284 != v285)
            {
                v286 = _ccall(15, 15, v284 - v285, 0, 0);
                if ((v286 & 1) * 2 != 1)
                    return;
            }
            v287 = *((char *)v110 - 3);
            v288 = *((char *)v111 - 3);
            if (v287 != v288)
            {
                v289 = _ccall(15, 15, v287 - v288, 0, 0);
                if ((v289 & 1) * 2 != 1)
                    return;
            }
            v290 = *((char *)v110 - 2);
            v291 = *((char *)v111 - 2);
            v292 = v290 - v291;
            if (v290 != v291)
            {
                v293 = _ccall(15, 15, v292, 0, 0);
                v292 = (v293 & 1) * 2 - 1;
            }
            goto LABEL_486ecd;
        }
        else
        {
            v292 = 0;
            goto LABEL_486ecd;
        }
    case 6:
        if (*((int *)((char *)v110 - 6)) != *((int *)((char *)v111 - 6)))
        {
            v372 = (char)*((int *)((char *)v110 - 6));
            v373 = *((char *)v111 - 6);
            if (v372 != v373)
            {
                v374 = _ccall(15, 15, v372 - v373, 0, 0);
                if ((v374 & 1) * 2 != 1)
                    return;
            }
            v375 = *((char *)v110 - 5);
            v376 = *((char *)v111 - 5);
            if (v375 != v376)
            {
                v377 = _ccall(15, 15, v375 - v376, 0, 0);
                if ((v377 & 1) * 2 != 1)
                    return;
            }
            v378 = *((char *)v110 - 4);
            v379 = *((char *)v111 - 4);
            if (v378 != v379)
            {
                v380 = _ccall(15, 15, v378 - v379, 0, 0);
                if ((v380 & 1) * 2 != 1)
                    return;
            }
            v381 = *((char *)v110 - 3);
            v382 = *((char *)v111 - 3);
            v383 = v381 - v382;
            if (v381 != v382)
            {
                v384 = _ccall(15, 15, v383, 0, 0);
                v383 = (v384 & 1) * 2 - 1;
            }
        }
        else
        {
            v383 = 0;
        }
        if (v383)
            return;
        break;
    case 7:
        if (*((int *)((char *)v110 - 7)) != *((int *)((char *)v111 - 7)))
        {
            v463 = (char)*((int *)((char *)v110 - 7));
            v464 = *((char *)v111 - 7);
            if (v463 != v464)
            {
                v465 = _ccall(15, 15, v463 - v464, 0, 0);
                if ((v465 & 1) * 2 != 1)
                    return;
            }
            v466 = *((char *)v110 - 6);
            v467 = *((char *)v111 - 6);
            if (v466 != v467)
            {
                v468 = _ccall(15, 15, v466 - v467, 0, 0);
                if ((v468 & 1) * 2 != 1)
                    return;
            }
            v469 = *((char *)v110 - 5);
            v470 = *((char *)v111 - 5);
            if (v469 != v470)
            {
                v471 = _ccall(15, 15, v469 - v470, 0, 0);
                if ((v471 & 1) * 2 != 1)
                    return;
            }
            v472 = *((char *)v110 - 4);
            v473 = *((char *)v111 - 4);
            v474 = v472 - v473;
            if (v472 != v473)
            {
                v475 = _ccall(15, 15, v474, 0, 0);
                v474 = (v475 & 1) * 2 - 1;
            }
        }
        else
        {
            v474 = 0;
        }
        if (v474)
            return;
        break;
    case 8:
        if (*((int *)((char *)v110 - 8)) != *((int *)((char *)v111 - 8)))
        {
            v177 = (char)*((int *)((char *)v110 - 8));
            v178 = *((char *)v111 - 8);
            if (v177 != v178)
            {
                v179 = _ccall(15, 15, v177 - v178, 0, 0);
                if ((v179 & 1) * 2 != 1)
                    return;
            }
            v180 = *((char *)v110 - 7);
            v181 = *((char *)v111 - 7);
            if (v180 != v181)
            {
                v182 = _ccall(15, 15, v180 - v181, 0, 0);
                if ((v182 & 1) * 2 != 1)
                    return;
            }
            v183 = *((char *)v110 - 6);
            v184 = *((char *)v111 - 6);
            if (v183 != v184)
            {
                v185 = _ccall(15, 15, v183 - v184, 0, 0);
                if ((v185 & 1) * 2 != 1)
                    return;
            }
            v186 = *((char *)v110 - 5);
            v187 = *((char *)v111 - 5);
            v188 = v186 - v187;
            if (v186 != v187)
            {
                v189 = _ccall(15, 15, v188, 0, 0);
                v188 = (v189 & 1) * 2 - 1;
            }
        }
        else
        {
            v188 = 0;
        }
        if (v188)
            return;
        break;
    case 9:
        if (*((int *)((char *)v110 - 9)) != *((int *)((char *)v111 - 9)))
        {
            v268 = (char)*((int *)((char *)v110 - 9));
            v269 = *((char *)v111 - 9);
            if (v268 != v269)
            {
                v270 = _ccall(15, 15, v268 - v269, 0, 0);
                if ((v270 & 1) * 2 != 1)
                    return;
            }
            v271 = *((char *)v110 - 8);
            v272 = *((char *)v111 - 8);
            if (v271 != v272)
            {
                v273 = _ccall(15, 15, v271 - v272, 0, 0);
                if ((v273 & 1) * 2 != 1)
                    return;
            }
            v274 = *((char *)v110 - 7);
            v275 = *((char *)v111 - 7);
            if (v274 != v275)
            {
                v276 = _ccall(15, 15, v274 - v275, 0, 0);
                if ((v276 & 1) * 2 != 1)
                    return;
            }
            v277 = *((char *)v110 - 6);
            v278 = *((char *)v111 - 6);
            v279 = v277 - v278;
            if (v277 != v278)
            {
                v280 = _ccall(15, 15, v279, 0, 0);
                v279 = (v280 & 1) * 2 - 1;
            }
        }
        else
        {
            v279 = 0;
        }
        if (v279)
            return;
        break;
    case 10:
        if (*((int *)((char *)v110 - 10)) != *((int *)((char *)v111 - 10)))
        {
            v359 = (char)*((int *)((char *)v110 - 10));
            v360 = *((char *)v111 - 10);
            if (v359 != v360)
            {
                v361 = _ccall(15, 15, v359 - v360, 0, 0);
                if ((v361 & 1) * 2 != 1)
                    return;
            }
            v362 = *((char *)v110 - 9);
            v363 = *((char *)v111 - 9);
            if (v362 != v363)
            {
                v364 = _ccall(15, 15, v362 - v363, 0, 0);
                if ((v364 & 1) * 2 != 1)
                    return;
            }
            v365 = *((char *)v110 - 8);
            v366 = *((char *)v111 - 8);
            if (v365 != v366)
            {
                v367 = _ccall(15, 15, v365 - v366, 0, 0);
                if ((v367 & 1) * 2 != 1)
                    return;
            }
            v368 = *((char *)v110 - 7);
            v369 = *((char *)v111 - 7);
            v370 = v368 - v369;
            if (v368 != v369)
            {
                v371 = _ccall(15, 15, v370, 0, 0);
                v370 = (v371 & 1) * 2 - 1;
            }
        }
        else
        {
            v370 = 0;
        }
        if (v370)
            return;
        break;
    case 11:
        if (*((int *)((char *)v110 - 11)) != *((int *)((char *)v111 - 11)))
        {
            v450 = (char)*((int *)((char *)v110 - 11));
            v451 = *((char *)v111 - 11);
            if (v450 != v451)
            {
                v452 = _ccall(15, 15, v450 - v451, 0, 0);
                if ((v452 & 1) * 2 != 1)
                    return;
            }
            v453 = *((char *)v110 - 10);
            v454 = *((char *)v111 - 10);
            if (v453 != v454)
            {
                v455 = _ccall(15, 15, v453 - v454, 0, 0);
                if ((v455 & 1) * 2 != 1)
                    return;
            }
            v456 = *((char *)v110 - 9);
            v457 = *((char *)v111 - 9);
            if (v456 != v457)
            {
                v458 = _ccall(15, 15, v456 - v457, 0, 0);
                if ((v458 & 1) * 2 != 1)
                    return;
            }
            v459 = *((char *)v110 - 8);
            v460 = *((char *)v111 - 8);
            v461 = v459 - v460;
            if (v459 != v460)
            {
                v462 = _ccall(15, 15, v461, 0, 0);
                v461 = (v462 & 1) * 2 - 1;
            }
        }
        else
        {
            v461 = 0;
        }
        if (v461)
            return;
        break;
    case 12:
        if (*((int *)((char *)v110 - 12)) != *((int *)((char *)v111 - 12)))
        {
            v164 = (char)*((int *)((char *)v110 - 12));
            v165 = *((char *)v111 - 12);
            if (v164 != v165)
            {
                v166 = _ccall(15, 15, v164 - v165, 0, 0);
                if ((v166 & 1) * 2 != 1)
                    return;
            }
            v167 = *((char *)v110 - 11);
            v168 = *((char *)v111 - 11);
            if (v167 != v168)
            {
                v169 = _ccall(15, 15, v167 - v168, 0, 0);
                if ((v169 & 1) * 2 != 1)
                    return;
            }
            v170 = *((char *)v110 - 10);
            v171 = *((char *)v111 - 10);
            if (v170 != v171)
            {
                v172 = _ccall(15, 15, v170 - v171, 0, 0);
                if ((v172 & 1) * 2 != 1)
                    return;
            }
            v173 = *((char *)v110 - 9);
            v174 = *((char *)v111 - 9);
            v175 = v173 - v174;
            if (v173 != v174)
            {
                v176 = _ccall(15, 15, v175, 0, 0);
                v175 = (v176 & 1) * 2 - 1;
            }
        }
        else
        {
            v175 = 0;
        }
        if (v175)
            return;
        break;
    case 13:
        if (*((int *)((char *)v110 - 13)) != *((int *)((char *)v111 - 13)))
        {
            v255 = (char)*((int *)((char *)v110 - 13));
            v256 = *((char *)v111 - 13);
            if (v255 != v256)
            {
                v257 = _ccall(15, 15, v255 - v256, 0, 0);
                if ((v257 & 1) * 2 != 1)
                    return;
            }
            v258 = *((char *)v110 - 12);
            v259 = *((char *)v111 - 12);
            if (v258 != v259)
            {
                v260 = _ccall(15, 15, v258 - v259, 0, 0);
                if ((v260 & 1) * 2 != 1)
                    return;
            }
            v261 = *((char *)v110 - 11);
            v262 = *((char *)v111 - 11);
            if (v261 != v262)
            {
                v263 = _ccall(15, 15, v261 - v262, 0, 0);
                if ((v263 & 1) * 2 != 1)
                    return;
            }
            v264 = *((char *)v110 - 10);
            v265 = *((char *)v111 - 10);
            v266 = v264 - v265;
            if (v264 != v265)
            {
                v267 = _ccall(15, 15, v266, 0, 0);
                v266 = (v267 & 1) * 2 - 1;
            }
        }
        else
        {
            v266 = 0;
        }
        if (v266)
            return;
        break;
    case 14:
        if (*((int *)((char *)v110 - 14)) != *((int *)((char *)v111 - 14)))
        {
            v346 = (char)*((int *)((char *)v110 - 14));
            v347 = *((char *)v111 - 14);
            if (v346 != v347)
            {
                v348 = _ccall(15, 15, v346 - v347, 0, 0);
                if ((v348 & 1) * 2 != 1)
                    return;
            }
            v349 = *((char *)v110 - 13);
            v350 = *((char *)v111 - 13);
            if (v349 != v350)
            {
                v351 = _ccall(15, 15, v349 - v350, 0, 0);
                if ((v351 & 1) * 2 != 1)
                    return;
            }
            v352 = *((char *)v110 - 12);
            v353 = *((char *)v111 - 12);
            if (v352 != v353)
            {
                v354 = _ccall(15, 15, v352 - v353, 0, 0);
                if ((v354 & 1) * 2 != 1)
                    return;
            }
            v355 = *((char *)v110 - 11);
            v356 = *((char *)v111 - 11);
            v357 = v355 - v356;
            if (v355 != v356)
            {
                v358 = _ccall(15, 15, v357, 0, 0);
                v357 = (v358 & 1) * 2 - 1;
            }
        }
        else
        {
            v357 = 0;
        }
        if (v357)
            return;
        break;
    case 15:
        if (*((int *)((char *)v110 - 15)) != *((int *)((char *)v111 - 15)))
        {
            v437 = (char)*((int *)((char *)v110 - 15));
            v438 = *((char *)v111 - 15);
            if (v437 != v438)
            {
                v439 = _ccall(15, 15, v437 - v438, 0, 0);
                if ((v439 & 1) * 2 != 1)
                    return;
            }
            v440 = *((char *)v110 - 14);
            v441 = *((char *)v111 - 14);
            if (v440 != v441)
            {
                v442 = _ccall(15, 15, v440 - v441, 0, 0);
                if ((v442 & 1) * 2 != 1)
                    return;
            }
            v443 = *((char *)v110 - 13);
            v444 = *((char *)v111 - 13);
            if (v443 != v444)
            {
                v445 = _ccall(15, 15, v443 - v444, 0, 0);
                if ((v445 & 1) * 2 != 1)
                    return;
            }
            v446 = *((char *)v110 - 12);
            v447 = *((char *)v111 - 12);
            v448 = v446 - v447;
            if (v446 != v447)
            {
                v449 = _ccall(15, 15, v448, 0, 0);
                v448 = (v449 & 1) * 2 - 1;
            }
        }
        else
        {
            v448 = 0;
        }
        if (v448)
            return;
        break;
    case 16:
        if (*((int *)((char *)v110 - 16)) != *((int *)((char *)v111 - 16)))
        {
            v151 = (char)*((int *)((char *)v110 - 16));
            v152 = *((char *)v111 - 16);
            if (v151 != v152)
            {
                v153 = _ccall(15, 15, v151 - v152, 0, 0);
                if ((v153 & 1) * 2 != 1)
                    return;
            }
            v154 = *((char *)v110 - 15);
            v155 = *((char *)v111 - 15);
            if (v154 != v155)
            {
                v156 = _ccall(15, 15, v154 - v155, 0, 0);
                if ((v156 & 1) * 2 != 1)
                    return;
            }
            v157 = *((char *)v110 - 14);
            v158 = *((char *)v111 - 14);
            if (v157 != v158)
            {
                v159 = _ccall(15, 15, v157 - v158, 0, 0);
                if ((v159 & 1) * 2 != 1)
                    return;
            }
            v160 = *((char *)v110 - 13);
            v161 = *((char *)v111 - 13);
            v162 = v160 - v161;
            if (v160 != v161)
            {
                v163 = _ccall(15, 15, v162, 0, 0);
                v162 = (v163 & 1) * 2 - 1;
            }
        }
        else
        {
            v162 = 0;
        }
        if (v162)
            return;
        break;
    case 17:
        if (*((int *)((char *)v110 - 0x11)) != *((int *)((char *)v111 - 0x11)))
        {
            v242 = (char)*((int *)((char *)v110 - 0x11));
            v243 = *((char *)v111 - 0x11);
            if (v242 != v243)
            {
                v244 = _ccall(15, 15, v242 - v243, 0, 0);
                if ((v244 & 1) * 2 != 1)
                    return;
            }
            v245 = *((char *)v110 - 16);
            v246 = *((char *)v111 - 16);
            if (v245 != v246)
            {
                v247 = _ccall(15, 15, v245 - v246, 0, 0);
                if ((v247 & 1) * 2 != 1)
                    return;
            }
            v248 = *((char *)v110 - 15);
            v249 = *((char *)v111 - 15);
            if (v248 != v249)
            {
                v250 = _ccall(15, 15, v248 - v249, 0, 0);
                if ((v250 & 1) * 2 != 1)
                    return;
            }
            v251 = *((char *)v110 - 14);
            v252 = *((char *)v111 - 14);
            v253 = v251 - v252;
            if (v251 != v252)
            {
                v254 = _ccall(15, 15, v253, 0, 0);
                v253 = (v254 & 1) * 2 - 1;
            }
        }
        else
        {
            v253 = 0;
        }
        if (v253)
            return;
        break;
    case 18:
        if (*((int *)((char *)v110 - 18)) != *((int *)((char *)v111 - 18)))
        {
            v333 = (char)*((int *)((char *)v110 - 18));
            v334 = *((char *)v111 - 18);
            if (v333 != v334)
            {
                v335 = _ccall(15, 15, v333 - v334, 0, 0);
                if ((v335 & 1) * 2 != 1)
                    return;
            }
            v336 = *((char *)v110 - 0x11);
            v337 = *((char *)v111 - 0x11);
            if (v336 != v337)
            {
                v338 = _ccall(15, 15, v336 - v337, 0, 0);
                if ((v338 & 1) * 2 != 1)
                    return;
            }
            v339 = *((char *)v110 - 16);
            v340 = *((char *)v111 - 16);
            if (v339 != v340)
            {
                v341 = _ccall(15, 15, v339 - v340, 0, 0);
                if ((v341 & 1) * 2 != 1)
                    return;
            }
            v342 = *((char *)v110 - 15);
            v343 = *((char *)v111 - 15);
            v344 = v342 - v343;
            if (v342 != v343)
            {
                v345 = _ccall(15, 15, v344, 0, 0);
                v344 = (v345 & 1) * 2 - 1;
            }
        }
        else
        {
            v344 = 0;
        }
        if (v344)
            return;
        break;
    case 19:
        if (*((int *)((char *)v110 - 19)) != *((int *)((char *)v111 - 19)))
        {
            v424 = (char)*((int *)((char *)v110 - 19));
            v425 = *((char *)v111 - 19);
            if (v424 != v425)
            {
                v426 = _ccall(15, 15, v424 - v425, 0, 0);
                if ((v426 & 1) * 2 != 1)
                    return;
            }
            v427 = *((char *)v110 - 18);
            v428 = *((char *)v111 - 18);
            if (v427 != v428)
            {
                v429 = _ccall(15, 15, v427 - v428, 0, 0);
                if ((v429 & 1) * 2 != 1)
                    return;
            }
            v430 = *((char *)v110 - 0x11);
            v431 = *((char *)v111 - 0x11);
            if (v430 != v431)
            {
                v432 = _ccall(15, 15, v430 - v431, 0, 0);
                if ((v432 & 1) * 2 != 1)
                    return;
            }
            v433 = *((char *)v110 - 16);
            v434 = *((char *)v111 - 16);
            v435 = v433 - v434;
            if (v433 != v434)
            {
                v436 = _ccall(15, 15, v435, 0, 0);
                v435 = (v436 & 1) * 2 - 1;
            }
        }
        else
        {
            v435 = 0;
        }
        if (v435)
            return;
        break;
    case 20:
        if (*((int *)((char *)v110 - 20)) != *((int *)((char *)v111 - 20)))
        {
            v138 = (char)*((int *)((char *)v110 - 20));
            v139 = *((char *)v111 - 20);
            if (v138 != v139)
            {
                v140 = _ccall(15, 15, v138 - v139, 0, 0);
                if ((v140 & 1) * 2 != 1)
                    return;
            }
            v141 = *((char *)v110 - 19);
            v142 = *((char *)v111 - 19);
            if (v141 != v142)
            {
                v143 = _ccall(15, 15, v141 - v142, 0, 0);
                if ((v143 & 1) * 2 != 1)
                    return;
            }
            v144 = *((char *)v110 - 18);
            v145 = *((char *)v111 - 18);
            if (v144 != v145)
            {
                v146 = _ccall(15, 15, v144 - v145, 0, 0);
                if ((v146 & 1) * 2 != 1)
                    return;
            }
            v147 = *((char *)v110 - 0x11);
            v148 = *((char *)v111 - 0x11);
            v149 = v147 - v148;
            if (v147 != v148)
            {
                v150 = _ccall(15, 15, v149, 0, 0);
                v149 = (v150 & 1) * 2 - 1;
            }
        }
        else
        {
            v149 = 0;
        }
        if (v149)
            return;
        break;
    case 21:
        if (*((int *)((char *)v110 - 21)) != *((int *)((char *)v111 - 21)))
        {
            v229 = (char)*((int *)((char *)v110 - 21));
            v230 = *((char *)v111 - 21);
            if (v229 != v230)
            {
                v231 = _ccall(15, 15, v229 - v230, 0, 0);
                if ((v231 & 1) * 2 != 1)
                    return;
            }
            v232 = *((char *)v110 - 20);
            v233 = *((char *)v111 - 20);
            if (v232 != v233)
            {
                v234 = _ccall(15, 15, v232 - v233, 0, 0);
                if ((v234 & 1) * 2 != 1)
                    return;
            }
            v235 = *((char *)v110 - 19);
            v236 = *((char *)v111 - 19);
            if (v235 != v236)
            {
                v237 = _ccall(15, 15, v235 - v236, 0, 0);
                if ((v237 & 1) * 2 != 1)
                    return;
            }
            v238 = *((char *)v110 - 18);
            v239 = *((char *)v111 - 18);
            v240 = v238 - v239;
            if (v238 != v239)
            {
                v241 = _ccall(15, 15, v240, 0, 0);
                v240 = (v241 & 1) * 2 - 1;
            }
        }
        else
        {
            v240 = 0;
        }
        if (v240)
            return;
        break;
    case 22:
        if (*((int *)((char *)v110 - 22)) != *((int *)((char *)v111 - 22)))
        {
            v320 = (char)*((int *)((char *)v110 - 22));
            v321 = *((char *)v111 - 22);
            if (v320 != v321)
            {
                v322 = _ccall(15, 15, v320 - v321, 0, 0);
                if ((v322 & 1) * 2 != 1)
                    return;
            }
            v323 = *((char *)v110 - 21);
            v324 = *((char *)v111 - 21);
            if (v323 != v324)
            {
                v325 = _ccall(15, 15, v323 - v324, 0, 0);
                if ((v325 & 1) * 2 != 1)
                    return;
            }
            v326 = *((char *)v110 - 20);
            v327 = *((char *)v111 - 20);
            if (v326 != v327)
            {
                v328 = _ccall(15, 15, v326 - v327, 0, 0);
                if ((v328 & 1) * 2 != 1)
                    return;
            }
            v329 = *((char *)v110 - 19);
            v330 = *((char *)v111 - 19);
            v331 = v329 - v330;
            if (v329 != v330)
            {
                v332 = _ccall(15, 15, v331, 0, 0);
                v331 = (v332 & 1) * 2 - 1;
            }
        }
        else
        {
            v331 = 0;
        }
        if (v331)
            return;
        break;
    case 23:
        if (*((int *)((char *)v110 - 23)) != *((int *)((char *)v111 - 23)))
        {
            v411 = (char)*((int *)((char *)v110 - 23));
            v412 = *((char *)v111 - 23);
            if (v411 != v412)
            {
                v413 = _ccall(15, 15, v411 - v412, 0, 0);
                if ((v413 & 1) * 2 != 1)
                    return;
            }
            v414 = *((char *)v110 - 22);
            v415 = *((char *)v111 - 22);
            if (v414 != v415)
            {
                v416 = _ccall(15, 15, v414 - v415, 0, 0);
                if ((v416 & 1) * 2 != 1)
                    return;
            }
            v417 = *((char *)v110 - 21);
            v418 = *((char *)v111 - 21);
            if (v417 != v418)
            {
                v419 = _ccall(15, 15, v417 - v418, 0, 0);
                if ((v419 & 1) * 2 != 1)
                    return;
            }
            v420 = *((char *)v110 - 20);
            v421 = *((char *)v111 - 20);
            v422 = v420 - v421;
            if (v420 != v421)
            {
                v423 = _ccall(15, 15, v422, 0, 0);
                v422 = (v423 & 1) * 2 - 1;
            }
        }
        else
        {
            v422 = 0;
        }
        if (v422)
            return;
        break;
    case 24:
        if (*((int *)((char *)v110 - 24)) != *((int *)((char *)v111 - 24)))
        {
            v125 = (char)*((int *)((char *)v110 - 24));
            v126 = *((char *)v111 - 24);
            if (v125 != v126)
            {
                v127 = _ccall(15, 15, v125 - v126, 0, 0);
                if ((v127 & 1) * 2 != 1)
                    return;
            }
            v128 = *((char *)v110 - 23);
            v129 = *((char *)v111 - 23);
            if (v128 != v129)
            {
                v130 = _ccall(15, 15, v128 - v129, 0, 0);
                if ((v130 & 1) * 2 != 1)
                    return;
            }
            v131 = *((char *)v110 - 22);
            v132 = *((char *)v111 - 22);
            if (v131 != v132)
            {
                v133 = _ccall(15, 15, v131 - v132, 0, 0);
                if ((v133 & 1) * 2 != 1)
                    return;
            }
            v134 = *((char *)v110 - 21);
            v135 = *((char *)v111 - 21);
            v136 = v134 - v135;
            if (v134 != v135)
            {
                v137 = _ccall(15, 15, v136, 0, 0);
                v136 = (v137 & 1) * 2 - 1;
            }
        }
        else
        {
            v136 = 0;
        }
        if (v136)
            return;
        break;
    case 25:
        if (*((int *)((char *)v110 - 25)) != *((int *)((char *)v111 - 25)))
        {
            v216 = *((char *)v111 - 25);
            v217 = *((char *)v110 - 25);
            if (v217 != v216)
            {
                v218 = _ccall(15, 15, v217 - v216, 0, 0);
                if ((v218 & 1) * 2 != 1)
                    return;
            }
            v219 = *((char *)v110 - 24);
            v220 = *((char *)v111 - 24);
            if (v219 != v220)
            {
                v221 = _ccall(15, 15, v219 - v220, 0, 0);
                if ((v221 & 1) * 2 != 1)
                    return;
            }
            v222 = *((char *)v110 - 23);
            v223 = *((char *)v111 - 23);
            if (v222 != v223)
            {
                v224 = _ccall(15, 15, v222 - v223, 0, 0);
                if ((v224 & 1) * 2 != 1)
                    return;
            }
            v225 = *((char *)v110 - 22);
            v226 = *((char *)v111 - 22);
            v227 = v225 - v226;
            if (v225 != v226)
            {
                v228 = _ccall(15, 15, v227, 0, 0);
                v227 = (v228 & 1) * 2 - 1;
            }
        }
        else
        {
            v227 = 0;
        }
        if (v227)
            return;
        break;
    case 26:
        if (*((int *)((char *)v110 - 26)) != *((int *)((char *)v111 - 26)))
        {
            v307 = *((char *)v111 - 26);
            v308 = *((char *)v110 - 26);
            if (v308 != v307)
            {
                v309 = _ccall(15, 15, v308 - v307, 0, 0);
                if ((v309 & 1) * 2 != 1)
                    return;
            }
            v310 = *((char *)v110 - 25);
            v311 = *((char *)v111 - 25);
            if (v310 != v311)
            {
                v312 = _ccall(15, 15, v310 - v311, 0, 0);
                if ((v312 & 1) * 2 != 1)
                    return;
            }
            v313 = *((char *)v110 - 24);
            v314 = *((char *)v111 - 24);
            if (v313 != v314)
            {
                v315 = _ccall(15, 15, v313 - v314, 0, 0);
                if ((v315 & 1) * 2 != 1)
                    return;
            }
            v316 = *((char *)v110 - 23);
            v317 = *((char *)v111 - 23);
            v318 = v316 - v317;
            if (v316 != v317)
            {
                v319 = _ccall(15, 15, v318, 0, 0);
                v318 = (v319 & 1) * 2 - 1;
            }
        }
        else
        {
            v318 = 0;
        }
        if (v318)
            return;
        break;
    case 27:
        if (*((int *)((char *)v110 - 27)) != *((int *)((char *)v111 - 27)))
        {
            v398 = (char)*((int *)((char *)v110 - 27));
            v399 = *((char *)v111 - 27);
            if (v398 != v399)
            {
                v400 = _ccall(15, 15, v398 - v399, 0, 0);
                if ((v400 & 1) * 2 != 1)
                    return;
            }
            v401 = *((char *)v110 - 26);
            v402 = *((char *)v111 - 26);
            if (v401 != v402)
            {
                v403 = _ccall(15, 15, v401 - v402, 0, 0);
                if ((v403 & 1) * 2 != 1)
                    return;
            }
            v404 = *((char *)v110 - 25);
            v405 = *((char *)v111 - 25);
            if (v404 != v405)
            {
                v406 = _ccall(15, 15, v404 - v405, 0, 0);
                if ((v406 & 1) * 2 != 1)
                    return;
            }
            v407 = *((char *)v110 - 24);
            v408 = *((char *)v111 - 24);
            v409 = v407 - v408;
            if (v407 != v408)
            {
                v410 = _ccall(15, 15, v409, 0, 0);
                v409 = (v410 & 1) * 2 - 1;
            }
        }
        else
        {
            v409 = 0;
        }
        if (v409)
            return;
        break;
    case 28:
        if (*((int *)((char *)v110 - 28)) != *((int *)((char *)v111 - 28)))
        {
            v112 = *((char *)v111 - 28);
            v113 = *((char *)v110 - 28);
            if (v113 != v112)
            {
                v114 = _ccall(15, 15, v113 - v112, 0, 0);
                if ((v114 & 1) * 2 != 1)
                    return;
            }
            v115 = *((char *)v110 - 27);
            v116 = *((char *)v111 - 27);
            if (v115 != v116)
            {
                v117 = _ccall(15, 15, v115 - v116, 0, 0);
                if ((v117 & 1) * 2 != 1)
                    return;
            }
            v118 = *((char *)v110 - 26);
            v119 = *((char *)v111 - 26);
            if (v118 != v119)
            {
                v120 = _ccall(15, 15, v118 - v119, 0, 0);
                if ((v120 & 1) * 2 != 1)
                    return;
            }
            v121 = *((char *)v110 - 25);
            v122 = *((char *)v111 - 25);
            v123 = v121 - v122;
            if (v121 != v122)
            {
                v124 = _ccall(15, 15, v123, 0, 0);
                v123 = (v124 & 1) * 2 - 1;
            }
            break;
        }
        else
        {
            v123 = 0;
            break;
        }
        if (v123)
            return;
        break;
    case 29:
        if (*((int *)((char *)v110 - 29)) != *((int *)((char *)v111 - 29)))
        {
            v203 = (char)*((int *)((char *)v110 - 29));
            v204 = *((char *)v111 - 29);
            if (v203 != v204)
            {
                v205 = _ccall(15, 15, v203 - v204, 0, 0);
                if ((v205 & 1) * 2 != 1)
                    return;
            }
            v206 = *((char *)v110 - 28);
            v207 = *((char *)v111 - 28);
            if (v206 != v207)
            {
                v208 = _ccall(15, 15, v206 - v207, 0, 0);
                if ((v208 & 1) * 2 != 1)
                    return;
            }
            v209 = *((char *)v110 - 27);
            v210 = *((char *)v111 - 27);
            if (v209 != v210)
            {
                v211 = _ccall(15, 15, v209 - v210, 0, 0);
                if ((v211 & 1) * 2 != 1)
                    return;
            }
            v212 = *((char *)v110 - 26);
            v213 = *((char *)v111 - 26);
            v214 = v212 - v213;
            if (v212 != v213)
            {
                v215 = _ccall(15, 15, v214, 0, 0);
                v214 = (v215 & 1) * 2 - 1;
            }
        }
        else
        {
            v214 = 0;
        }
        if (v214)
            return;
        break;
    case 30:
        if (*((int *)((char *)v110 - 30)) != *((int *)((char *)v111 - 30)))
        {
            v294 = (char)*((int *)((char *)v110 - 30));
            v295 = *((char *)v111 - 30);
            if (v294 != v295)
            {
                v296 = _ccall(15, 15, v294 - v295, 0, 0);
                if ((v296 & 1) * 2 != 1)
                    return;
            }
            v297 = *((char *)v110 - 29);
            v298 = *((char *)v111 - 29);
            if (v297 != v298)
            {
                v299 = _ccall(15, 15, v297 - v298, 0, 0);
                if ((v299 & 1) * 2 != 1)
                    return;
            }
            v300 = *((char *)v110 - 28);
            v301 = *((char *)v111 - 28);
            if (v300 != v301)
            {
                v302 = _ccall(15, 15, v300 - v301, 0, 0);
                if ((v302 & 1) * 2 != 1)
                    return;
            }
            v303 = *((char *)v110 - 27);
            v304 = *((char *)v111 - 27);
            v305 = v303 - v304;
            if (v303 != v304)
            {
                v306 = _ccall(15, 15, v305, 0, 0);
                v305 = (v306 & 1) * 2 - 1;
            }
        }
        else
        {
            v305 = 0;
        }
        if (v305)
            return;
        break;
    case 31:
        if (*((int *)((char *)v110 - 31)) != *((int *)((char *)v111 - 31)))
        {
            v385 = *((char *)v111 - 31);
            v386 = *((char *)v110 - 31);
            if (v386 != v385)
            {
                v387 = _ccall(15, 15, v386 - v385, 0, 0);
                if ((v387 & 1) * 2 != 1)
                    return;
            }
            v388 = *((char *)v110 - 30);
            v389 = *((char *)v111 - 30);
            if (v388 != v389)
            {
                v390 = _ccall(15, 15, v388 - v389, 0, 0);
                if ((v390 & 1) * 2 != 1)
                    return;
            }
            v391 = *((char *)v110 - 29);
            v392 = *((char *)v111 - 29);
            if (v391 != v392)
            {
                v393 = _ccall(15, 15, v391 - v392, 0, 0);
                if ((v393 & 1) * 2 != 1)
                    return;
            }
            v394 = *((char *)v110 - 28);
            v395 = *((char *)v111 - 28);
            v396 = v394 - v395;
            if (v394 != v395)
            {
                v397 = _ccall(15, 15, v396, 0, 0);
                v396 = (v397 & 1) * 2 - 1;
            }
        }
        else
        {
            v396 = 0;
        }
        if (v396)
            return;
        break;
    default:
LABEL_486b12:
        return;
    }
LABEL_4876c5:
    v479 = *((char *)v110 - 2);
    v480 = *((char *)v111 - 2);
    if (v479 != v480)
    {
        v481 = _ccall(15, 15, v479 - v480, 0, 0);
        v292 = (v481 & 1) * 2 - 1;
LABEL_486ecd:
        if (v292)
            return;
    }
}
