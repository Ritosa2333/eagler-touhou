/* Entry: 0x004d82c0; symbol: FUN_004d82c0; body bytes: 134 */

undefined4 FUN_004d82c0(void)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  
  iVar1 = FUN_004b5900();
  if (iVar1 == 0) {
    puVar3 = FUN_0049e0c0(0);
    if (puVar3 == (undefined4 *)0x0) {
      uVar2 = 0xffffffff;
    }
    else {
      puVar3 = FUN_00534dd0(0);
      if (puVar3 == (undefined4 *)0x0) {
        uVar2 = 0xffffffff;
      }
      else {
        puVar3 = FUN_0051cc20(0);
        if (puVar3 == (undefined4 *)0x0) {
          uVar2 = 0xffffffff;
        }
        else {
          iVar1 = FUN_004a7700();
          if (iVar1 == 0) {
            iVar1 = FUN_004a7700();
            if (iVar1 == 0) {
              iVar1 = FUN_0052e210();
              if (iVar1 == 0) {
                iVar1 = FUN_004a7700();
                if (iVar1 == 0) {
                  uVar2 = 0;
                }
                else {
                  uVar2 = 0xffffffff;
                }
              }
              else {
                uVar2 = 0xffffffff;
              }
            }
            else {
              uVar2 = 0xffffffff;
            }
          }
          else {
            uVar2 = 0xffffffff;
          }
        }
      }
    }
  }
  else {
    uVar2 = 0xffffffff;
  }
  return uVar2;
}

