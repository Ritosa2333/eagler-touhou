/* Entry: 0x0055ddbb; symbol: FUN_0055ddbb; body bytes: 144 */

uint __cdecl FUN_0055ddbb(uint param_1,uint param_2)

{
  int iVar1;
  bool bVar2;
  
  if (param_1 < 0xdead) {
    if (param_1 == 0xdeac) {
      return 0;
    }
    if (param_1 < 0xc434) {
      if (param_1 == 0xc433) {
        return 0;
      }
      if (param_1 == 0x2a) {
        return 0;
      }
      if (param_1 == 0xc42c) {
        return 0;
      }
      if (param_1 == 0xc42d) {
        return 0;
      }
      if (param_1 == 0xc42e) {
        return 0;
      }
      iVar1 = param_1 - 0xc431;
      goto LAB_0055de3b;
    }
    if (param_1 == 0xc435) {
      return 0;
    }
    if (param_1 == 0xd698) {
      return 0;
    }
    iVar1 = param_1 - 0xdeaa;
    bVar2 = iVar1 == 0;
  }
  else if (param_1 < 0xdeb2) {
    if (param_1 == 0xdeb1) {
      return 0;
    }
    if (param_1 == 0xdead) {
      return 0;
    }
    if (param_1 == 0xdeae) {
      return 0;
    }
    iVar1 = param_1 - 0xdeaf;
    bVar2 = iVar1 == 0;
  }
  else {
    if (param_1 == 0xdeb2) {
      return 0;
    }
    if (param_1 == 0xdeb3) {
      return 0;
    }
    iVar1 = param_1 - 65000;
    bVar2 = iVar1 == 0;
  }
  if (bVar2) {
    return 0;
  }
  iVar1 = iVar1 + -1;
LAB_0055de3b:
  if (iVar1 == 0) {
    return 0;
  }
  return param_2 & 0xffffff7f;
}

