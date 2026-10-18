/* Entry: 0x0050eb70; symbol: FUN_0050eb70; body bytes: 824 */

undefined4 FUN_0050eb70(int *param_1)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  char **unaff_EDI;
  uint *puVar5;
  int local_10;
  uint *local_8;
  
  if (param_1[1] != 0) {
    if ((((*(int *)param_1[1] == 0x32304854) && (*(int *)(param_1[1] + 8) == 4)) &&
        (*(int *)(param_1[1] + 0x24) == *param_1 + -0x2c)) &&
       (((*(int *)(param_1[1] + 0xc) == 0xe0 && (*(int *)(param_1[1] + 0x10) == 0x7ae8)) &&
        ((*(int *)(param_1[1] + 0x14) == 0x1f8 && (*(int *)(param_1[1] + 0x18) == 0x92140)))))) {
      FUN_004100e0((uint *)(param_1[1] + 0x2c),*(uint *)(param_1[1] + 0x24),0xac,'5',0x10,
                   *(uint *)(param_1[1] + 0x24));
      iVar1 = param_1[1];
      iVar2 = FUN_0041f610(*(int *)(param_1[1] + 0x28) << 2);
      param_1[2] = iVar2;
      FUN_005391f0((byte *)(iVar1 + 0x2c),*(int *)(param_1[1] + 0x24),(undefined1 *)param_1[2],
                   *(SIZE_T *)(param_1[1] + 0x28));
      local_10 = *(int *)(param_1[1] + 0x28);
      local_8 = (uint *)param_1[2];
      while( true ) {
        if (local_10 < 1) {
          return 0;
        }
        if ((short)*local_8 == 0x5243) {
          if (((*(short *)((int)local_8 + 2) == 1) &&
              (uVar3 = FUN_0050fc90(local_8,0x7ae8), uVar3 == local_8[1])) && (local_8[2] == 0x7ae8)
             ) {
            if (local_8[3] == 2) {
              FUN_00543e20((uint *)(param_1 + 0x22918),local_8,0x7ae8);
            }
            else {
              FUN_00543e20((uint *)(param_1 + local_8[3] * 0x1148a + local_8[4] * 0x1eba + 4),
                           local_8,0x7ae8);
            }
          }
        }
        else {
          if ((short)*local_8 != 0x5453) {
            std::_Adl_verify_range<char*,char_const*>
                      ((char **)"error ScoreFile Data Error\n",unaff_EDI);
            return 0;
          }
          if (((*(short *)((int)local_8 + 2) == 2) &&
              (uVar3 = FUN_0050fc90(local_8,0x1f8), uVar3 == local_8[1])) && (local_8[2] == 0x1f8))
          {
            puVar4 = local_8;
            puVar5 = (uint *)(param_1 + 0x247d2);
            for (iVar1 = 0x7e; iVar1 != 0; iVar1 = iVar1 + -1) {
              *puVar5 = *puVar4;
              puVar4 = puVar4 + 1;
              puVar5 = puVar5 + 1;
            }
          }
        }
        local_10 = local_10 - local_8[2];
        if (local_10 < 0) break;
        local_8 = (uint *)((int)local_8 + local_8[2]);
      }
      std::_Adl_verify_range<char*,char_const*>((char **)"error ScoreFile Data Error\n",unaff_EDI);
      return 0;
    }
    std::_Adl_verify_range<char*,char_const*>((char **)&DAT_00573808,unaff_EDI);
  }
  std::_Adl_verify_range<char*,char_const*>((char **)"Init ScoreFile\n",unaff_EDI);
  if ((param_1[1] != 0) && (param_1[1] != 0)) {
    FUN_0041f670((LPVOID)param_1[1]);
    param_1[1] = 0;
  }
  iVar1 = FUN_0041f610(0x2c);
  param_1[1] = iVar1;
  _memset((void *)param_1[1],0,0x2c);
  *(undefined4 *)param_1[1] = 0x32304854;
  *(undefined4 *)(param_1[1] + 8) = 4;
  *(undefined4 *)(param_1[1] + 0x20) = 0x100;
  *(undefined4 *)(param_1[1] + 0xc) = 0xe0;
  *(undefined4 *)(param_1[1] + 0x10) = 0x7ae8;
  *(undefined4 *)(param_1[1] + 0x14) = 0x1f8;
  *(undefined4 *)(param_1[1] + 0x18) = 0x92140;
  return 0;
}

