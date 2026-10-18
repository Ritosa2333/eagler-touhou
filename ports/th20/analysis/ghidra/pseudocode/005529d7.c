/* Entry: 0x005529d7; symbol: FUN_005529d7; body bytes: 326 */

undefined4 __cdecl FUN_005529d7(uint param_1,undefined4 param_2)

{
  uint *puVar1;
  code *pcVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  __acrt_ptd *p_Var5;
  undefined4 uVar6;
  uint *puVar7;
  uint *puVar8;
  
  p_Var5 = FUN_00557a13();
  if (p_Var5 != (__acrt_ptd *)0x0) {
    puVar1 = *(uint **)p_Var5;
    for (puVar7 = puVar1; puVar7 != puVar1 + 0x24; puVar7 = puVar7 + 3) {
      if (*puVar7 == param_1) {
        if (puVar7 == (uint *)0x0) {
          return 0;
        }
        pcVar2 = (code *)puVar7[2];
        if (pcVar2 == (code *)0x0) {
          return 0;
        }
        if (pcVar2 == (code *)0x5) {
          puVar7[2] = 0;
          return 1;
        }
        if (pcVar2 != (code *)0x1) {
          uVar3 = *(undefined4 *)(p_Var5 + 4);
          *(undefined4 *)(p_Var5 + 4) = param_2;
          if (puVar7[1] == 8) {
            for (puVar8 = puVar1 + 9; puVar8 != puVar1 + 0x24; puVar8 = puVar8 + 3) {
              puVar8[2] = 0;
            }
            uVar4 = *(undefined4 *)(p_Var5 + 8);
            uVar6 = uVar4;
            if (*puVar7 < 0xc0000092) {
              if (*puVar7 == 0xc0000091) {
                uVar6 = 0x84;
              }
              else if (*puVar7 == 0xc000008d) {
                uVar6 = 0x82;
              }
              else if (*puVar7 == 0xc000008e) {
                uVar6 = 0x83;
              }
              else if (*puVar7 == 0xc000008f) {
                uVar6 = 0x86;
              }
              else {
                if (*puVar7 != 0xc0000090) goto LAB_00552aeb;
                uVar6 = 0x81;
              }
LAB_00552ae8:
              *(undefined4 *)(p_Var5 + 8) = uVar6;
            }
            else {
              if (*puVar7 == 0xc0000092) {
                uVar6 = 0x8a;
                goto LAB_00552ae8;
              }
              if (*puVar7 == 0xc0000093) {
                uVar6 = 0x85;
                goto LAB_00552ae8;
              }
              if (*puVar7 == 0xc00002b4) {
                uVar6 = 0x8e;
                goto LAB_00552ae8;
              }
              if (*puVar7 == 0xc00002b5) {
                uVar6 = 0x8d;
                goto LAB_00552ae8;
              }
            }
LAB_00552aeb:
            (*(code *)PTR_guard_check_icall_0056c37c)(8,uVar6);
            (*pcVar2)();
            *(undefined4 *)(p_Var5 + 8) = uVar4;
          }
          else {
            puVar7[2] = 0;
            (*(code *)PTR_guard_check_icall_0056c37c)(puVar7[1]);
            (*pcVar2)();
          }
          *(undefined4 *)(p_Var5 + 4) = uVar3;
        }
        return 0xffffffff;
      }
    }
  }
  return 0;
}

