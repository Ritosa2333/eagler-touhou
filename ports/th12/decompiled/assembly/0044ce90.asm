
// block 0044ce90
0044ce90  sub        esp, 0x54
0044ce93  push       esi
0044ce94  xor        esi, esi
0044ce96  mov        eax, 0x80000000
0044ce9b  mov        dword ptr [esp + 0x24], eax
0044ce9f  mov        dword ptr [esp + 0x28], eax
0044cea3  mov        dword ptr [esp + 0x2c], eax
0044cea7  mov        dword ptr [esp + 0x30], eax
0044ceab  mov        eax, 0xa
0044ceb0  lea        edx, [esp + 4]
0044ceb4  push       edx
0044ceb5  mov        edx, dword ptr [esp + 0x60]
0044ceb9  mov        word ptr [esp + 0x48], ax
0044cebe  lea        eax, [esp + 0x18]
0044cec2  push       eax
0044cec3  push       esi
0044cec4  push       esi
0044cec5  push       0x20
0044cec7  push       esi
0044cec8  xor        ecx, ecx
0044ceca  push       esi
0044cecb  mov        word ptr [esp + 0x62], cx
0044ced0  mov        ecx, dword ptr [esp + 0x7c]
0044ced4  push       esi
0044ced5  push       ecx
0044ced6  push       edx
0044ced7  mov        dword ptr [esp + 0x3c], 0x44
0044cedf  mov        dword ptr [esp + 0x40], esi
0044cee3  mov        dword ptr [esp + 0x44], esi
0044cee7  mov        dword ptr [esp + 0x48], esi
0044ceeb  mov        dword ptr [esp + 0x5c], 0x50
0044cef3  mov        dword ptr [esp + 0x60], 0x19
0044cefb  mov        dword ptr [esp + 0x64], esi
0044ceff  mov        dword ptr [esp + 0x68], esi
0044cf03  mov        dword ptr [esp + 0x70], esi
0044cf07  mov        dword ptr [esp + 0x74], esi
0044cf0b  mov        dword ptr [esp + 0x78], esi
0044cf0f  mov        dword ptr [esp + 0x7c], esi
0044cf13  call       dword ptr [0x4980b8]

// block 0044cf19
0044cf19  test       eax, eax
0044cf1b  jne        0x44cf27

// block 0044cf1d
0044cf1d  or         eax, 0xffffffff
0044cf20  pop        esi
0044cf21  add        esp, 0x54
0044cf24  ret        0xc

// block 0044cf27
0044cf27  cmp        dword ptr [esp + 0x64], esi
0044cf2b  je         0x44cf5e

// block 0044cf2d
0044cf2d  mov        esi, dword ptr [0x4980bc]
0044cf33  mov        dword ptr [esp + 0x60], 0x103
0044cf3b  jmp        0x44cf40

// block 0044cf40
0044cf40  mov        ecx, dword ptr [esp + 4]
0044cf44  lea        eax, [esp + 0x60]
0044cf48  push       eax
0044cf49  push       ecx
0044cf4a  call       esi

// block 0044cf4c
0044cf4c  mov        eax, dword ptr [esp + 0x60]
0044cf50  cmp        eax, 0x103
0044cf55  je         0x44cf40

// block 0044cf57
0044cf57  pop        esi
0044cf58  add        esp, 0x54
0044cf5b  ret        0xc

// block 0044cf5e
0044cf5e  xor        eax, eax
0044cf60  pop        esi
0044cf61  add        esp, 0x54
0044cf64  ret        0xc
