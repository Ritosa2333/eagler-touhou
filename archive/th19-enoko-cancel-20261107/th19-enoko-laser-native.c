// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 52a970

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

int __thiscall
FUN_0052a970(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
            undefined4 param_5,int param_6)

{
  int *piVar1;
  float10 fVar2;
  char cVar3;
  undefined1 *puVar4;
  undefined4 extraout_ECX;
  int iVar5;
  int iVar6;
  float10 fVar7;
  int iVar8;
  float local_254;
  float local_250;
  float local_24c;
  float local_248;
  float local_244;
  float local_240;
  float local_23c;
  undefined8 local_238;
  float local_230;
  float local_22c;
  undefined1 local_228 [4];
  undefined1 local_224 [4];
  undefined1 local_220 [16];
  undefined1 local_210 [516];
  uint local_c;
  
  local_c = DAT_005c3d80 ^ (uint)&local_254;
  if ((param_6 != 0) && (param_1[0x217] != 0)) {
    return 0;
  }
  local_22c = 0.0;
  iVar6 = 0;
  iVar5 = 0;
  local_24c = DAT_005aa30c / DAT_005aa280;
  local_244 = 0.0;
  local_240 = 0.0;
  local_23c = 0.0;
  _memset(local_210,0,0x200);
  fVar2 = (float10)fcos((float10)(float)param_1[0x1e]);
  fVar7 = (float10)fsin((float10)(float)param_1[0x1e]);
  local_244 = (float)(fVar2 * (float10)(DAT_005aa30c / DAT_005aa280));
  local_240 = (float)(fVar7 * (float10)(DAT_005aa30c / DAT_005aa280));
  local_254 = local_244 + (float)param_1[0x18];
  local_230 = 0.0;
  local_250 = local_240 + (float)param_1[0x19];
  local_244 = local_244 + local_244;
  local_240 = local_240 + local_240;
  local_23c = 0.0;
  local_238 = CONCAT44(local_250,local_254);
  if (DAT_005aa30c / DAT_005aa280 + local_24c <= (float)param_1[0x1f]) {
    local_248 = DAT_005aa30c / DAT_005aa280;
    do {
      cVar3 = FUN_004d2c30(param_4,local_254,local_250,local_244,local_240,param_1[0x1e]);
      if (cVar3 != '\0') {
        piVar1 = (int *)param_1[0x220];
        iVar5 = iVar5 + 1;
        local_210[iVar6] = 1;
        *(int *)(*piVar1 + 0x815708) = *(int *)(*piVar1 + 0x815708) + 1;
        iVar8 = param_1[0x21d];
        if (((iVar8 < 0x12) || (iVar8 == 0x22)) || (iVar8 == 0x26)) {
          iVar8 = param_1[0x238] * 2 + 0xd4;
          puVar4 = local_220;
        }
        else if (iVar8 < 0x20) {
          iVar8 = param_1[0x238] * 2 + 0x104;
          puVar4 = local_224;
        }
        else {
          if (0x21 < iVar8) goto LAB_0052abe7;
          iVar8 = param_1[0x238] * 2 + 0x11c;
          puVar4 = local_228;
        }
        FUN_004cb960(puVar4,extraout_ECX,iVar8,&local_238,0,0xffffffff,0,0);
      }
LAB_0052abe7:
      iVar6 = iVar6 + 1;
      local_230 = local_22c + local_23c;
      local_254 = local_254 + local_244;
      local_250 = local_250 + local_240;
      local_24c = local_24c + DAT_005aa30c;
      local_238 = CONCAT44(local_250,local_254);
      local_22c = local_230;
    } while (local_248 + local_24c <= (float)param_1[0x1f]);
    if (iVar5 != 0) {
      (**(code **)(*param_1 + 0x38))(local_210,iVar6);
    }
  }
  return iVar5;
}


// Reference 005a8768
// Address 524fc0

int __thiscall
FUN_00524fc0(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
            undefined4 param_5,int param_6)

