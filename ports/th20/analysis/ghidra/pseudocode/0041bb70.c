/* Entry: 0x0041bb70; symbol: FUN_0041bb70; body bytes: 181 */

void * __cdecl FUN_0041bb70(void *param_1)

{
  char cVar1;
  uint *puVar2;
  int iVar3;
  int local_8;
  
  puVar2 = (uint *)FUN_0040c300((undefined4 *)&stack0x00000008);
  iVar3 = FUN_0040ff90((int)&stack0x00000008);
  local_8 = (int)puVar2 + iVar3 * 2;
  iVar3 = FUN_0041b8e0(puVar2,local_8);
  for (; iVar3 != local_8; local_8 = local_8 + -2) {
    cVar1 = FUN_00419150(*(short *)(local_8 + -2));
    if (cVar1 != '\0') break;
  }
  do {
    if (iVar3 == local_8) {
LAB_0041bc09:
      FUN_0040de00(param_1,puVar2,local_8 - (int)puVar2 >> 1);
      return param_1;
    }
    cVar1 = FUN_00419150(*(short *)(local_8 + -2));
    if (cVar1 == '\0') goto LAB_0041bc09;
    local_8 = local_8 + -2;
  } while( true );
}

