
// block 004829c6
004829c6  mov        edi, edi
004829c8  push       ebp
004829c9  mov        ebp, esp
004829cb  mov        eax, dword ptr [ebp + 8]
004829ce  mov        ecx, dword ptr [0x4adb9c]
004829d4  push       esi

// block 004829d5
004829d5  cmp        dword ptr [eax + 4], edx
004829d8  je         0x4829e9

// block 004829da
004829da  mov        esi, ecx
004829dc  imul       esi, esi, 0xc
004829df  add        esi, dword ptr [ebp + 8]
004829e2  add        eax, 0xc
004829e5  cmp        eax, esi
004829e7  jb         0x4829d5

// block 004829e9
004829e9  imul       ecx, ecx, 0xc
004829ec  add        ecx, dword ptr [ebp + 8]
004829ef  pop        esi
004829f0  cmp        eax, ecx
004829f2  jae        0x4829f9

// block 004829f4
004829f4  cmp        dword ptr [eax + 4], edx
004829f7  je         0x4829fb

// block 004829f9
004829f9  xor        eax, eax

// block 004829fb
004829fb  pop        ebp
004829fc  ret        
