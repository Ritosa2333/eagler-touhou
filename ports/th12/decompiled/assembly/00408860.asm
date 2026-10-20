
// block 00408860
00408860  push       ebp
00408861  mov        ebp, esp
00408863  and        esp, 0xfffffff8
00408866  sub        esp, 8
00408869  fld        dword ptr [ebp + 8]
0040886c  call       0x4937c0

// block 00408871
00408871  fstp       dword ptr [esp + 4]
00408875  fld        dword ptr [esp + 4]
00408879  mov        esp, ebp
0040887b  pop        ebp
0040887c  ret        4
