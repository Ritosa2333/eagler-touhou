
// block 0040e413
0040e413  mov        ecx, dword ptr [0x4b43dc]
0040e419  mov        edx, dword ptr [ecx + 0x48]
0040e41c  push       ebp
0040e41d  push       0x11
0040e41f  lea        eax, [esp + 0x24]
0040e423  push       eax
0040e424  push       edx
0040e425  mov        eax, 0x17
0040e42a  xor        ecx, ecx
0040e42c  call       0x4615a0

// block 0040e431
0040e431  mov        eax, dword ptr [esp + 0x1c]
0040e435  mov        edx, dword ptr [0x4b43dc]
0040e43b  push       ebp
0040e43c  push       0x19
0040e43e  lea        ecx, [esp + 0x24]
0040e442  mov        dword ptr [edi + 0x10], eax
0040e445  mov        eax, dword ptr [edx + 0x48]
0040e448  push       ecx
0040e449  push       eax
0040e44a  jmp        0x40e5a4
