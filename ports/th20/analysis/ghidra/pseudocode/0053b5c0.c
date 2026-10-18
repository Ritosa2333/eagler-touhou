/* Entry: 0x0053b5c0; symbol: FUN_0053b5c0; body bytes: 10716 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_0053b5c0(void *this,float param_1)

{
  undefined4 uVar1;
  int *piVar2;
  uint uVar3;
  void *pvVar4;
  float fVar5;
  undefined4 uVar6;
  undefined4 *puVar7;
  int iVar8;
  float *pfVar9;
  float *pfVar10;
  void *extraout_ECX;
  void *extraout_ECX_00;
  void *extraout_ECX_01;
  void *this_00;
  void *extraout_ECX_02;
  void *this_01;
  void *this_02;
  float10 fVar11;
  float10 fVar12;
  float10 fVar13;
  float10 fVar14;
  float fVar15;
  char cVar16;
  uint local_274;
  undefined4 local_26c;
  undefined4 local_268;
  uint local_24c;
  uint local_248;
  uint local_244;
  uint local_240;
  uint local_23c;
  uint local_238;
  uint local_234;
  uint local_230;
  uint local_22c;
  uint local_228;
  uint local_224;
  uint local_220;
  uint local_21c;
  uint local_218;
  uint local_214;
  float *local_1f0;
  int *local_1e0;
  float local_1d8;
  undefined4 local_1d4;
  undefined4 local_1cc;
  undefined4 local_1c8;
  float local_1c0;
  undefined4 local_1bc;
  float local_1b4;
  float local_1b0;
  float local_1a8 [3];
  float local_19c;
  undefined4 local_198;
  undefined4 local_194;
  float local_190;
  float local_18c;
  uint local_188;
  float local_184;
  float local_180;
  float local_17c;
  uint local_178;
  float local_174;
  float local_170;
  float local_16c;
  uint local_168;
  float local_164;
  float local_160;
  uint local_15c;
  float local_158;
  uint local_154;
  float local_150;
  float local_14c;
  float local_148;
  float local_144;
  float local_140;
  uint local_13c;
  float local_138;
  uint local_134;
  float local_130;
  float local_12c;
  float local_128;
  uint local_124;
  float local_120;
  uint local_11c;
  float local_118;
  float local_114;
  float local_110;
  uint local_10c;
  float local_108;
  float local_104;
  uint local_100;
  float local_fc;
  float local_f8;
  uint local_f4;
  float local_f0;
  float local_ec;
  uint local_e8;
  float local_e4;
  float local_e0;
  uint local_dc;
  float local_d8;
  float local_d4;
  uint local_d0;
  uint local_cc;
  float local_c8;
  float local_c4;
  float local_c0;
  float local_bc;
  uint local_b8;
  float local_b4;
  float local_b0;
  float local_ac;
  uint local_a8;
  float local_a4;
  float local_a0;
  float local_9c;
  uint local_98;
  float local_94;
  float local_90;
  float local_8c;
  float local_88;
  float local_84;
  float local_80;
  float local_7c;
  float local_78;
  float local_74;
  float local_70;
  float local_6c;
  float local_68;
  float local_64;
  float local_60;
  float local_5c;
  float local_58;
  float local_54;
  float local_50;
  float local_4c;
  float local_48;
  float local_44;
  float local_40;
  float local_3c;
  float local_38;
  float local_34;
  float local_30;
  float local_2c;
  float local_28;
  float local_24;
  float local_20;
  float local_1c;
  float local_18;
  uint local_14;
  
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  if ((*(int *)((int)this + 8) != -1) && (*(int *)((int)this + 4) != -1)) {
    local_1e0 = (int *)FUN_005403d0((int)this);
LAB_0053b625:
                    /* WARNING: Load size is inaccurate */
    if ((float)*local_1e0 <= *this) {
      if ((*(byte *)((int)local_1e0 + 10) & *(byte *)((int)this + 0x30)) == 0) goto LAB_0053dfbc;
      switch((short)local_1e0[1]) {
      case 0:
        goto LAB_0053df44;
      case 1:
        goto LAB_0053e0f6;
      default:
        iVar8 = (**(code **)(**(int **)((int)this + 0x28) + 4))();
        if (iVar8 == -1) goto LAB_0053e00c;
        if ((iVar8 == 0) || (iVar8 != 1)) goto LAB_0053df44;
        goto LAB_0053b625;
      case 10:
        FUN_00540300((void *)((int)this + 0xc));
        iVar8 = FUN_00411700((int)this + 0xc);
        if (iVar8 != 0) {
          FUN_0053f0b0((void *)((int)this + 0xc),4,(float *)((int)this + 4),'\0');
          FUN_0053f0b0((void *)((int)this + 0xc),4,(float *)((int)this + 8),'\0');
          FUN_0053f0b0((void *)((int)this + 0xc),4,(float *)this,'\0');
          FUN_0053f0b0((void *)((int)this + 0xc),4,&local_108,'\0');
          FUN_00412da0((void *)((int)this + 0xc),local_108);
          local_1e0 = (int *)FUN_005403d0((int)this);
          if (-1 < *(int *)((int)this + 8)) goto LAB_0053df44;
        }
        goto LAB_0053e0f6;
      case 0xb:
        *(undefined1 *)(local_1e0 + 3) = 0;
        iVar8 = FUN_0053f3b0(this,(int)this,0,0);
        if (iVar8 != 0) {
LAB_0053e0f6:
          *(undefined4 *)((int)this + 8) = 0xffffffff;
          *(undefined4 *)((int)this + 4) = 0xffffffff;
          goto LAB_0053e113;
        }
        local_1e0 = (int *)FUN_005403d0((int)this);
        goto LAB_0053b625;
      case 0xc:
        goto switchD_0053b687_caseD_c;
      case 0xd:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_14c,'i');
        if (local_14c == 0.0) goto switchD_0053b687_caseD_c;
        goto LAB_0053df44;
      case 0xe:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_144,'i');
        if (local_144 != 0.0) goto switchD_0053b687_caseD_c;
        goto LAB_0053df44;
      case 0xf:
        FUN_0053e390(*(void **)((int)this + 0x28),0xffffffff,0);
        break;
      case 0x10:
        pvVar4 = *(void **)((int)this + 0x28);
        fVar15 = FUN_0053efb0((int)this,1,
                              *(float *)((int)local_1e0 + (local_1e0[4] + 4U & 0xfffffffc) + 0x10));
        FUN_0053e390(pvVar4,fVar15,1);
        break;
      case 0x11:
        pvVar4 = *(void **)((int)this + 0x28);
        fVar15 = FUN_0053ed50((int)this,0.0);
        puVar7 = FUN_0053e920(pvVar4,(int)fVar15);
        if (puVar7 != (undefined4 *)0x0) {
          iVar8 = FUN_0040c300(puVar7);
          *(undefined4 *)(iVar8 + 8) = 0xffffffff;
        }
        goto LAB_0053df44;
      case 0x12:
        pvVar4 = *(void **)((int)this + 0x28);
        fVar15 = FUN_0053ed50((int)this,0.0);
        puVar7 = FUN_0053e920(pvVar4,(int)fVar15);
        if (puVar7 != (undefined4 *)0x0) {
          iVar8 = FUN_0040c300(puVar7);
          *(uint *)(iVar8 + 0x44) = *(uint *)(iVar8 + 0x44) | 1;
        }
        goto LAB_0053df44;
      case 0x13:
        pvVar4 = *(void **)((int)this + 0x28);
        fVar15 = FUN_0053ed50((int)this,0.0);
        puVar7 = FUN_0053e920(pvVar4,(int)fVar15);
        if (puVar7 != (undefined4 *)0x0) {
          iVar8 = FUN_0040c300(puVar7);
          *(uint *)(iVar8 + 0x44) = *(uint *)(iVar8 + 0x44) & 0xfffffffe;
        }
        goto LAB_0053df44;
      case 0x14:
        pvVar4 = *(void **)((int)this + 0x28);
        fVar15 = FUN_0053ed50((int)this,0.0);
        puVar7 = FUN_0053e920(pvVar4,(int)fVar15);
        if (puVar7 != (undefined4 *)0x0) {
          fVar15 = FUN_0053ed50((int)this,1.4013e-45);
          iVar8 = FUN_0040c300(puVar7);
          *(float *)(iVar8 + 0x2c) = fVar15;
        }
        goto LAB_0053df44;
      case 0x15:
        FUN_0053e560(*(int *)((int)this + 0x28));
        break;
      case 0x16:
        goto LAB_0053df44;
      case 0x17:
        fVar15 = FUN_0053ed50((int)this,0.0);
                    /* WARNING: Load size is inaccurate */
        *(float *)this = *this - (float)(int)fVar15;
        goto LAB_0053df44;
      case 0x18:
        fVar14 = (float10)FUN_0053e970(this,0);
                    /* WARNING: Load size is inaccurate */
        *(float *)this = *this - (float)fVar14;
        goto LAB_0053df44;
      case 0x1e:
        goto LAB_0053df44;
      case 0x1f:
        goto LAB_0053df44;
      case 0x28:
        fVar15 = FUN_0053ed50((int)this,0.0);
        FUN_005405b0((void *)((int)this + 0xc),(int)fVar15);
        goto LAB_0053df44;
      case 0x29:
        FUN_00540300((void *)((int)this + 0xc));
        goto LAB_0053df44;
      case 0x2a:
        local_164 = FUN_0053eee0((int)this,0.0);
        FUN_0053f260((void *)((int)this + 0xc),4,(uint *)&local_164,'i');
        break;
      case 0x2b:
        cVar16 = 'i';
        pfVar9 = (float *)FUN_0053e850(this,0);
        FUN_0053f0b0((void *)((int)this + 0xc),4,pfVar9,cVar16);
        break;
      case 0x2c:
        fVar14 = (float10)FUN_0053eb70(this,0);
        local_16c = (float)fVar14;
        FUN_0053f260((void *)((int)this + 0xc),4,(uint *)&local_16c,'f');
        break;
      case 0x2d:
        cVar16 = 'f';
        pfVar9 = (float *)FUN_0053e720(this,0);
        FUN_0053f0b0((void *)((int)this + 0xc),4,pfVar9,cVar16);
        break;
      case 0x2e:
        fVar15 = FUN_0053ed50((int)this,1.4013e-45);
        fVar15 = FUN_0053ed50((int)this,(float)((int)fVar15 * 2 + 2));
        pfVar9 = (float *)FUN_0053e850(this,0);
        *pfVar9 = fVar15;
        goto LAB_0053df44;
      case 0x2f:
        fVar15 = FUN_0053ed50((int)this,1.4013e-45);
        fVar14 = (float10)FUN_0053e970(this,(int)fVar15 * 2 + 2);
        pfVar9 = (float *)FUN_0053e720(this,0);
        *pfVar9 = (float)fVar14;
        goto LAB_0053df44;
      case 0x32:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_184,'i');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_34,'i');
        local_34 = (float)((int)local_34 + (int)local_184);
        FUN_0053f260((void *)((int)this + 0xc),4,(uint *)&local_34,'i');
        *(undefined1 *)(local_1e0 + 3) = 0;
        break;
      case 0x33:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_84,'f');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_20,'f');
        local_20 = local_20 + local_84;
        FUN_0053f260((void *)((int)this + 0xc),4,(uint *)&local_20,'f');
        *(undefined1 *)(local_1e0 + 3) = 0;
        break;
      case 0x34:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_5c,'i');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_38,'i');
        local_38 = (float)((int)local_38 - (int)local_5c);
        FUN_0053f260((void *)((int)this + 0xc),4,(uint *)&local_38,'i');
        *(undefined1 *)(local_1e0 + 3) = 0;
        break;
      case 0x35:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_94,'f');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_48,'f');
        local_48 = local_48 - local_94;
        FUN_0053f260((void *)((int)this + 0xc),4,(uint *)&local_48,'f');
        *(undefined1 *)(local_1e0 + 3) = 0;
        break;
      case 0x36:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_60,'i');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_3c,'i');
        local_3c = (float)((int)local_3c * (int)local_60);
        FUN_0053f260((void *)((int)this + 0xc),4,(uint *)&local_3c,'i');
        *(undefined1 *)(local_1e0 + 3) = 0;
        break;
      case 0x37:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_a4,'f');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_4c,'f');
        local_4c = local_4c * local_a4;
        FUN_0053f260((void *)((int)this + 0xc),4,(uint *)&local_4c,'f');
        *(undefined1 *)(local_1e0 + 3) = 0;
        break;
      case 0x38:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_68,'i');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_40,'i');
        local_40 = (float)((int)local_40 / (int)local_68);
        FUN_0053f260((void *)((int)this + 0xc),4,(uint *)&local_40,'i');
        *(undefined1 *)(local_1e0 + 3) = 0;
        break;
      case 0x39:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_b4,'f');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_50,'f');
        local_50 = local_50 / local_b4;
        FUN_0053f260((void *)((int)this + 0xc),4,(uint *)&local_50,'f');
        *(undefined1 *)(local_1e0 + 3) = 0;
        break;
      case 0x3a:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_88,'i');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_44,'i');
        local_44 = (float)((int)local_44 % (int)local_88);
        FUN_0053f260((void *)((int)this + 0xc),4,(uint *)&local_44,'i');
        *(undefined1 *)(local_1e0 + 3) = 0;
        break;
      case 0x3b:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_c8,'i');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_c4,'i');
        local_214 = (uint)(local_c4 == local_c8);
        local_d0 = local_214;
        FUN_0053f260((void *)((int)this + 0xc),4,&local_d0,'i');
        break;
      case 0x3c:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_180,'f');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_17c,'f');
        local_230 = (uint)(local_17c == local_180);
        local_188 = local_230;
        FUN_0053f260((void *)((int)this + 0xc),4,&local_188,'i');
        break;
      case 0x3d:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_e4,'i');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_e0,'i');
        local_218 = (uint)(local_e0 != local_e4);
        local_e8 = local_218;
        FUN_0053f260((void *)((int)this + 0xc),4,&local_e8,'i');
        break;
      case 0x3e:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_18c,'f');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_190,'f');
        local_234 = (uint)(local_190 != local_18c);
        local_178 = local_234;
        FUN_0053f260((void *)((int)this + 0xc),4,&local_178,'i');
        break;
      case 0x3f:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_fc,'i');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_f8,'i');
        local_21c = (uint)((int)local_f8 < (int)local_fc);
        local_100 = local_21c;
        FUN_0053f260((void *)((int)this + 0xc),4,&local_100,'i');
        break;
      case 0x40:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_174,'f');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_170,'f');
        local_238 = (uint)(local_170 < local_174);
        local_15c = local_238;
        FUN_0053f260((void *)((int)this + 0xc),4,&local_15c,'i');
        break;
      case 0x41:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_118,'i');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_114,'i');
        local_220 = (uint)((int)local_114 <= (int)local_118);
        local_11c = local_220;
        FUN_0053f260((void *)((int)this + 0xc),4,&local_11c,'i');
        break;
      case 0x42:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_158,'f');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_140,'f');
        local_23c = (uint)(local_140 <= local_158);
        local_13c = local_23c;
        FUN_0053f260((void *)((int)this + 0xc),4,&local_13c,'i');
        break;
      case 0x43:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_130,'i');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_12c,'i');
        local_224 = (uint)((int)local_130 < (int)local_12c);
        local_134 = local_224;
        FUN_0053f260((void *)((int)this + 0xc),4,&local_134,'i');
        break;
      case 0x44:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_128,'f');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_138,'f');
        local_240 = (uint)(local_128 < local_138);
        local_124 = local_240;
        FUN_0053f260((void *)((int)this + 0xc),4,&local_124,'i');
        break;
      case 0x45:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_150,'i');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_148,'i');
        local_228 = (uint)((int)local_150 <= (int)local_148);
        local_154 = local_228;
        FUN_0053f260((void *)((int)this + 0xc),4,&local_154,'i');
        break;
      case 0x46:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_110,'f');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_120,'f');
        local_244 = (uint)(local_110 <= local_120);
        local_10c = local_244;
        FUN_0053f260((void *)((int)this + 0xc),4,&local_10c,'i');
        break;
      case 0x47:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_160,'i');
        local_22c = (uint)(local_160 == 0.0);
        local_168 = local_22c;
        FUN_0053f260((void *)((int)this + 0xc),4,&local_168,'i');
        break;
      case 0x48:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_104,'f');
        local_248 = (uint)(local_104 == _DAT_0056e0e8);
        local_f4 = local_248;
        FUN_0053f260((void *)((int)this + 0xc),4,&local_f4,'i');
        break;
      case 0x49:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_ec,'i');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_f0,'i');
        if ((local_f0 == 0.0) && (local_ec == 0.0)) {
          local_274 = 0;
        }
        else {
          local_274 = 1;
        }
        local_dc = local_274;
        FUN_0053f260((void *)((int)this + 0xc),4,&local_dc,'i');
        break;
      case 0x4a:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_d4,'i');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_d8,'i');
        if ((local_d8 == 0.0) || (local_d4 == 0.0)) {
          local_24c = 0;
        }
        else {
          local_24c = 1;
        }
        local_cc = local_24c;
        FUN_0053f260((void *)((int)this + 0xc),4,&local_cc,'i');
        break;
      case 0x4b:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_bc,'i');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_c0,'i');
        local_b8 = (uint)local_c0 ^ (uint)local_bc;
        FUN_0053f260((void *)((int)this + 0xc),4,&local_b8,'i');
        break;
      case 0x4c:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_ac,'i');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_b0,'i');
        local_a8 = (uint)local_b0 | (uint)local_ac;
        FUN_0053f260((void *)((int)this + 0xc),4,&local_a8,'i');
        break;
      case 0x4d:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_9c,'i');
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_a0,'i');
        local_98 = (uint)local_a0 & (uint)local_9c;
        FUN_0053f260((void *)((int)this + 0xc),4,&local_98,'i');
        break;
      case 0x4e:
        local_54 = FUN_0053ed50((int)this,0.0);
        iVar8 = (int)local_54 + -1;
        piVar2 = (int *)FUN_0053e850(this,0);
        *piVar2 = iVar8;
        FUN_0053f260((void *)((int)this + 0xc),4,(uint *)&local_54,'i');
        break;
      case 0x4f:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_90,'f');
        fVar14 = FUN_00439820(extraout_ECX,local_90);
        local_8c = (float)fVar14;
        FUN_0053f260((void *)((int)this + 0xc),4,(uint *)&local_8c,'f');
        break;
      case 0x50:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_7c,'f');
        fVar14 = FUN_004397c0(extraout_ECX_01,local_7c);
        local_78 = (float)fVar14;
        FUN_0053f260((void *)((int)this + 0xc),4,(uint *)&local_78,'f');
        break;
      case 0x51:
        FUN_00422e10(&local_1c0);
        fVar14 = (float10)FUN_0053e970(this,2);
        fVar14 = FUN_00438540((float)fVar14);
        fVar12 = (float10)FUN_0053e970(this,3);
        FUN_00439330(this_00,&local_1c0,(float)fVar14,(float)fVar12);
        pfVar9 = (float *)FUN_0053e720(this,0);
        *pfVar9 = local_1c0;
        puVar7 = (undefined4 *)FUN_0053e720(this,1);
        *puVar7 = local_1bc;
        goto LAB_0053df44;
      case 0x52:
        fVar14 = (float10)FUN_0053e970(this,0);
        fVar14 = FUN_00438540((float)fVar14);
        pfVar9 = (float *)FUN_0053e720(this,0);
        *pfVar9 = (float)fVar14;
        goto LAB_0053df44;
      case 0x53:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_24,'i');
        local_24 = (float)-(int)local_24;
        FUN_0053f260((void *)((int)this + 0xc),4,(uint *)&local_24,'i');
        break;
      case 0x54:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_28,'f');
        local_28 = (float)((uint)local_28 ^ _DAT_0056d7d0);
        FUN_0053f260((void *)((int)this + 0xc),4,(uint *)&local_28,'f');
        break;
      case 0x55:
        fVar14 = (float10)FUN_0053e970(this,1);
        fVar12 = (float10)FUN_0053e970(this,2);
        pfVar9 = (float *)FUN_0053e720(this,0);
        *pfVar9 = (float)fVar14 * (float)fVar14 + (float)fVar12 * (float)fVar12;
        goto LAB_0053df44;
      case 0x56:
        fVar14 = (float10)FUN_0053e970(this,1);
        fVar12 = (float10)FUN_0053e970(this,2);
        fVar14 = FUN_00446b30(extraout_ECX_02,
                              (float)fVar14 * (float)fVar14 + (float)fVar12 * (float)fVar12);
        pfVar9 = (float *)FUN_0053e720(this,0);
        *pfVar9 = (float)fVar14;
        goto LAB_0053df44;
      case 0x57:
        fVar14 = (float10)FUN_0053e970(this,1);
        fVar12 = (float10)FUN_0053e970(this,2);
        fVar13 = (float10)FUN_0053e970(this,3);
        fVar11 = (float10)FUN_0053e970(this,4);
        fVar14 = FUN_00459280((float)fVar11 - (float)fVar12,(float)fVar13 - (float)fVar14);
        pfVar9 = (float *)FUN_0053e720(this,0);
        *pfVar9 = (float)fVar14;
        goto LAB_0053df44;
      case 0x58:
        FUN_0053f0b0((void *)((int)this + 0xc),4,&local_70,'f');
        fVar14 = FUN_00446b30(extraout_ECX_00,local_70);
        local_80 = (float)fVar14;
        FUN_0053f260((void *)((int)this + 0xc),4,(uint *)&local_80,'f');
        break;
      case 0x59:
        fVar14 = (float10)FUN_0053e970(this,1);
        fVar12 = (float10)FUN_0053e970(this,2);
        fVar14 = FUN_004396c0((float)fVar12,(float)fVar14);
        pfVar9 = (float *)FUN_0053e720(this,0);
        *pfVar9 = (float)fVar14;
        goto LAB_0053df44;
      case 0x5a:
        FUN_00422e10(&local_1b4);
        FUN_00422e10(&local_1cc);
        fVar14 = (float10)FUN_0053e970(this,2);
        local_1b4 = (float)fVar14;
        fVar14 = (float10)FUN_0053e970(this,3);
        local_1b0 = (float)fVar14;
        fVar14 = (float10)FUN_0053e970(this,4);
        fVar14 = FUN_00438540((float)fVar14);
        FUN_00458fa0(&local_1cc,(float *)&local_1cc,&local_1b4,(float)fVar14);
        puVar7 = (undefined4 *)FUN_0053e720(this,0);
        *puVar7 = local_1cc;
        puVar7 = (undefined4 *)FUN_0053e720(this,1);
        *puVar7 = local_1c8;
        goto LAB_0053df44;
      case 0x5b:
        fVar15 = FUN_0053ed50((int)this,0.0);
        uVar3 = FUN_0053f780((int)this + 0x34);
        if (uVar3 < (int)fVar15 + 1U) {
          FUN_0053f390((void *)((int)this + 0x34),(int)fVar15 + 1);
        }
        fVar5 = FUN_0053ed50((int)this,2.8026e-45);
        if ((int)fVar5 < 1) {
          iVar8 = FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
          FUN_00429970(iVar8);
          iVar8 = FUN_0053f780((int)this + 0x34);
          if (iVar8 == (int)fVar15 + 1) {
            FUN_0053f1b0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)((int)this + 0x34));
          }
        }
        else {
          uVar6 = *(undefined4 *)((int)this + 4);
          uVar1 = *(undefined4 *)((int)this + 8);
          iVar8 = FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
          *(undefined4 *)(iVar8 + 0x30) = uVar6;
          *(undefined4 *)(iVar8 + 0x34) = uVar1;
          pfVar9 = (float *)FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
          FUN_0042a110(pfVar9);
          pvVar4 = (void *)FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
          fVar5 = FUN_0053ed50((int)this,2.8026e-45);
          FUN_0041df50(pvVar4,fVar5);
          pvVar4 = (void *)FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
          fVar5 = FUN_0053ed50((int)this,4.2039e-45);
          FUN_004394d0(pvVar4,fVar5);
          fVar14 = (float10)FUN_0053e970(this,4);
          local_18 = (float)fVar14;
          fVar14 = (float10)FUN_0053e970(this,5);
          local_2c = (float)fVar14;
          pvVar4 = (void *)FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
          FUN_004393d0(pvVar4,&local_18);
          pvVar4 = (void *)FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
          FUN_00438a70(pvVar4,&local_2c);
          fVar5 = local_18;
          pfVar9 = (float *)FUN_0053e720(this,1);
          *pfVar9 = fVar5;
          local_18 = 0.0;
          local_2c = 0.0;
          pvVar4 = (void *)FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
          FUN_00439460(pvVar4,&local_18);
          pvVar4 = (void *)FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
          FUN_00438b00(pvVar4,&local_2c);
          iVar8 = FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
          FUN_004395b0(iVar8);
          uVar6 = FUN_0041ca90((int)this + 0xc);
          iVar8 = FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
          *(undefined4 *)(iVar8 + 0x2c) = uVar6;
        }
        goto LAB_0053df44;
      case 0x5c:
        fVar15 = FUN_0053ed50((int)this,0.0);
        uVar3 = FUN_0053f780((int)this + 0x34);
        if (uVar3 < (int)fVar15 + 1U) {
          FUN_0053f390((void *)((int)this + 0x34),(int)fVar15 + 1);
        }
        uVar6 = *(undefined4 *)((int)this + 4);
        uVar1 = *(undefined4 *)((int)this + 8);
        iVar8 = FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
        *(undefined4 *)(iVar8 + 0x30) = uVar6;
        *(undefined4 *)(iVar8 + 0x34) = uVar1;
        pfVar9 = (float *)FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
        FUN_0042a110(pfVar9);
        pvVar4 = (void *)FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
        fVar5 = FUN_0053ed50((int)this,2.8026e-45);
        FUN_0041df50(pvVar4,fVar5);
        pvVar4 = (void *)FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
        fVar5 = FUN_0053ed50((int)this,4.2039e-45);
        FUN_004394d0(pvVar4,fVar5);
        fVar14 = (float10)FUN_0053e970(this,4);
        local_1c = (float)fVar14;
        fVar14 = (float10)FUN_0053e970(this,5);
        local_30 = (float)fVar14;
        pvVar4 = (void *)FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
        FUN_004393d0(pvVar4,&local_1c);
        pvVar4 = (void *)FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
        FUN_00438a70(pvVar4,&local_30);
        fVar5 = local_1c;
        pfVar9 = (float *)FUN_0053e720(this,1);
        *pfVar9 = fVar5;
        fVar14 = (float10)FUN_0053e970(this,6);
        local_1c = (float)fVar14;
        fVar14 = (float10)FUN_0053e970(this,7);
        local_30 = (float)fVar14;
        pvVar4 = (void *)FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
        FUN_00439460(pvVar4,&local_1c);
        pvVar4 = (void *)FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
        FUN_00438b00(pvVar4,&local_30);
        iVar8 = FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
        FUN_004395b0(iVar8);
        uVar6 = FUN_0041ca90((int)this + 0xc);
        iVar8 = FUN_0053b560((void *)((int)this + 0x34),(int)fVar15);
        *(undefined4 *)(iVar8 + 0x2c) = uVar6;
        goto LAB_0053df44;
      case 0x5d:
        FUN_00422e10(&local_1d8);
        fVar14 = (float10)FUN_0053e970(this,2);
        fVar12 = (float10)FUN_0053e970(this,3);
        fVar13 = FUN_004298e0(0x5ba4a8);
        fVar11 = FUN_004297d0(0x5ba4a8);
        FUN_00439330(this_01,&local_1d8,(float)fVar11,
                     ((float)fVar12 - (float)fVar14) * (float)fVar13 + (float)fVar14);
        pfVar9 = (float *)FUN_0053e720(this,0);
        *pfVar9 = local_1d8;
        puVar7 = (undefined4 *)FUN_0053e720(this,1);
        *puVar7 = local_1d4;
        goto LAB_0053df44;
      case 0x5e:
        FUN_00422e10(local_1a8);
        FUN_00422e10(&local_19c);
        fVar14 = (float10)FUN_0053e970(this,2);
        fVar12 = (float10)FUN_0053e970(this,4);
        fVar13 = (float10)FUN_0053e970(this,3);
        fVar14 = FUN_00438540((float)fVar14 - (float)fVar12);
        FUN_00439330(this_02,local_1a8,(float)fVar14,(float)fVar13);
        fVar14 = (float10)FUN_0053e970(this,5);
        local_1a8[0] = (float)fVar14 * local_1a8[0];
        FUN_00458fa0(local_1a8,&local_19c,local_1a8,(float)fVar12);
        local_194 = 0;
        pfVar9 = (float *)FUN_0053e720(this,0);
        *pfVar9 = local_19c;
        puVar7 = (undefined4 *)FUN_0053e720(this,1);
        *puVar7 = local_198;
        goto LAB_0053df44;
      case 0x5f:
        fVar14 = (float10)FUN_0053e970(this,1);
        FUN_00429210(&local_6c,(float)fVar14);
        fVar14 = (float10)FUN_0053e970(this,2);
        FUN_00429210(&local_74,(float)fVar14);
        fVar14 = FUN_004292e0(&local_74);
        fVar15 = (float)fVar14 * DAT_0056c8d0;
        fVar14 = FUN_004292e0(&local_6c);
        pfVar9 = (float *)FUN_0053e720(this,0);
        *pfVar9 = fVar15 - (float)fVar14;
        goto LAB_0053df44;
      case 0x60:
        fVar14 = (float10)FUN_0053e970(this,1);
        FUN_00429210(&local_58,(float)fVar14);
        fVar14 = FUN_004292e0(&local_58);
        if (((float)fVar14 <= DAT_0056e0f8 / DAT_0056c8d0) ||
           (fVar14 = FUN_004292e0(&local_58), (float)fVar14 <= DAT_0056e0f0 / DAT_0056c8d0)) {
          local_268 = DAT_0056e0f4;
        }
        else {
          local_268 = DAT_0056c8cc;
        }
        puVar7 = (undefined4 *)FUN_0053e720(this,0);
        *puVar7 = local_268;
        goto LAB_0053df44;
      case 0x61:
        fVar14 = (float10)FUN_0053e970(this,1);
        FUN_00429210(&local_64,(float)fVar14);
        fVar14 = FUN_004292e0(&local_64);
        if (0.0 <= (float)fVar14) {
          local_26c = DAT_0056c8cc;
        }
        else {
          local_26c = DAT_0056e0f4;
        }
        puVar7 = (undefined4 *)FUN_0053e720(this,0);
        *puVar7 = local_26c;
