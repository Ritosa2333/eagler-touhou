
// block 00472d44
00472d44  mov        edi, edi
00472d46  push       ebp
00472d47  mov        ebp, esp
00472d49  cmp        dword ptr [0x49d574], 0
00472d50  je         0x472d6b

// block 00472d52
00472d52  push       0x49d574
00472d57  call       0x477a40

// block 00472d5c
00472d5c  pop        ecx
00472d5d  test       eax, eax
00472d5f  je         0x472d6b

// block 00472d61
00472d61  push       dword ptr [ebp + 8]
00472d64  call       dword ptr [0x49d574]

// block 00472d6a
00472d6a  pop        ecx

// block 00472d6b
00472d6b  call       0x47c20c

// block 00472d70
00472d70  push       0x498354
00472d75  push       0x498338
00472d7a  call       0x472ca8

// block 00472d7f
00472d7f  pop        ecx
00472d80  pop        ecx
00472d81  test       eax, eax
00472d83  jne        0x472dc7

// block 00472d85
00472d85  push       0x47b343
00472d8a  call       0x46da32

// block 00472d8f
00472d8f  mov        eax, 0x498304
00472d94  mov        dword ptr [esp], 0x498334
00472d9b  call       0x472c8b

// block 00472da0
00472da0  cmp        dword ptr [0x4d6438], 0
00472da7  pop        ecx
00472da8  je         0x472dc5

// block 00472daa
00472daa  push       0x4d6438
00472daf  call       0x477a40

// block 00472db4
00472db4  pop        ecx
00472db5  test       eax, eax
00472db7  je         0x472dc5

// block 00472db9
00472db9  push       0
00472dbb  push       2
00472dbd  push       0
00472dbf  call       dword ptr [0x4d6438]

// block 00472dc5
00472dc5  xor        eax, eax

// block 00472dc7
00472dc7  pop        ebp
00472dc8  ret        
