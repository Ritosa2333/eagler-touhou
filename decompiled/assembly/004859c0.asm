
// block 004859c0
004859c0  push       ebp
004859c1  mov        ebp, esp
004859c3  push       esi
004859c4  xor        eax, eax
004859c6  push       eax
004859c7  push       eax
004859c8  push       eax
004859c9  push       eax
004859ca  push       eax
004859cb  push       eax
004859cc  push       eax
004859cd  push       eax
004859ce  mov        edx, dword ptr [ebp + 0xc]
004859d1  lea        ecx, [ecx]

// block 004859d4
004859d4  mov        al, byte ptr [edx]
004859d6  or         al, al
004859d8  je         0x4859e3

// block 004859da
004859da  add        edx, 1
004859dd  bts        dword ptr [esp], eax
004859e1  jmp        0x4859d4

// block 004859e3
004859e3  mov        esi, dword ptr [ebp + 8]
004859e6  or         ecx, 0xffffffff
004859e9  lea        ecx, [ecx]

// block 004859ec
004859ec  add        ecx, 1
004859ef  mov        al, byte ptr [esi]
004859f1  or         al, al
004859f3  je         0x4859fe

// block 004859f5
004859f5  add        esi, 1
004859f8  bt         dword ptr [esp], eax
004859fc  jae        0x4859ec

// block 004859fe
004859fe  mov        eax, ecx
00485a00  add        esp, 0x20
00485a03  pop        esi
00485a04  leave      
00485a05  ret        
