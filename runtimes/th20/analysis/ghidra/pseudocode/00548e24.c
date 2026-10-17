/* Entry: 0x00548e24; symbol: FUN_00548e24; body bytes: 397 */

uint __thiscall FUN_00548e24(void *this,byte param_1)

{
  int iVar1;
  uint *puVar2;
  ushort *puVar3;
  byte *pbVar4;
  uint uVar5;
  char *pcVar6;
  uint uVar7;
  uint uVar8;
  bool bVar9;
  uint local_c;
  
  uVar5 = __crt_stdio_output::to_integer_size(*(length_modifier *)((int)this + 0x28));
  if (uVar5 == 1) {
    uVar8 = *(uint *)((int)this + 0x1c);
    pbVar4 = *(byte **)((int)this + 0x10);
    *(byte **)((int)this + 0x10) = pbVar4 + 4;
    if ((uVar8 >> 4 & 1) == 0) {
      uVar7 = (uint)*pbVar4;
    }
    else {
      uVar7 = (uint)(char)*pbVar4;
    }
  }
  else if (uVar5 == 2) {
    uVar8 = *(uint *)((int)this + 0x1c);
    puVar3 = *(ushort **)((int)this + 0x10);
    *(ushort **)((int)this + 0x10) = puVar3 + 2;
    if ((uVar8 >> 4 & 1) == 0) {
      uVar7 = (uint)*puVar3;
    }
    else {
      uVar7 = (uint)(short)*puVar3;
    }
  }
  else {
    if (uVar5 != 4) {
      if (uVar5 != 8) {
        iVar1 = *(int *)((int)this + 8);
        *(undefined1 *)(iVar1 + 0x1c) = 1;
        *(undefined4 *)(iVar1 + 0x18) = 0x16;
        uVar5 = FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,
                             *(int **)((int)this + 8));
        return uVar5 & 0xffffff00;
      }
      puVar2 = *(uint **)((int)this + 0x10);
      uVar8 = *(uint *)((int)this + 0x1c);
      *(uint **)((int)this + 0x10) = puVar2 + 2;
      uVar7 = *puVar2;
      local_c = puVar2[1];
      goto LAB_00548f05;
    }
    uVar8 = *(uint *)((int)this + 0x1c);
    if ((uVar8 >> 4 & 1) == 0) {
      puVar2 = *(uint **)((int)this + 0x10);
      local_c = 0;
      *(uint **)((int)this + 0x10) = puVar2 + 1;
      uVar7 = *puVar2;
      goto LAB_00548f05;
    }
    puVar2 = *(uint **)((int)this + 0x10);
    *(uint **)((int)this + 0x10) = puVar2 + 1;
    uVar7 = *puVar2;
  }
  local_c = (int)uVar7 >> 0x1f;
LAB_00548f05:
  if ((((uVar8 >> 4 & 1) != 0) && ((int)local_c < 1)) && ((int)local_c < 0)) {
    bVar9 = uVar7 != 0;
    uVar7 = -uVar7;
    local_c = -(local_c + bVar9);
    uVar8 = uVar8 | 0x40;
    *(uint *)((int)this + 0x1c) = uVar8;
  }
  if (*(int *)((int)this + 0x24) < 0) {
    *(undefined4 *)((int)this + 0x24) = 1;
  }
  else {
    *(uint *)((int)this + 0x1c) = uVar8 & 0xfffffff7;
    FUN_00548966((void *)((int)this + 0x3c),*(uint *)((int)this + 0x24),*(int *)((int)this + 8));
  }
  if (uVar7 == 0 && local_c == 0) {
    *(uint *)((int)this + 0x1c) = *(uint *)((int)this + 0x1c) & 0xffffffdf;
  }
  *(undefined1 *)((int)this + 0x38) = 0;
  if (uVar5 == 8) {
    FUN_0054930f(this,uVar7,local_c,param_1);
  }
  else {
    FUN_005491a0(this,uVar7,param_1);
  }
  pcVar6 = (char *)(*(uint *)((int)this + 0x1c) >> 7);
  if ((((uint)pcVar6 & 1) != 0) &&
     ((*(int *)((int)this + 0x34) == 0 || (pcVar6 = *(char **)((int)this + 0x30), *pcVar6 != '0'))))
  {
    *(int *)((int)this + 0x30) = *(int *)((int)this + 0x30) + -1;
    **(undefined1 **)((int)this + 0x30) = 0x30;
    *(int *)((int)this + 0x34) = *(int *)((int)this + 0x34) + 1;
  }
  return CONCAT31((int3)((uint)pcVar6 >> 8),1);
}

