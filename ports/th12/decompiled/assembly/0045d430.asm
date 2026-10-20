
// block 0045d430
0045d430  push       ecx
0045d431  fld        dword ptr [esp + 0x14]
0045d435  push       ebp
0045d436  fstp       dword ptr [esp + 4]
0045d43a  push       esi
0045d43b  mov        esi, dword ptr [edi + 0x8856b0]
0045d441  fld        dword ptr [esp + 0x10]
0045d445  fstp       dword ptr [esi]
0045d447  mov        ebp, dword ptr [esp + 0x20]
0045d44b  fld        dword ptr [esp + 0x14]
0045d44f  mov        dword ptr [esi + 0x10], eax
0045d452  fstp       dword ptr [esi + 4]
0045d455  lea        eax, [ebp + 1]
0045d458  fldz       
0045d45a  add        esi, 0x14
0045d45d  fstp       dword ptr [esi - 0xc]
0045d460  fld1       
0045d462  fstp       dword ptr [esi - 8]
0045d465  fild       dword ptr [esp + 0x20]
0045d469  fdivr      qword ptr [0x4a3cd0]
0045d46f  fstp       dword ptr [esp + 0x1c]
0045d473  test       eax, eax
0045d475  jle        0x45d4e1

// block 0045d477
0045d477  push       ebx
0045d478  mov        ebx, eax
0045d47a  lea        ebx, [ebx]

// block 0045d480
0045d480  fld        dword ptr [esp + 0x1c]
0045d484  sub        esp, 8
0045d487  fstp       dword ptr [esp + 4]
0045d48b  mov        ecx, esi
0045d48d  fld        dword ptr [esp + 0x14]
0045d491  fstp       dword ptr [esp]
0045d494  call       0x45d890

// block 0045d499
0045d499  fld        dword ptr [esi]
0045d49b  mov        ecx, dword ptr [esp + 0x28]
0045d49f  fadd       dword ptr [esp + 0x14]
0045d4a3  sub        esp, 8
0045d4a6  mov        dword ptr [esi + 0x10], ecx
0045d4a9  add        esi, 0x14
0045d4ac  fstp       dword ptr [esi - 0x14]
0045d4af  fld        dword ptr [esp + 0x20]
0045d4b3  fadd       dword ptr [esi - 0x10]
0045d4b6  fstp       dword ptr [esi - 0x10]
0045d4b9  fldz       
0045d4bb  fstp       dword ptr [esi - 0xc]
0045d4be  fld1       
0045d4c0  fstp       dword ptr [esi - 8]
0045d4c3  fld        dword ptr [esp + 0x28]
0045d4c7  fstp       dword ptr [esp + 4]
0045d4cb  fld        dword ptr [esp + 0x14]
0045d4cf  fstp       dword ptr [esp]
0045d4d2  call       0x464640

// block 0045d4d7
0045d4d7  fstp       dword ptr [esp + 0xc]
0045d4db  sub        ebx, 1
0045d4de  jne        0x45d480

// block 0045d4e0
0045d4e0  pop        ebx

// block 0045d4e1
0045d4e1  mov        esi, dword ptr [0x4ce8cc]
0045d4e7  call       0x45a3c0

// block 0045d4ec
0045d4ec  mov        eax, dword ptr [0x4ce8f0]
0045d4f1  mov        edx, dword ptr [eax]
0045d4f3  push       0
0045d4f5  push       0xe
0045d4f7  push       eax
0045d4f8  mov        eax, dword ptr [edx + 0xe4]
0045d4fe  call       eax

// block 0045d500
0045d500  mov        ecx, dword ptr [0x4ce8cc]
0045d506  cmp        byte ptr [ecx + 0x4b5647], 0
0045d50d  je         0x45d547

// block 0045d50f
0045d50f  mov        eax, dword ptr [0x4ce8f0]
0045d514  mov        edx, dword ptr [eax]
0045d516  push       3
0045d518  push       4
0045d51a  push       0
0045d51c  push       eax
0045d51d  mov        eax, dword ptr [edx + 0x10c]
0045d523  call       eax

// block 0045d525
0045d525  mov        eax, dword ptr [0x4ce8f0]
0045d52a  mov        ecx, dword ptr [eax]
0045d52c  mov        edx, dword ptr [ecx + 0x10c]
0045d532  push       3
0045d534  push       1
0045d536  push       0
0045d538  push       eax
0045d539  call       edx

// block 0045d53b
0045d53b  mov        eax, dword ptr [0x4ce8cc]
0045d540  mov        byte ptr [eax + 0x4b5647], 0

// block 0045d547
0045d547  cmp        byte ptr [edi + 0x4b5642], 1
0045d54e  je         0x45d583

// block 0045d550
0045d550  mov        eax, dword ptr [0x4ce8f0]
0045d555  mov        ecx, dword ptr [eax]
0045d557  mov        edx, dword ptr [ecx + 0x10c]
0045d55d  push       0
0045d55f  push       6
0045d561  push       0
0045d563  push       eax
0045d564  call       edx

// block 0045d566
0045d566  mov        eax, dword ptr [0x4ce8f0]
0045d56b  mov        ecx, dword ptr [eax]
0045d56d  mov        edx, dword ptr [ecx + 0x10c]
0045d573  push       0
0045d575  push       3
0045d577  push       0
0045d579  push       eax
0045d57a  call       edx

// block 0045d57c
0045d57c  mov        byte ptr [edi + 0x4b5642], 1

// block 0045d583
0045d583  mov        eax, dword ptr [0x4ce8f0]
0045d588  mov        ecx, dword ptr [eax]
0045d58a  mov        edx, dword ptr [ecx + 0x164]
0045d590  push       0x44
0045d592  push       eax
0045d593  call       edx

// block 0045d595
0045d595  mov        edx, dword ptr [edi + 0x8856b0]
0045d59b  mov        eax, dword ptr [0x4ce8f0]
0045d5a0  mov        ecx, dword ptr [eax]
0045d5a2  push       0x14
0045d5a4  push       edx
0045d5a5  push       ebp
0045d5a6  push       6
0045d5a8  push       eax
0045d5a9  mov        eax, dword ptr [ecx + 0x14c]
0045d5af  call       eax

// block 0045d5b1
0045d5b1  inc        dword ptr [edi + 0xac]
0045d5b7  lea        ecx, [ebp + ebp*4 + 0xa]
0045d5bb  add        ecx, ecx
0045d5bd  add        ecx, ecx
0045d5bf  add        dword ptr [edi + 0x8856b0], ecx
0045d5c5  pop        esi
0045d5c6  xor        eax, eax
0045d5c8  pop        ebp
0045d5c9  pop        ecx
0045d5ca  ret        0x18
