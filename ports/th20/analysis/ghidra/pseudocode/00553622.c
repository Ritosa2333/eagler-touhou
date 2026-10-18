/* Entry: 0x00553622; symbol: FUN_00553622; body bytes: 275 */

undefined4 __fastcall FUN_00553622(undefined4 *param_1)

{
  uint *puVar1;
  undefined4 uVar2;
  byte bVar3;
  _func_void_void_ptr_ulong_void_ptr *p_Var4;
  _func_void_void_ptr_ulong_void_ptr *p_Var5;
  _func_void_void_ptr_ulong_void_ptr *p_Var6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  _func_void_void_ptr_ulong_void_ptr *local_8;
  
  puVar1 = *(uint **)*param_1;
  if (puVar1 == (uint *)0x0) {
LAB_0055372d:
    uVar2 = 0xffffffff;
  }
  else {
    bVar3 = (byte)DAT_005b25c0 & 0x1f;
    p_Var4 = (_func_void_void_ptr_ulong_void_ptr *)
             ((*puVar1 ^ DAT_005b25c0) >> bVar3 | (*puVar1 ^ DAT_005b25c0) << 0x20 - bVar3);
    p_Var5 = (_func_void_void_ptr_ulong_void_ptr *)
             ((puVar1[1] ^ DAT_005b25c0) >> bVar3 | (puVar1[1] ^ DAT_005b25c0) << 0x20 - bVar3);
    p_Var6 = (_func_void_void_ptr_ulong_void_ptr *)
             ((puVar1[2] ^ DAT_005b25c0) >> bVar3 | (puVar1[2] ^ DAT_005b25c0) << 0x20 - bVar3);
    local_8 = p_Var4;
    if (p_Var5 == p_Var6) {
      uVar7 = (int)p_Var6 - (int)p_Var4 >> 2;
      uVar8 = uVar7;
      if (0x200 < uVar7) {
        uVar8 = 0x200;
      }
      uVar9 = uVar8 + uVar7;
      if (uVar8 + uVar7 == 0) {
        uVar9 = 0x20;
      }
      if (uVar9 < uVar7) {
LAB_005536a3:
        uVar9 = uVar7 + 4;
        local_8 = (_func_void_void_ptr_ulong_void_ptr *)__recalloc_base(p_Var4,uVar9,4);
        FUN_00557ba0((LPVOID)0x0);
        if (local_8 == (_func_void_void_ptr_ulong_void_ptr *)0x0) goto LAB_0055372d;
      }
      else {
        local_8 = (_func_void_void_ptr_ulong_void_ptr *)__recalloc_base(p_Var4,uVar9,4);
        FUN_00557ba0((LPVOID)0x0);
        if (local_8 == (_func_void_void_ptr_ulong_void_ptr *)0x0) goto LAB_005536a3;
      }
      uVar8 = DAT_005b25c0;
      p_Var5 = local_8 + uVar7 * 4;
      p_Var6 = local_8 + uVar9 * 4;
      uVar7 = (uint)(p_Var6 + (3 - (int)p_Var5)) >> 2;
      if (p_Var6 < p_Var5) {
        uVar7 = 0;
      }
      p_Var4 = p_Var5;
      if (uVar7 != 0) {
        for (; uVar7 != 0; uVar7 = uVar7 - 1) {
          *(uint *)p_Var4 = uVar8;
          p_Var4 = p_Var4 + 4;
        }
      }
    }
    p_Var4 = __crt_fast_encode_pointer<void_(__stdcall*)(void*,unsigned_long,void*)>
                       (*(_func_void_void_ptr_ulong_void_ptr **)param_1[1]);
    *(_func_void_void_ptr_ulong_void_ptr **)p_Var5 = p_Var4;
    p_Var4 = __crt_fast_encode_pointer<void_(__stdcall*)(void*,unsigned_long,void*)>(local_8);
    **(undefined4 **)*param_1 = p_Var4;
    p_Var5 = __crt_fast_encode_pointer<void_(__stdcall*)(void*,unsigned_long,void*)>(p_Var5 + 4);
    *(_func_void_void_ptr_ulong_void_ptr **)(*(int *)*param_1 + 4) = p_Var5;
    p_Var5 = __crt_fast_encode_pointer<void_(__stdcall*)(void*,unsigned_long,void*)>(p_Var6);
    *(_func_void_void_ptr_ulong_void_ptr **)(*(int *)*param_1 + 8) = p_Var5;
    uVar2 = 0;
  }
  return uVar2;
}

