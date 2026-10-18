/* Entry: 0x004dc5f0; symbol: FUN_004dc5f0; body bytes: 449 */

undefined4 __cdecl FUN_004dc5f0(void *param_1)

{
  int *piVar1;
  char **ppcVar2;
  int iVar3;
  
  if (DAT_005c4edc == 0) {
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0xac))(piVar1,0,0,3,DAT_005c5b20,DAT_0056c8cc,0);
  }
  else {
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0xac))(piVar1,0,0,3,0xffffffff,DAT_0056c8cc,0);
    ppcVar2 = (char **)FUN_00412730(0x5c4d40);
    iVar3 = (**(code **)(*ppcVar2 + 0x94))(ppcVar2,0,DAT_005c4edc);
    if (iVar3 != 0) {
      std::_Adl_verify_range<char*,char_const*>((char **)"error SetRenderTarget\n",ppcVar2);
    }
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0xac))(piVar1,0,0,3,DAT_005c5b20,DAT_0056c8cc,0);
  }
  FUN_00424080((int)DAT_005c0028);
  FUN_0041dba0((int)DAT_005c0028);
  FUN_0041db60((int)DAT_005c0028);
  FUN_0041db40((int)DAT_005c0028);
  FUN_004240a0((int)DAT_005c0028);
  FUN_0041dbc0((int)DAT_005c0028);
  FUN_00475120((int)DAT_005c0028);
  FUN_004dd820((int)DAT_005c0028);
  FUN_00475150(DAT_005c0028);
  *(undefined4 *)(DAT_005c0028 + 0xcc) = 0;
  *(undefined4 *)(DAT_005c0028 + 200) = 0;
  FUN_0041db80((int)DAT_005c0028);
  FUN_0041dce0(param_1,2);
  return 1;
}