LAB_0053df44:
        if ((char)local_1e0[3] != '\0') {
          iVar8 = FUN_00411700((int)this + 0xc);
          FUN_00412da0((void *)((int)this + 0xc),iVar8 - (uint)*(byte *)(local_1e0 + 3));
        }
      }
LAB_0053dfbc:
      *(uint *)((int)this + 8) = (uint)*(ushort *)((int)local_1e0 + 6) + *(int *)((int)this + 8);
      local_1e0 = (int *)((uint)*(ushort *)((int)local_1e0 + 6) + (int)local_1e0);
      goto LAB_0053b625;
    }
                    /* WARNING: Load size is inaccurate */
    *(float *)this = *this + param_1;
LAB_0053e00c:
    local_1f0 = (float *)FUN_00414f30((int)this + 0x34);
    pfVar9 = (float *)FUN_00414f70((int)this + 0x34);
    for (; local_1f0 != pfVar9; local_1f0 = local_1f0 + 0xe) {
      iVar8 = FUN_0041caf0((int)local_1f0);
      if (iVar8 != 0) {
        iVar8 = FUN_0053e8e0(*(void **)(*(int *)((int)this + 0x28) + 0x58),(int)local_1f0[0xc],
                             (int)local_1f0[0xd]);
        fVar14 = FUN_0042a110(local_1f0);
        pfVar10 = (float *)FUN_0053e7c0(this,iVar8,(int)local_1f0[0xb],1);
        *pfVar10 = (float)fVar14;
      }
    }
  }
LAB_0053e113:
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffff0);
  return;
switchD_0053b687_caseD_c:
  *(float *)this = (float)local_1e0[5];
  *(int *)((int)this + 8) = *(int *)((int)this + 8) + local_1e0[4];
  local_1e0 = (int *)((int)local_1e0 + local_1e0[4]);
  goto LAB_0053b625;
}

