/* Entry: 0x00447200; symbol: FUN_00447200; body bytes: 73 */

bool __cdecl FUN_00447200(uint *param_1,uint param_2,uint *param_3,uint param_4)

{
  int iVar1;
  bool bVar2;
  
  if (param_2 == param_4) {
    if (param_2 == 0) {
      bVar2 = true;
    }
    else {
      iVar1 = compare(param_1,param_3,param_2);
      bVar2 = iVar1 == 0;
    }
  }
  else {
    bVar2 = false;
  }
  return bVar2;
}

