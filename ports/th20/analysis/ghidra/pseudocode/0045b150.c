/* Entry: 0x0045b150; symbol: FUN_0045b150; body bytes: 701 */

void __thiscall FUN_0045b150(void *this,int param_1)

{
  undefined4 uVar1;
  LPVOID pvVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  uint local_40;
  uint local_38;
  undefined4 local_30 [4];
  int local_20;
  int *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *(undefined4 *)((int)this + 0x58) = 0;
  if (*(int *)((int)this + 8) != 0) {
    for (local_38 = 0; local_38 < *(uint *)((int)this + 0x14); local_38 = local_38 + 1) {
      if ((*(int *)((int)this + 8) != 0) && (*(int *)(*(int *)((int)this + 8) + local_38 * 4) != 0))
      {
        (**(code **)(**(int **)(*(int *)((int)this + 8) + local_38 * 4) + 8))
                  (*(undefined4 *)(*(int *)((int)this + 8) + local_38 * 4));
      }
    }
    if (*(int *)((int)this + 8) != 0) {
      FUN_0040d840(*(LPVOID *)((int)this + 8));
      *(undefined4 *)((int)this + 8) = 0;
    }
  }
  local_c = (int *)0x0;
  uVar1 = FUN_00447480("D:\\cygwin\\home\\zun\\prog\\th20\\src\\core\\zwave.cpp:322 LPDIRECTSOUNDBUFFER"
                       ,*(uint *)((int)this + 0x14));
  *(undefined4 *)((int)this + 8) = uVar1;
  puVar4 = (undefined4 *)((int)this + 0x60);
  puVar5 = local_30;
  for (iVar3 = 9; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar5 = *puVar4;
    puVar4 = puVar4 + 1;
    puVar5 = puVar5 + 1;
  }
  local_20 = param_1 + 0x20;
  local_38 = 0;
  do {
    if (((*(uint *)((int)this + 0x14) <= local_38) ||
        (iVar3 = (**(code **)(*(int *)**(undefined4 **)((int)this + 0x84) + 0xc))
                           (**(undefined4 **)((int)this + 0x84),local_30,
                            *(int *)((int)this + 8) + local_38 * 4,0), iVar3 < 0)) ||
       (iVar3 = (**(code **)**(undefined4 **)(*(int *)((int)this + 8) + local_38 * 4))
                          (*(undefined4 *)(*(int *)((int)this + 8) + local_38 * 4),&DAT_00576c28,
                           &local_c), iVar3 < 0)) goto LAB_0045b3fb;
    if (local_c != (int *)0x0) {
      pvVar2 = (LPVOID)FUN_00459350("D:\\cygwin\\home\\zun\\prog\\th20\\src\\core\\zwave.cpp:336 DSBPOSITIONNOTIFY"
                                    ,0x10);
      if (pvVar2 == (LPVOID)0x0) goto LAB_0045b3fb;
      for (local_40 = 0; local_40 < 0x10; local_40 = local_40 + 1) {
        *(uint *)((int)pvVar2 + local_40 * 8) =
             *(int *)((int)this + 0x9c) * local_40 + -1 + *(int *)((int)this + 0x9c);
        *(undefined4 *)((int)pvVar2 + local_40 * 8 + 4) = *(undefined4 *)((int)this + 0xa0);
      }
      iVar3 = (**(code **)(*local_c + 0xc))(local_c,0x10,pvVar2);
      if (iVar3 < 0) {
        if (local_c != (int *)0x0) {
          (**(code **)(*local_c + 8))(local_c);
          local_c = (int *)0x0;
        }
        if (pvVar2 != (LPVOID)0x0) {
          FUN_0040d840(pvVar2);
        }
LAB_0045b3fb:
        FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
        return;
      }
      if (local_c != (int *)0x0) {
        (**(code **)(*local_c + 8))(local_c);
        local_c = (int *)0x0;
      }
      if (pvVar2 != (LPVOID)0x0) {
        FUN_0040d840(pvVar2);
      }
    }
    local_38 = local_38 + 1;
  } while( true );
}

