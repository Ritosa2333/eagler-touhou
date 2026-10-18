/* Entry: 0x0053e2b0; symbol: FUN_0053e2b0; body bytes: 217 */

undefined4 __thiscall FUN_0053e2b0(void *this,float param_1)

{
  bool bVar1;
  char **ppcVar2;
  undefined4 uVar3;
  int iVar4;
  char **local_c;
  
  bVar1 = true;
  ppcVar2 = (char **)((int)this + 0x5c);
  while( true ) {
    while( true ) {
      local_c = ppcVar2;
      if (local_c == (char **)0x0) {
        *(int *)((int)this + 0xc) = (int)this + 0x10;
        return 0;
      }
      ppcVar2 = (char **)FUN_0040ff90((int)local_c);
      uVar3 = FUN_0040c300(local_c);
      *(undefined4 *)((int)this + 0xc) = uVar3;
      if (bVar1) break;
      iVar4 = FUN_0053b5c0(*(void **)((int)this + 0xc),param_1);
      if (iVar4 != 0) {
        FUN_0048afa0(*(void **)((int)this + 0xc));
        FUN_00411ce0((int)local_c);
        FUN_00413b70(DAT_005b8894,local_c);
      }
    }
    iVar4 = FUN_0053b5c0(*(void **)((int)this + 0xc),param_1);
    if (iVar4 != 0) break;
    bVar1 = false;
  }
  return 0xffffffff;
}