{
  char cVar1;
  undefined4 *puVar2;
  undefined1 local_7c [12];
  undefined1 local_70 [12];
  undefined1 local_64 [12];
  int local_58;
  int local_54;
  int local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined1 local_28 [4];
  code *local_24;
  undefined4 local_20;
  int local_1c;
  undefined4 *local_18;
  int local_14;
  void *local_10;
  int local_c;
  int *local_8;
  
  if ((param_6 == 0) || (param_1[0x217] == 0)) {
    local_14 = 0;
    local_c = 0;
    local_8 = param_1;
    FUN_00402780();
    FUN_00402780();
    FUN_00402780();
    local_58 = local_8[0x18];
    local_54 = local_8[0x19];
    local_50 = local_8[0x1a];
    local_10 = (void *)FUN_004d7440(local_8[0x22a],
                                    "D:\\cygwin\\home\\zun\\prog\\th19\\src\\game\\laser.cpp:3814 u_char"
                                   );
    if (local_10 == (void *)0x0) {
      local_14 = 0;
    }
    else {
      _memset(local_10,0,local_8[0x22a]);
      puVar2 = (undefined4 *)FUN_00407350(local_64);
      local_34 = *puVar2;
      local_30 = puVar2[1];
      local_2c = puVar2[2];
      puVar2 = (undefined4 *)FUN_00403b00(local_70,&local_34);
      local_4c = *puVar2;
      local_48 = puVar2[1];
      local_44 = puVar2[2];
      puVar2 = (undefined4 *)FUN_00404010(local_7c,&local_34);
      local_34 = *puVar2;
      local_30 = puVar2[1];
      local_2c = puVar2[2];
      local_18 = (undefined4 *)local_8[0x65a];
      for (local_c = 0; local_c < local_8[0x22a]; local_c = local_c + 1) {
        local_40 = *local_18;
        local_3c = local_18[1];
        local_38 = local_18[2];
        cVar1 = FUN_004d4910(param_4,local_40,local_3c);
        if (cVar1 != '\0') {
          *(undefined1 *)((int)local_10 + local_c) = 1;
          local_14 = local_14 + 1;
          *(int *)(*(int *)local_8[0x220] + 0x815708) =
               *(int *)(*(int *)local_8[0x220] + 0x815708) + 1;
          if (local_c % 10 == 0) {
            local_20 = FUN_0040e210();
            local_1c = local_8[0x229] * 2 + 0xd4;
            FUN_0040a490(local_28,"bullet",local_1c,&local_40,0,0xffffffff,0);
          }
        }
        local_18 = local_18 + 8;
      }
      if (local_14 < local_8[0x22a]) {
        if (0 < local_14) {
          local_24 = *(code **)(*local_8 + 0x38);
          (*local_24)(local_10,local_c);
        }
      }
      else {
        local_8[6] = local_8[6] & 0xfffffff9U | 2;
      }
      if (local_10 != (void *)0x0) {
        FUN_004d74a0(local_10);
      }
    }
  }
  else {
    local_14 = 0;
  }
  return local_14;
}


// Reference 005a87f8
// Address 52d350

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

int __thiscall
FUN_0052d350(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
            undefined4 param_5,int param_6)

{
  int *piVar1;
  float10 fVar2;
  char cVar3;
  undefined1 *puVar4;
  undefined4 extraout_ECX;
  int iVar5;
  int iVar6;
  float10 fVar7;
  int iVar8;
  float local_254;
  float local_250;
  float local_24c;
  float local_248;
  float local_244;
  float local_240;
  float local_23c;
  undefined8 local_238;
  float local_230;
  float local_22c;
  undefined1 local_228 [4];
  undefined1 local_224 [4];
  undefined1 local_220 [16];
  undefined1 local_210 [516];
  uint local_c;
  
  local_c = DAT_005c3d80 ^ (uint)&local_254;
  if ((param_6 != 0) && (param_1[0x217] != 0)) {
    return 0;
  }
  local_22c = 0.0;
  iVar5 = 0;
  iVar6 = 0;
  local_24c = DAT_005aa30c / DAT_005aa280;
  local_244 = 0.0;
  local_240 = 0.0;
  local_23c = 0.0;
  _memset(local_210,0,0x200);
  fVar2 = (float10)fcos((float10)(float)param_1[0x1e]);
  fVar7 = (float10)fsin((float10)(float)param_1[0x1e]);
  local_244 = (float)(fVar2 * (float10)(DAT_005aa30c / DAT_005aa280));
  local_240 = (float)(fVar7 * (float10)(DAT_005aa30c / DAT_005aa280));
  local_254 = local_244 + (float)param_1[0x18];
  local_230 = 0.0;
  local_250 = local_240 + (float)param_1[0x19];
  local_244 = local_244 + local_244;
  local_240 = local_240 + local_240;
  local_23c = 0.0;
  local_238 = CONCAT44(local_250,local_254);
  if (DAT_005aa30c / DAT_005aa280 + local_24c <= (float)param_1[0x1f]) {
    local_248 = DAT_005aa30c / DAT_005aa280;
    do {
      cVar3 = FUN_004d2c30(param_4,local_254,local_250,local_244,local_240,param_1[0x1e]);
      if (cVar3 != '\0') {
        piVar1 = (int *)param_1[0x220];
        iVar6 = iVar6 + 1;
        local_210[iVar5] = 1;
        *(int *)(*piVar1 + 0x815708) = *(int *)(*piVar1 + 0x815708) + 1;
        iVar8 = param_1[0x21d];
        if (((iVar8 < 0x12) || (iVar8 == 0x22)) || (iVar8 == 0x26)) {
          iVar8 = param_1[0x22c] * 2 + 0xd4;
          puVar4 = local_220;
        }
        else if (iVar8 < 0x1f) {
          iVar8 = param_1[0x22c] * 2 + 0x104;
          puVar4 = local_228;
        }
        else {
          if (0x21 < iVar8) goto LAB_0052d5c3;
          iVar8 = param_1[0x22c] * 2 + 0x11c;
          puVar4 = local_224;
        }
        FUN_004cb960(puVar4,extraout_ECX,iVar8,&local_238,0,0xffffffff,0,0);
      }
LAB_0052d5c3:
      iVar5 = iVar5 + 1;
      local_230 = local_22c + local_23c;
      local_254 = local_254 + local_244;
      local_250 = local_250 + local_240;
      local_24c = local_24c + DAT_005aa30c;
      local_238 = CONCAT44(local_250,local_254);
      local_22c = local_230;
    } while (local_248 + local_24c <= (float)param_1[0x1f]);
    if (iVar6 != 0) {
      if (iVar6 < iVar5) {
        (**(code **)(*param_1 + 0x38))(local_210,iVar5);
      }
      else {
        param_1[6] = param_1[6] & 0xfffffffbU | 2;
      }
    }
  }
  return iVar6;
}


// Reference 005a8888
