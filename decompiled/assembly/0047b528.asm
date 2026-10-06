
// block 0047b528
0047b528  push       ebp
0047b529  mov        ebp, dword ptr [eax + 0x18]
0047b52c  push       dword ptr [eax + 0xc]
0047b52f  push       dword ptr [eax + 0x10]
0047b532  push       dword ptr [eax + 0x14]
0047b535  call       0x47b478

// block 0047b53a
0047b53a  add        esp, 0xc
0047b53d  pop        ebp
0047b53e  mov        eax, dword ptr [esp + 8]
0047b542  mov        edx, dword ptr [esp + 0x10]
0047b546  mov        dword ptr [edx], eax
0047b548  mov        eax, 3
