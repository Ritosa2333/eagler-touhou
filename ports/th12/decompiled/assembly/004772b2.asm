
// block 004772b2
004772b2  mov        edi, edi
004772b4  push       ebp
004772b5  mov        ebp, esp
004772b7  mov        eax, dword ptr [ebp + 8]
004772ba  push       esi
004772bb  xor        esi, esi
004772bd  cmp        eax, esi
004772bf  jne        0x4772de

// block 004772c1
004772c1  call       0x46e96f

// block 004772c6
004772c6  push       esi
004772c7  push       esi
004772c8  push       esi
004772c9  push       esi
004772ca  push       esi
004772cb  mov        dword ptr [eax], 0x16
004772d1  call       0x470f2d

// block 004772d6
004772d6  add        esp, 0x14
004772d9  push       0x16
004772db  pop        eax
004772dc  jmp        0x4772e8

// block 004772de
004772de  mov        ecx, dword ptr [0x4adafc]
004772e4  mov        dword ptr [eax], ecx
004772e6  xor        eax, eax

// block 004772e8
004772e8  pop        esi
004772e9  pop        ebp
004772ea  ret        
