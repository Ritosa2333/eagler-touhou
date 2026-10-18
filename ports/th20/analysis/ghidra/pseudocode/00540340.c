/* Entry: 0x00540340; symbol: FUN_00540340; body bytes: 142 */

int __thiscall FUN_00540340(void *this,char *param_1)

{
  int iVar1;
  undefined4 *puVar2;
  int iVar3;
  int local_10;
  int local_c;
  
  local_c = 0;
  local_10 = *(int *)((int)this + 8) + -1;
  while( true ) {
    if (local_10 < local_c) {
      return -1;
    }
    iVar1 = (local_10 - local_c) / 2 + local_c;
    puVar2 = (undefined4 *)FUN_0053b590((void *)((int)this + 0x20c),iVar1);
    iVar3 = _strcmp(param_1,(char *)*puVar2);
    if (iVar3 == 0) break;
    if (iVar3 < 0) {
      local_10 = iVar1 + -1;
    }
    else {
      local_c = iVar1 + 1;
    }
  }
  return iVar1;
}

