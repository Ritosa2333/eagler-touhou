/* Entry: 0x00563eb5; symbol: __trandisp2; body bytes: 154 */

/* Library Function - Single Match
    __trandisp2
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void __fastcall __trandisp2(undefined4 param_1,int param_2)

{
  byte bVar1;
  float10 fVar2;
  int iVar3;
  char cVar4;
  byte bVar5;
  char cVar6;
  undefined2 uVar7;
  int unaff_EBP;
  float10 in_ST0;
  float10 in_ST1;
  
  if (*(char *)(param_2 + 0xe) == '\x05') {
    uVar7 = (undefined2)
            CONCAT31((uint3)((byte)((ushort)*(undefined2 *)(unaff_EBP + -0xa4) >> 8) & 0xfe | 2),
                     0x3f);
  }
  else {
    uVar7 = 0x133f;
  }
  *(undefined2 *)(unaff_EBP + -0xa2) = uVar7;
  fVar2 = (float10)0;
  *(int *)(unaff_EBP + -0x94) = param_2;
  *(ushort *)(unaff_EBP + -0xa0) =
       (ushort)NAN(in_ST0) << 8 | (ushort)(in_ST0 < fVar2) << 9 | (ushort)(in_ST0 != fVar2) << 10 |
       (ushort)(in_ST0 == fVar2) << 0xe;
  *(undefined1 *)(unaff_EBP + -0x90) = 0;
  fVar2 = (float10)0;
  *(ushort *)(unaff_EBP + -0xa0) =
       (ushort)NAN(in_ST1) << 8 | (ushort)(in_ST1 < fVar2) << 9 | (ushort)(in_ST1 != fVar2) << 10 |
       (ushort)(in_ST1 == fVar2) << 0xe;
  cVar6 = (char)(*(char *)(unaff_EBP + -0x9f) << 1) >> 1;
  bVar1 = cVar6 << 1;
  cVar4 = (char)(*(char *)(unaff_EBP + -0x9f) << 1) >> 1;
  bVar5 = cVar4 << 1 | cVar4 < '\0';
  iVar3 = (int)(char)((&DAT_0059d8be)[bVar5 & 0xf] |
                     (&DAT_0059d8be)[bVar1 & 0xf | cVar6 < '\0'] << 2);
  (*(code *)PTR_guard_check_icall_0056c37c)(CONCAT11(bVar1,bVar5) & 0x404,param_2,iVar3);
                    /* WARNING: Could not recover jumptable at 0x00563f4d. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(param_2 + iVar3 + 0x10))();
  return;
}

