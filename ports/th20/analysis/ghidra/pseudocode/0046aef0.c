/* Entry: 0x0046aef0; symbol: FUN_0046aef0; body bytes: 466 */

void __fastcall FUN_0046aef0(undefined4 *param_1)

{
  bool bVar1;
  int *piVar2;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var3;
  int *piVar4;
  int *unaff_FS_OFFSET;
  int local_1c [2];
  char **local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00568d50;
  local_10 = *unaff_FS_OFFSET;
  local_14 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR_FUN_0056f5c0;
  std::_Adl_verify_range<char*,char_const*>((char **)"shutdown AsciiInf\n",local_14);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[2]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[3]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[0x6894]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[0x6895]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[0x6896]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[0x6897]);
  FUN_0040bcf0(param_1 + 0x6889);
  FUN_0044c430(DAT_005c0028,2);
  FUN_0044c430(DAT_005c0028,0);
  DAT_005c0698 = 0;
  FUN_0040c080(local_1c,8);
  FUN_00412280(param_1 + 0x6883,local_1c);
  piVar2 = (int *)FUN_00412540();
  while( true ) {
    bVar1 = FUN_00411bb0(local_1c,piVar2);
    if (!bVar1) break;
    p_Var3 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0040c300(local_1c);
    piVar4 = (int *)FUN_0040f160(p_Var3);
    FUN_0046a090(piVar4);
    FUN_00411c30(local_1c);
  }
  FUN_00411b00(local_1c);
  FUN_004155b0();
  FUN_0046aed0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(param_1 + 0x688d));
  FUN_0040b980(param_1 + 0x6889);
  FUN_00449370((int)(param_1 + 0x2f6));
  FUN_00449370((int)(param_1 + 0x17d));
  FUN_00449370((int)(param_1 + 4));
  FUN_0041fdf0(param_1);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee((uint)local_14 ^ (uint)&stack0xfffffffc);
  return;
}

