
// block 00483f19
00483f19  mov        edi, edi
00483f1b  push       ebp
00483f1c  mov        ebp, esp
00483f1e  push       esi
00483f1f  mov        esi, dword ptr [ebp + 8]
00483f22  test       esi, esi
00483f24  je         0x483f5b

// block 00483f26
00483f26  mov        eax, dword ptr [esi]
00483f28  cmp        eax, dword ptr [0x4adf68]
00483f2e  je         0x483f37

// block 00483f30
00483f30  push       eax
00483f31  call       0x46c941

// block 00483f36
00483f36  pop        ecx

// block 00483f37
00483f37  mov        eax, dword ptr [esi + 4]
00483f3a  cmp        eax, dword ptr [0x4adf6c]
00483f40  je         0x483f49

// block 00483f42
00483f42  push       eax
00483f43  call       0x46c941

// block 00483f48
00483f48  pop        ecx

// block 00483f49
00483f49  mov        esi, dword ptr [esi + 8]
00483f4c  cmp        esi, dword ptr [0x4adf70]
00483f52  je         0x483f5b

// block 00483f54
00483f54  push       esi
00483f55  call       0x46c941

// block 00483f5a
00483f5a  pop        ecx

// block 00483f5b
00483f5b  pop        esi
00483f5c  pop        ebp
00483f5d  ret        
