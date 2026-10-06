
// block 00466680
00466680  push       esi
00466681  mov        esi, ecx
00466683  mov        dword ptr [esi], 0x4a3b24
00466689  call       0x465f50

// block 0046668e
0046668e  test       byte ptr [esp + 8], 1
00466693  je         0x46669e

// block 00466695
00466695  push       esi
00466696  call       0x46ca4f

// block 0046669b
0046669b  add        esp, 4

// block 0046669e
0046669e  mov        eax, esi
004666a0  pop        esi
004666a1  ret        4
