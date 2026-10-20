
// block 004929c7
004929c7  push       8
004929c9  push       0x4ab538
004929ce  call       0x46fcec

// block 004929d3
004929d3  mov        eax, dword ptr [ebp + 0x10]
004929d6  test       dword ptr [eax], 0x80000000
004929dc  je         0x4929e3

// block 004929de
004929de  mov        ebx, dword ptr [ebp + 0xc]
004929e1  jmp        0x4929ed

// block 004929e3
004929e3  mov        ecx, dword ptr [eax + 8]
004929e6  mov        edx, dword ptr [ebp + 0xc]
004929e9  lea        ebx, [ecx + edx + 0xc]

// block 004929ed
004929ed  and        dword ptr [ebp - 4], 0
004929f1  mov        esi, dword ptr [ebp + 0x14]
004929f4  push       esi
004929f5  push       eax
004929f6  push       dword ptr [ebp + 0xc]
004929f9  mov        edi, dword ptr [ebp + 8]
004929fc  push       edi
004929fd  call       0x492848

// block 00492a02
00492a02  add        esp, 0x10
00492a05  dec        eax
00492a06  je         0x492a27

// block 00492a08
00492a08  dec        eax
00492a09  jne        0x492a3f

// block 00492a0b
00492a0b  push       1
00492a0d  lea        eax, [esi + 8]
00492a10  push       eax
00492a11  push       dword ptr [edi + 0x18]
00492a14  call       0x4921d6

// block 00492a19
00492a19  pop        ecx
00492a1a  pop        ecx
00492a1b  push       eax
00492a1c  push       dword ptr [esi + 0x18]
00492a1f  push       ebx
00492a20  call       0x491a1a

// block 00492a25
00492a25  jmp        0x492a3f

// block 00492a27
00492a27  lea        eax, [esi + 8]
00492a2a  push       eax
00492a2b  push       dword ptr [edi + 0x18]
00492a2e  call       0x4921d6

// block 00492a33
00492a33  pop        ecx
00492a34  pop        ecx
00492a35  push       eax
00492a36  push       dword ptr [esi + 0x18]
00492a39  push       ebx
00492a3a  call       0x491a13

// block 00492a3f
00492a3f  mov        dword ptr [ebp - 4], 0xfffffffe
00492a46  call       0x46fd31

// block 00492a4b
00492a4b  ret        
