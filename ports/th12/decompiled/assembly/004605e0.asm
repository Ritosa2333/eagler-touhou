
// block 004605e0
004605e0  mov        eax, dword ptr [esi]
004605e2  test       eax, eax
004605e4  je         0x4605f4

// block 004605e6
004605e6  mov        ecx, dword ptr [eax]
004605e8  mov        edx, dword ptr [ecx + 8]
004605eb  push       eax
004605ec  call       edx

// block 004605ee
004605ee  mov        dword ptr [esi], 0

// block 004605f4
004605f4  mov        eax, dword ptr [esi + 4]
004605f7  test       eax, eax
004605f9  je         0x46060b

// block 004605fb
004605fb  push       eax
004605fc  call       0x46c941

// block 00460601
00460601  add        esp, 4
00460604  mov        dword ptr [esi + 4], 0

// block 0046060b
0046060b  ret        
