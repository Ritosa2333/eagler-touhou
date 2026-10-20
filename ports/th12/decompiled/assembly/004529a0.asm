
// block 004529a0
004529a0  push       esi
004529a1  push       0x44
004529a3  call       0x46c9ea

// block 004529a8
004529a8  mov        esi, eax
004529aa  add        esp, 4
004529ad  test       esi, esi
004529af  je         0x4529ef

// block 004529b1
004529b1  and        dword ptr [esi + 0x40], 0xfffffffe
004529b5  push       0x44
004529b7  push       0
004529b9  push       esi
004529ba  call       0x477420

// block 004529bf
004529bf  mov        eax, dword ptr [esp + 0x28]
004529c3  mov        ecx, dword ptr [esp + 0x24]
004529c7  mov        edx, dword ptr [esp + 0x20]
004529cb  or         dword ptr [esi], 2
004529ce  add        esp, 0xc
004529d1  push       eax
004529d2  mov        eax, dword ptr [esp + 0x14]
004529d6  push       ecx
004529d7  mov        ecx, dword ptr [esp + 0x14]
004529db  push       edx
004529dc  mov        edx, dword ptr [esp + 0x14]
004529e0  push       eax
004529e1  push       ecx
004529e2  push       edx
004529e3  push       esi
004529e4  call       0x452a60

// block 004529e9
004529e9  mov        eax, esi
004529eb  pop        esi
004529ec  ret        0x18

// block 004529ef
004529ef  xor        eax, eax
004529f1  pop        esi
004529f2  ret        0x18
