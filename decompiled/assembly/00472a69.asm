
// block 00472a69
00472a69  push       8
00472a6b  push       0x4aabc0
00472a70  call       0x46fcec

// block 00472a75
00472a75  push       0xe
00472a77  call       0x46ec9a

// block 00472a7c
00472a7c  pop        ecx
00472a7d  and        dword ptr [ebp - 4], 0
00472a81  mov        eax, dword ptr [ebp + 8]
00472a84  mov        esi, dword ptr [eax + 4]

// block 00472a87
00472a87  test       esi, esi
00472a89  je         0x472aa1

// block 00472a8b
00472a8b  mov        edi, dword ptr [esi + 4]
00472a8e  push       dword ptr [esi]
00472a90  call       0x46c941

// block 00472a95
00472a95  push       esi
00472a96  call       0x46c941

// block 00472a9b
00472a9b  pop        ecx
00472a9c  pop        ecx
00472a9d  mov        esi, edi
00472a9f  jmp        0x472a87

// block 00472aa1
00472aa1  mov        dword ptr [ebp - 4], 0xfffffffe
00472aa8  call       0x472ab3

// block 00472aad
00472aad  call       0x46fd31

// block 00472ab2
00472ab2  ret        
