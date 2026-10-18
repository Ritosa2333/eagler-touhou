/* Entry: 0x00467000; symbol: FUN_00467000; body bytes: 111 */

ulonglong FUN_00467000(void)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  ulonglong uVar4;
  
  puVar1 = (uint *)FUN_00425620((undefined4 *)&stack0x00000004);
  iVar2 = FUN_00425620((undefined4 *)&stack0x00000008);
  iVar3 = FUN_00425620((undefined4 *)&stack0x0000000c);
  uVar4 = FUN_00543e20((uint *)(iVar3 - (iVar2 - (int)puVar1)),puVar1,iVar2 - (int)puVar1);
  return uVar4;
}

