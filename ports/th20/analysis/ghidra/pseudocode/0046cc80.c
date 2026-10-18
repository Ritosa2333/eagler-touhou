/* Entry: 0x0046cc80; symbol: FUN_0046cc80; body bytes: 190 */

void __thiscall FUN_0046cc80(void *this,undefined4 *param_1,uint param_2)

{
  undefined4 uVar1;
  char *pcVar2;
  int *unaff_FS_OFFSET;
  undefined8 uVar3;
  undefined1 *local_28;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005676ed;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,0x12);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  pcVar2 = (char *)FUN_00470620((int)this);
  uVar3 = FUN_00459210(pcVar2,pcVar2 + 0xff,param_2,10);
  local_28 = (undefined1 *)uVar3;
  *local_28 = 0;
  FUN_0046ca40(this,param_1);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

