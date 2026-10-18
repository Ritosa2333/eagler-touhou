/* Entry: 0x0052df30; symbol: FUN_0052df30; body bytes: 564 */

undefined4 __fastcall FUN_0052df30(undefined4 *param_1)

{
  bool bVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined1 auStack_8c [24];
  undefined4 uStack_74;
  undefined8 local_60;
  undefined8 local_58;
  undefined1 *local_50;
  undefined1 *local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 *local_3c;
  undefined4 *local_38;
  int local_34;
  undefined *local_30;
  int local_2c;
  undefined *local_28;
  int local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 *local_14;
  int local_10;
  undefined4 *local_c;
  undefined1 local_6;
  undefined1 local_5;
  
  local_10 = param_1[0xc];
  local_c = param_1;
  if (local_10 == 0) {
    iVar3 = FUN_0052f4c0((int)(param_1 + 5));
    if (iVar3 != 0) {
      local_14 = local_c + 5;
      local_24 = FUN_0052f740(local_14,0);
      FUN_0052f800(local_c,1);
      local_18 = local_c[4];
      uStack_74 = 0x52dfb3;
      puVar2 = FUN_0049e080(&local_40,(uint *)"trophy",0,(undefined4 *)0x0);
      local_c[0x12] = *puVar2;
      local_1c = local_c[4];
      uStack_74 = 0x52dfd9;
      puVar2 = FUN_0049e080(&local_44,(uint *)"trophy",7,(undefined4 *)0x0);
      local_c[0x13] = *puVar2;
      local_20 = local_c[4];
      uStack_74 = 0x52dfff;
      puVar2 = FUN_0049e080(&local_48,(uint *)"trophy",8,(undefined4 *)0x0);
      local_c[0x14] = *puVar2;
      local_28 = FUN_0052f060((byte *)(DAT_005c685c + 4 + local_24 * 0x704));
      local_2c = FUN_0044ced0(local_c + 0x13);
      local_4c = &stack0xffffff70;
      FUN_0052d0d0(&stack0xffffff70,&local_5);
      FUN_0044add0(DAT_005c0028,&local_58,local_2c,0x80c0c0,0,2,0);
      local_30 = PTR_DAT_005b0aa8;
      local_34 = FUN_0044ced0(local_c + 0x14);
      local_50 = auStack_8c;
      FUN_0052d140(auStack_8c,&local_6);
      FUN_0044add0(DAT_005c0028,&local_60,local_34,0x80c0c0,0,2,0);
      FUN_00426d70(&DAT_005ba830,0x4f,0);
    }
  }
  else if (local_10 != 1) {
    if (local_10 == 2) {
      FUN_0044fcd0(param_1 + 0x12);
      FUN_0044fcd0(local_c + 0x13);
      FUN_0044fcd0(local_c + 0x14);
      FUN_004217c0(local_c);
      return 1;
    }
    goto LAB_0052e148;
  }
  local_38 = local_c + 0xe;
  bVar1 = FUN_004235c0(local_38,0xf0);
  if (bVar1) {
    iVar3 = FUN_0052f4c0((int)(local_c + 5));
    if (iVar3 == 0) {
      FUN_0052f800(local_c,2);
    }
    else {
      FUN_0052f800(local_c,0);
    }
  }
LAB_0052e148:
  local_3c = local_c + 0xe;
  FUN_00423540(local_3c);
  return 1;
}

