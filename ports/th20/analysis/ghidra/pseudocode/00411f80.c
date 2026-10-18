/* Entry: 0x00411f80; symbol: FUN_00411f80; body bytes: 381 */

void __thiscall FUN_00411f80(void *this,int *param_1,int param_2)

{
  code *pcVar1;
  bool bVar2;
  uint uVar3;
  undefined4 uVar4;
  int *piVar5;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *this_00;
  undefined4 *puVar6;
  int *unaff_FS_OFFSET;
  undefined4 local_20;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  uint local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567b95;
  local_10 = *unaff_FS_OFFSET;
  uVar3 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_14 = uVar3;
  if (param_1[3] != 0) {
    pcVar1 = (code *)param_1[3];
    uVar4 = FUN_00412710((int)param_1);
    (*pcVar1)(uVar4,uVar3);
    param_1[3] = 0;
  }
  uVar4 = FUN_0040c310(&DAT_005c0240,0);
  FUN_0040b730(&local_20,uVar4);
  local_8 = 0;
  *param_1 = param_2;
  FUN_0040c080(local_1c,8);
  FUN_00412280((void *)((int)this + 4),local_1c);
  local_8 = CONCAT31(local_8._1_3_,1);
  piVar5 = (int *)FUN_00412540();
  do {
    bVar2 = FUN_00411bb0(local_1c,piVar5);
    if (!bVar2) {
      local_8 = local_8 & 0xffffff00;
      FUN_00411b00(local_1c);
      FUN_00411ea0((void *)((int)this + 4),param_1 + 5);
      local_8 = 0xffffffff;
      FUN_0040b900(&local_20);
LAB_004120e2:
      *unaff_FS_OFFSET = local_10;
      FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
      return;
    }
    this_00 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               *)FUN_0040c300(local_1c);
    puVar6 = (undefined4 *)FUN_0040f160(this_00);
    if (param_2 <= *(int *)*puVar6) {
      FUN_00411f30(this_00,(int)(param_1 + 5));
      local_8 = local_8 & 0xffffff00;
      FUN_00411b00(local_1c);
      local_8 = 0xffffffff;
      FUN_0040b900(&local_20);
      goto LAB_004120e2;
    }
    FUN_00411c30(local_1c);
  } while( true );
}

