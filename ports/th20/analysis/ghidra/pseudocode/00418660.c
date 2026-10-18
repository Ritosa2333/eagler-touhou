/* Entry: 0x00418660; symbol: FUN_00418660; body bytes: 87 */

int __cdecl
FUN_00418660(undefined1 (*param_1) [32],uint param_2,undefined1 (*param_3) [32],uint param_4)

{
  uint *puVar1;
  int iVar2;
  
  puVar1 = _Min_value<>(&param_2,&param_4);
  iVar2 = FUN_0041c100(param_1,param_3,*puVar1);
  if (iVar2 == 0) {
    if (param_2 < param_4) {
      iVar2 = -1;
    }
    else if (param_4 < param_2) {
      iVar2 = 1;
    }
    else {
      iVar2 = 0;
    }
  }
  return iVar2;
}

