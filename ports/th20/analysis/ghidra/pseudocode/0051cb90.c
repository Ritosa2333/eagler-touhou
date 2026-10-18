/* Entry: 0x0051cb90; symbol: FUN_0051cb90; body bytes: 140 */

/* WARNING: Removing unreachable block (ram,0x0051cbaf) */

uint __fastcall FUN_0051cb90(basic_streambuf<char,struct_std::char_traits<char>_> *param_1)

{
  undefined1 uVar1;
  bool bVar2;
  undefined1 *puVar3;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  int iVar4;
  undefined3 extraout_var_01;
  longlong lVar5;
  int _FileHandle;
  uint local_10;
  uint local_c;
  
  lVar5 = Gnavail();
  _FileHandle = (int)lVar5;
  if (lVar5 < 2) {
    uVar1 = FUN_0051c1e0(param_1);
    iVar4 = eof(_FileHandle);
    bVar2 = FUN_0051b5b0(iVar4,CONCAT31(extraout_var_00,uVar1));
    if (bVar2) {
      local_c = eof(_FileHandle);
    }
    else {
      uVar1 = FUN_0051cb20((int *)param_1);
      local_c = CONCAT31(extraout_var_01,uVar1);
    }
    local_10 = local_c;
  }
  else {
    puVar3 = (undefined1 *)FUN_0051a950((int)param_1);
    uVar1 = FUN_004ed5a0(*puVar3);
    local_10 = CONCAT31(extraout_var,uVar1);
  }
  return local_10;
}

