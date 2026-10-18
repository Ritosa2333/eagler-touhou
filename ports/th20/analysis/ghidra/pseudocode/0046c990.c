/* Entry: 0x0046c990; symbol: FUN_0046c990; body bytes: 170 */

void __cdecl FUN_0046c990(void *param_1,undefined4 *param_2,wchar_t *param_3)

{
  undefined4 uVar1;
  wchar_t *_Dst;
  int *unaff_FS_OFFSET;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005679cd;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,0x12);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  _Dst = (wchar_t *)FUN_00470620((int)param_1);
  _vswprintf_s(_Dst,0x100,param_3,&stack0x00000010);
  FUN_0046ca40(param_1,param_2);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

