
// block 0040e4f5
0040e4f5  mov        eax, dword ptr [0x4b43dc]
0040e4fa  mov        ecx, dword ptr [eax + 0x48]
0040e4fd  push       ebp
0040e4fe  push       0xd
0040e500  lea        edx, [esp + 0x24]
0040e504  push       edx
0040e505  push       ecx
0040e506  mov        eax, 0x17
0040e50b  xor        ecx, ecx
0040e50d  call       0x4615a0

// block 0040e512
0040e512  mov        edx, dword ptr [esp + 0x1c]
0040e516  mov        ecx, dword ptr [0x4b43dc]
0040e51c  push       ebp
0040e51d  push       0x14
0040e51f  lea        eax, [esp + 0x24]
0040e523  mov        dword ptr [edi + 0x10], edx
0040e526  mov        edx, dword ptr [ecx + 0x48]
0040e529  push       eax
0040e52a  push       edx
0040e52b  jmp        0x40e5a4
