/* Entry: 0x004d97f0; symbol: FUN_004d97f0; body bytes: 270 */

void __cdecl FUN_004d97f0(ushort *param_1,undefined4 *param_2)

{
  LPVOID pvVar1;
  int iVar2;
  undefined4 *puVar3;
  uint local_18;
  uint local_10;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_c = 0;
  local_10 = 0;
  FUN_004d9920(&local_c,&local_10);
  if ((local_10 != 0) &&
     (pvVar1 = (LPVOID)FUN_004d86a0("D:\\cygwin\\home\\zun\\prog\\th20\\src\\game\\mother.cpp:483 Gdiplus::ImageCodecInfo"
                                    ,local_10), pvVar1 != (LPVOID)0x0)) {
    FUN_004d9900(local_c,local_10,pvVar1);
    for (local_18 = 0; local_18 < local_c; local_18 = local_18 + 1) {
      iVar2 = FUN_005513b0(*(ushort **)((int)pvVar1 + local_18 * 0x4c + 0x30),param_1);
      if (iVar2 == 0) {
        puVar3 = (undefined4 *)(local_18 * 0x4c + (int)pvVar1);
        *param_2 = *puVar3;
        param_2[1] = puVar3[1];
        param_2[2] = puVar3[2];
        param_2[3] = puVar3[3];
        FUN_0040d840(pvVar1);
        goto LAB_004d98f0;
      }
    }
    FUN_0040d840(pvVar1);
  }
LAB_004d98f0:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

