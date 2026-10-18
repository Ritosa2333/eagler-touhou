/* Entry: 0x0053a3c0; symbol: FUN_0053a3c0; body bytes: 576 */

/* WARNING: Removing unreachable block (ram,0x0053a529) */

uint * __thiscall FUN_0053a3c0(void *this,byte *param_1,uint *param_2)

{
  SIZE_T SVar1;
  char cVar2;
  byte bVar3;
  undefined4 *puVar4;
  int iVar5;
  size_t sVar6;
  SIZE_T SVar7;
  uint *local_20;
  uint *local_c;
  
  local_c = (uint *)0x0;
  if (*(int *)((int)this + 0xc) == 0) {
    return (uint *)0x0;
  }
  puVar4 = FUN_0053a350(this,param_1);
  if (puVar4 != (undefined4 *)0x0) {
    SVar7 = puVar4[5] - puVar4[1];
    SVar1 = puVar4[2];
    if ((SVar7 == SVar1) && (param_2 != (uint *)0x0)) {
      local_c = param_2;
    }
    else {
      local_c = (uint *)FUN_0041f610(SVar7);
      if (local_c == (uint *)0x0) goto LAB_0053a5d5;
    }
    cVar2 = (**(code **)(**(int **)((int)this + 0xc) + 0x18))(puVar4[1],DAT_005767d4);
    if ((cVar2 != '\0') &&
       (iVar5 = (**(code **)(**(int **)((int)this + 0xc) + 8))(local_c,SVar7), iVar5 != 0)) {
      sVar6 = _strlen((char *)*puVar4);
      bVar3 = FUN_00456270((char *)*puVar4,sVar6);
      bVar3 = bVar3 & 7;
      FUN_004100e0(local_c,SVar7,(&DAT_005ae000)[(uint)bVar3 * 0xc],
                   (&DAT_005ae001)[(uint)bVar3 * 0xc],*(int *)(&DAT_005ae004 + (uint)bVar3 * 0xc),
                   *(uint *)(&DAT_005ae008 + (uint)bVar3 * 0xc));
      if (SVar7 == SVar1) {
        local_20 = local_c;
      }
      else {
        local_20 = (uint *)FUN_005391f0((byte *)local_c,SVar7,(undefined1 *)param_2,SVar1);
      }
      if (local_c == param_2) {
        return local_20;
      }
      if (local_c == (uint *)0x0) {
        return local_20;
      }
      if (local_c == (uint *)0x0) {
        return local_20;
      }
      FUN_0041f670(local_c);
      return local_20;
    }
  }
LAB_0053a5d5:
  std::_Adl_verify_range<char*,char_const*>
            ((char **)"info : %s error\r\n",*(char ***)((int)this + 8));
  if ((local_c != (uint *)0x0) && (local_c != (uint *)0x0)) {
    FUN_0041f670(local_c);
  }
  return (uint *)0x0;
}

