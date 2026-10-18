/* Entry: 0x0044c9b0; symbol: FUN_0044c9b0; body bytes: 296 */

undefined4 * __fastcall FUN_0044c9b0(int param_1)

{
  int iVar1;
  undefined4 *puVar2;
  void *pvVar3;
  
  iVar1 = FUN_0040ff90(param_1 + 0x6000718);
  if (iVar1 == 0) {
    puVar2 = FUN_00447800();
    FUN_004299d0((int)puVar2);
    FUN_00429e30(puVar2);
    puVar2[0x131] = 0xffff;
  }
  else {
    puVar2 = (undefined4 *)FUN_0040ff90(param_1 + 0x6000718);
    puVar2 = (undefined4 *)FUN_0040c300(puVar2);
    FUN_00411ce0((int)(puVar2 + 0x179));
    puVar2[0x131] = puVar2[0x17f];
    *(undefined1 *)(puVar2 + 0x17e) = 1;
    FUN_004299d0((int)puVar2);
    FUN_00429e30(puVar2);
    puVar2[0x156] = 0;
    puVar2[0x157] = 0;
    FUN_0044a670(puVar2 + 0x13b,puVar2);
    pvVar3 = (void *)FUN_004494b0(puVar2 + 0x145,0);
    FUN_0044a670(pvVar3,puVar2);
    pvVar3 = (void *)FUN_004494b0(puVar2 + 0x145,1);
    FUN_0044a670(pvVar3,puVar2);
  }
  return puVar2;
}

