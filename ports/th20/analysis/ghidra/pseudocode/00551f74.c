/* Entry: 0x00551f74; symbol: operator(); body bytes: 350 */

/* Library Function - Single Match
    public: char * __thiscall <lambda_2116bde18c9e5f34230805ea4a4660ed>::operator()(void)const 
   
   Library: Visual Studio 2019 Release */

char * __thiscall
<lambda_2116bde18c9e5f34230805ea4a4660ed>::operator()
          (<lambda_2116bde18c9e5f34230805ea4a4660ed> *this)

{
  int *piVar1;
  wchar_t *pwVar2;
  int iVar3;
  undefined4 *puVar4;
  int iVar5;
  int local_14;
  undefined4 local_10;
  int local_c;
  uint local_8;
  
  pwVar2 = FUN_005520d3(**(int **)this,(byte *)**(undefined4 **)(this + 4));
  if (pwVar2 != (wchar_t *)0x0) {
    local_c = FUN_005579c2();
    local_14 = *(int *)(local_c + 0x4c);
    local_10 = *(undefined4 *)(local_c + 0x48);
    local_8 = 0;
    iVar3 = FUN_0055da96(&local_8,(uint *)0x0,0,pwVar2,0,&local_14);
    if (iVar3 == 0) {
      puVar4 = (undefined4 *)FUN_005584c0(local_8 + 4);
      if (puVar4 != (undefined4 *)0x0) {
        iVar5 = FUN_0055da96((uint *)0x0,puVar4 + 1,local_8,pwVar2,0xffffffff,&local_14);
        iVar3 = local_14;
        if (iVar5 == 0) {
          piVar1 = *(int **)(local_14 + 0x24 + **(int **)this * 0x10);
          if (piVar1 != (int *)0x0) {
            LOCK();
            iVar5 = *piVar1;
            *piVar1 = iVar5 + -1;
            UNLOCK();
            if (iVar5 + -1 == 0) {
              FUN_00557ba0(*(LPVOID *)(local_14 + 0x24 + **(int **)this * 0x10));
              *(undefined4 *)(iVar3 + 0x24 + **(int **)this * 0x10) = 0;
            }
          }
          if (((*(uint *)(local_c + 0x350) & DAT_005b29f8) == 0) &&
             (piVar1 = *(int **)(iVar3 + 0x24 + **(int **)this * 0x10), piVar1 != (int *)0x0)) {
            LOCK();
            iVar5 = *piVar1;
            *piVar1 = *piVar1 + -1;
            UNLOCK();
            if (iVar5 == 1) {
              FUN_00557ba0(*(LPVOID *)(iVar3 + 0x24 + **(int **)this * 0x10));
              *(undefined4 *)(iVar3 + 0x24 + **(int **)this * 0x10) = 0;
            }
          }
          *puVar4 = *(undefined4 *)(iVar3 + 0xc);
          *(undefined4 **)(iVar3 + 0x24 + **(int **)this * 0x10) = puVar4;
          *(undefined4 **)(iVar3 + 0x1c + **(int **)this * 0x10) = puVar4 + 1;
          return (char *)(puVar4 + 1);
        }
        if ((iVar5 == 0x16) || (iVar5 == 0x22)) goto LAB_005520c8;
        FUN_00557ba0(puVar4);
      }
    }
    else if ((iVar3 == 0x16) || (iVar3 == 0x22)) {
LAB_005520c8:
                    /* WARNING: Subroutine does not return */
      __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
  }
  return (char *)0x0;
}

