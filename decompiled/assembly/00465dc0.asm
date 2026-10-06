
// block 00465dc0
00465dc0  push       esi
00465dc1  mov        esi, ecx
00465dc3  call       0x465f50

// block 00465dc8
00465dc8  test       byte ptr [esp + 8], 1
00465dcd  je         0x465dd8

// block 00465dcf
00465dcf  push       esi
00465dd0  call       0x46ca4f

// block 00465dd5
00465dd5  add        esp, 4

// block 00465dd8
00465dd8  mov        eax, esi
00465dda  pop        esi
00465ddb  ret        4
