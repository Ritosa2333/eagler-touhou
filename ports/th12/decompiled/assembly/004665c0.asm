
// block 004665c0
004665c0  cmp        dword ptr [edi + 4], 0
004665c4  jne        0x4665cc

// block 004665c6
004665c6  mov        eax, 0x800401f0
004665cb  ret        

// block 004665cc
004665cc  push       ebx
004665cd  push       esi
004665ce  xor        ebx, ebx
004665d0  xor        esi, esi
004665d2  cmp        dword ptr [edi + 0x10], ebx
004665d5  jbe        0x4665ef

// block 004665d7
004665d7  mov        eax, dword ptr [edi + 4]
004665da  mov        eax, dword ptr [eax + esi*4]
004665dd  mov        ecx, dword ptr [eax]
004665df  mov        edx, dword ptr [ecx + 0x34]
004665e2  push       0
004665e4  push       eax
004665e5  call       edx

// block 004665e7
004665e7  inc        esi
004665e8  or         ebx, eax
004665ea  cmp        esi, dword ptr [edi + 0x10]
004665ed  jb         0x4665d7

// block 004665ef
004665ef  pop        esi
004665f0  mov        eax, ebx
004665f2  pop        ebx
004665f3  ret        
