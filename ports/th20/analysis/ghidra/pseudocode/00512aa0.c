/* Entry: 0x00512aa0; symbol: FUN_00512aa0; body bytes: 1164 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00512aa0(int param_1)

{
  char cVar1;
  bool bVar2;
  int iVar3;
  undefined4 *puVar4;
  float *pfVar5;
  uint uVar6;
  float10 fVar7;
  float fVar8;
  undefined4 uVar9;
  uint local_68 [5];
  uint local_54;
  uint local_50;
  uint local_4c;
  void *local_48;
  int local_44;
  void *local_40;
  void *local_3c;
  float local_38;
  float local_34;
  int local_30;
  float local_2c;
  int local_28;
  byte local_21;
  float local_20;
  float local_1c;
  float local_18;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_28 = param_1;
  cVar1 = FUN_00485a40(param_1);
  if ((cVar1 != '\0') && (iVar3 = FUN_004859f0(local_28), iVar3 != 0)) {
    local_3c = (void *)(local_28 + 0x18);
    bVar2 = FUN_00423560(local_3c,200);
    if (bVar2) {
      puVar4 = (undefined4 *)FUN_004859f0(local_28);
      local_30 = FUN_0040c300(puVar4);
      local_68[0] = 0xffff4040;
      local_68[1] = 0xff4040ff;
      local_68[2] = 0xffffff00;
      local_68[3] = 0xff40ff40;
      FUN_004b8620(DAT_005c0698,0,0);
      FUN_00488b00(DAT_005c0698,0xc);
      local_40 = (void *)(local_28 + 0x18);
      iVar3 = FUN_00472040(local_40,8);
      if (iVar3 < 6) {
        FUN_00470ad0(DAT_005c0698,local_68[*(int *)(local_30 + 0x2c)]);
      }
      else {
        FUN_00470ad0(DAT_005c0698,0xffffffff);
      }
      FUN_00422dd0(&local_14,DAT_0056ed14,DAT_0056fb80,0);
      local_48 = (void *)FUN_00478060(0);
      puVar4 = (undefined4 *)FUN_004a0da0(local_30);
      local_44 = FUN_0040c300(puVar4);
      uVar9 = 0x512bd9;
      iVar3 = FUN_00498a80(local_48,local_44);
      pfVar5 = (float *)FUN_0047a2c0(iVar3);
      local_20 = *pfVar5;
      local_1c = pfVar5[1];
      local_18 = pfVar5[2];
      if (DAT_005742d8 <= local_20) {
        local_34 = local_20;
        if (DAT_0056f090 < local_20) {
          local_34 = DAT_0056f090;
        }
        local_38 = local_34;
      }
      else {
        local_38 = DAT_005742d8;
      }
      local_14 = local_38 + DAT_0056ed14;
      FUN_0040ddd0(&stack0xffffff90,"Wonder Stone Appeared!");
      pfVar5 = &local_14;
      FUN_0046ce90(DAT_005c0698,pfVar5);
      local_10 = local_10 + DAT_0056cd90;
      uVar6 = *(int *)(local_30 + 0x30) + 1;
      local_4c = uVar6;
      FUN_0040ddd0(&stack0xffffff8c,"Level ");
      FUN_0046cfa0(DAT_005c0698,&local_14,pfVar5,uVar9,uVar6);
      local_10 = local_10 - DAT_0056cd90;
      fVar7 = FUN_00511cf0((float *)(local_28 + 0x38));
      local_2c = (float)fVar7;
      FUN_004e6920(DAT_005c0698,1);
      FUN_004b8d90(DAT_005c0698,local_2c + DAT_0056c8cc,local_2c + DAT_0056c8cc);
      local_21 = FUN_00511af0((byte *)(local_28 + 100));
      uVar9 = 0x512d37;
      FUN_00488940(DAT_005c0698,local_21);
      FUN_0040ddd0(&stack0xffffff90,"Wonder Stone Appeared!");
      pfVar5 = &local_14;
      FUN_0046ce90(DAT_005c0698,pfVar5);
      local_10 = local_10 + DAT_0056cd90;
      uVar6 = *(int *)(local_30 + 0x30) + 1;
      local_50 = uVar6;
      FUN_0040ddd0(&stack0xffffff8c,"Level ");
      FUN_0046cfa0(DAT_005c0698,&local_14,pfVar5,uVar9,uVar6);
      local_10 = local_10 - DAT_0056cd90;
      fVar8 = local_2c * _DAT_005742d0 + DAT_0056c8cc;
      FUN_004b8d90(DAT_005c0698,fVar8,local_2c * _DAT_005742d0 + DAT_0056c8cc);
      FUN_0040ddd0(&stack0xffffff90,"Wonder Stone Appeared!");
      pfVar5 = &local_14;
      FUN_0046ce90(DAT_005c0698,pfVar5);
      local_10 = local_10 + DAT_0056cd90;
      uVar6 = *(int *)(local_30 + 0x30) + 1;
      local_54 = uVar6;
      FUN_0040ddd0(&stack0xffffff8c,"Level ");
      FUN_0046cfa0(DAT_005c0698,&local_14,pfVar5,fVar8,uVar6);
      local_10 = local_10 - DAT_0056cd90;
      fVar8 = local_2c * _DAT_0057055c + DAT_0056c8cc;
      FUN_004b8d90(DAT_005c0698,fVar8,local_2c * _DAT_0057055c + DAT_0056c8cc);
      FUN_0040ddd0(&stack0xffffff90,"Wonder Stone Appeared!");
      pfVar5 = &local_14;
      FUN_0046ce90(DAT_005c0698,pfVar5);
      local_10 = local_10 + DAT_0056cd90;
      uVar6 = *(int *)(local_30 + 0x30) + 1;
      local_68[4] = uVar6;
      FUN_0040ddd0(&stack0xffffff8c,"Level ");
      FUN_0046cfa0(DAT_005c0698,&local_14,pfVar5,fVar8,uVar6);
      local_10 = local_10 - DAT_0056cd90;
      FUN_004e67e0(DAT_005c0698);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

