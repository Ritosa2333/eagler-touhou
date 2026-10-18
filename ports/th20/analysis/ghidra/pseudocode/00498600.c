/* Entry: 0x00498600; symbol: FUN_00498600; body bytes: 663 */

undefined4 * __thiscall FUN_00498600(void *this,undefined4 param_1)

{
  void *pvVar1;
  int iVar2;
  undefined4 *puVar3;
  
  switch(param_1) {
  case 0xffffd8ff:
    puVar3 = (undefined4 *)((int)this + 0x100);
    break;
  case 0xffffd900:
    puVar3 = (undefined4 *)((int)this + 0x104);
    break;
  case 0xffffd901:
    puVar3 = (undefined4 *)((int)this + 0x108);
    break;
  case 0xffffd902:
    puVar3 = (undefined4 *)((int)this + 0x10c);
    break;
  default:
    puVar3 = (undefined4 *)0x0;
    break;
  case 0xffffd923:
    iVar2 = FUN_00412730(*(int *)((int)this + 0x424));
    puVar3 = (undefined4 *)(iVar2 + 0x48);
    break;
  case 0xffffd924:
    iVar2 = FUN_00412730(*(int *)((int)this + 0x424));
    puVar3 = (undefined4 *)(iVar2 + 0x4c);
    break;
  case 0xffffd925:
    iVar2 = FUN_00412730(*(int *)((int)this + 0x424));
    puVar3 = (undefined4 *)(iVar2 + 0x50);
    break;
  case 0xffffd929:
    pvVar1 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar2 = FUN_00485660(pvVar1,0);
    if (iVar2 == 0) {
      puVar3 = (undefined4 *)((int)this + 0x100);
    }
    else {
      pvVar1 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
      iVar2 = FUN_00485660(pvVar1,0);
      puVar3 = (undefined4 *)(iVar2 + 0x100);
    }
    break;
  case 0xffffd92a:
    pvVar1 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar2 = FUN_00485660(pvVar1,0);
    if (iVar2 == 0) {
      puVar3 = (undefined4 *)((int)this + 0x104);
    }
    else {
      pvVar1 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
      iVar2 = FUN_00485660(pvVar1,0);
      puVar3 = (undefined4 *)(iVar2 + 0x104);
    }
    break;
  case 0xffffd92b:
    pvVar1 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar2 = FUN_00485660(pvVar1,0);
    if (iVar2 == 0) {
      puVar3 = (undefined4 *)((int)this + 0x108);
    }
    else {
      pvVar1 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
      iVar2 = FUN_00485660(pvVar1,0);
      puVar3 = (undefined4 *)(iVar2 + 0x108);
    }
    break;
  case 0xffffd92c:
    pvVar1 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar2 = FUN_00485660(pvVar1,0);
    if (iVar2 == 0) {
      puVar3 = (undefined4 *)((int)this + 0x10c);
    }
    else {
      pvVar1 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
      iVar2 = FUN_00485660(pvVar1,0);
      puVar3 = (undefined4 *)(iVar2 + 0x10c);
    }
    break;
  case 0xffffd93a:
    iVar2 = FUN_00412730(*(int *)((int)this + 0x424));
    puVar3 = (undefined4 *)(iVar2 + 0x10);
    break;
  case 0xffffd93b:
    iVar2 = FUN_00412730(*(int *)((int)this + 0x424));
    puVar3 = (undefined4 *)(iVar2 + 0x14);
    break;
  case 0xffffd93c:
    iVar2 = FUN_00412730(*(int *)((int)this + 0x424));
    puVar3 = (undefined4 *)(iVar2 + 0x18);
    break;
  case 0xffffd93d:
    iVar2 = FUN_00412730(*(int *)((int)this + 0x424));
    puVar3 = (undefined4 *)(iVar2 + 0x1c);
    break;
  case 0xffffd959:
    puVar3 = &DAT_005c49d8;
    break;
  case 0xffffd95a:
    puVar3 = &DAT_005c49dc;
    break;
  case 0xffffd95b:
    puVar3 = &DAT_005c49e0;
    break;
  case 0xffffd95c:
    puVar3 = &DAT_005c49e4;
  }
  return puVar3;
}

