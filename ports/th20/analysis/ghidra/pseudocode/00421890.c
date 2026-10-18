/* Entry: 0x00421890; symbol: FUN_00421890; body bytes: 181 */

void __cdecl FUN_00421890(int param_1,int param_2)

{
  undefined4 uVar1;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  uVar1 = FUN_0040c310(&DAT_005c0240,0xf);
  FUN_0040b730(&local_c,uVar1);
  if (param_1 == 0) {
LAB_004218ea:
    if (param_2 < 0x100) {
      FUN_0040b900(&local_c);
      goto LAB_00421937;
    }
  }
  else if (param_1 == 2) {
    if (param_2 < 0xc) {
      FUN_0040b900(&local_c);
      goto LAB_00421937;
    }
    goto LAB_004218ea;
  }
  FID_conflict__swprintf((wchar_t *)&DAT_005b9ee0,(wchar_t *)0xa,"Button %d",param_2);
  FUN_0040b900(&local_c);
LAB_00421937:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

