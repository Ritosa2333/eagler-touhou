/* Entry: 0x004a3b10; symbol: FUN_004a3b10; body bytes: 456 */

void __fastcall FUN_004a3b10(undefined4 *param_1)

{
  bool bVar1;
  int *piVar2;
  undefined4 *puVar3;
  int iVar4;
  void *pvVar5;
  int *unaff_FS_OFFSET;
  undefined4 *local_24;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00569710;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR_FUN_005703cc;
  FUN_0040c080(local_1c,8);
  FUN_00412280(param_1 + 0xf3,local_1c);
  piVar2 = (int *)FUN_00412540();
  while( true ) {
    bVar1 = FUN_00411bb0(local_1c,piVar2);
    if (!bVar1) break;
    puVar3 = (undefined4 *)FUN_0040c300(local_1c);
    iVar4 = FUN_0040c300(puVar3);
    FUN_00497240(iVar4);
    FUN_00411c30(local_1c);
  }
  FUN_00411b00(local_1c);
  FUN_00411ce0((int)(param_1 + 0xf9));
  pvVar5 = (void *)FUN_00412730(param_1[0x109]);
  FUN_004a55d0(pvVar5,(int)param_1);
  if (((uint)param_1[0xd5] >> 0x12 & 1) == 0) {
    if (((uint)param_1[0xd5] >> 7 & 1) != 0) {
      pvVar5 = (void *)FUN_00412730(param_1[0x109]);
      FUN_0049c2e0(pvVar5,param_1[0xbe],(void *)0x0);
    }
    local_24 = (undefined4 *)FUN_00414f30((int)(param_1 + 0x25));
    puVar3 = (undefined4 *)FUN_00414f70((int)(param_1 + 0x25));
    for (; local_24 != puVar3; local_24 = local_24 + 5) {
      FUN_0044fcd0(local_24);
    }
  }
  FUN_0048af30((void *)param_1[0xd7]);
  param_1[0xd7] = 0;
  FUN_00414400(param_1 + 0xfe);
  FUN_004a3ac0((int)(param_1 + 0x22));
  FUN_004a3ce0(param_1);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

