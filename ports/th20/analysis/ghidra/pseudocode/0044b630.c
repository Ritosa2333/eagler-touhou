/* Entry: 0x0044b630; symbol: FUN_0044b630; body bytes: 81 */

void * __cdecl FUN_0044b630(void *param_1)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  
  puVar1 = (uint *)FUN_0040c300((undefined4 *)&stack0x00000008);
  iVar2 = FUN_0040ff90((int)&stack0x00000008);
  iVar2 = (int)puVar1 + iVar2 * 2;
  iVar3 = FUN_0044b4e0(puVar1,iVar2);
  FUN_0040de00(param_1,iVar3,iVar2 - iVar3 >> 1);
  return param_1;
}

