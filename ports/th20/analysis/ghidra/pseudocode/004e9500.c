/* Entry: 0x004e9500; symbol: FUN_004e9500; body bytes: 108 */

undefined8 __cdecl FUN_004e9500(uint *param_1,uint *param_2,undefined4 param_3,int param_4)

{
  undefined8 *puVar1;
  int *unaff_FS_OFFSET;
  undefined8 uVar2;
  undefined1 local_20 [16];
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005674f0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  puVar1 = (undefined8 *)FUN_004f1e70(local_20,param_1,param_2,param_3,param_4);
  uVar2 = FUN_0046eaf0(puVar1);
  *unaff_FS_OFFSET = local_10;
  return uVar2;
}

