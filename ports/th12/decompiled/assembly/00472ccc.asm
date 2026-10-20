
// block 00472ccc
00472ccc  mov        edi, edi
00472cce  push       ebp
00472ccf  mov        ebp, esp
00472cd1  mov        ecx, dword ptr [ebp + 8]
00472cd4  push       esi
00472cd5  xor        esi, esi
00472cd7  cmp        ecx, esi
00472cd9  jne        0x472cf8

// block 00472cdb
00472cdb  call       0x46e96f

// block 00472ce0
00472ce0  push       esi
00472ce1  push       esi
00472ce2  push       esi
00472ce3  push       esi
00472ce4  push       esi
00472ce5  mov        dword ptr [eax], 0x16
00472ceb  call       0x470f2d

// block 00472cf0
00472cf0  add        esp, 0x14
00472cf3  push       0x16
00472cf5  pop        eax
00472cf6  jmp        0x472d05

// block 00472cf8
00472cf8  mov        eax, dword ptr [0x4b3d94]
00472cfd  cmp        eax, esi
00472cff  je         0x472cdb

// block 00472d01
00472d01  mov        dword ptr [ecx], eax
00472d03  xor        eax, eax

// block 00472d05
00472d05  pop        esi
00472d06  pop        ebp
00472d07  ret        
