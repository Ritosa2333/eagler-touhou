
// block 0048289f
0048289f  mov        edi, edi
004828a1  push       ebp
004828a2  mov        ebp, esp
004828a4  sub        esp, 0x18
004828a7  push       esi
004828a8  push       0
004828aa  push       8
004828ac  mov        ecx, 0x4b42f8
004828b1  call       0x47dc29

// block 004828b6
004828b6  test       eax, eax
004828b8  je         0x4828cc

// block 004828ba
004828ba  and        dword ptr [eax], 0
004828bd  mov        byte ptr [eax + 4], 0
004828c1  and        dword ptr [eax + 4], 0xffff00ff
004828c8  mov        esi, eax
004828ca  jmp        0x4828ce

// block 004828cc
004828cc  xor        esi, esi

// block 004828ce
004828ce  push       esi
004828cf  push       dword ptr [ebp + 8]
004828d2  call       0x4827df

// block 004828d7
004828d7  lea        eax, [ebp - 8]
004828da  push       eax
004828db  call       0x47e0c2

// block 004828e0
004828e0  add        esp, 0xc
004828e3  push       dword ptr [ebp + 0xc]
004828e6  lea        eax, [ebp - 0x10]
004828e9  push       eax
004828ea  push       0x20
004828ec  lea        eax, [ebp - 0x18]
004828ef  push       eax
004828f0  lea        ecx, [ebp - 8]
004828f3  call       0x47ec6e

// block 004828f8
004828f8  mov        ecx, eax
004828fa  call       0x47e9ae

// block 004828ff
004828ff  push       eax
00482900  mov        ecx, esi
00482902  call       0x47dde0

// block 00482907
00482907  mov        eax, dword ptr [ebp + 8]
0048290a  pop        esi
0048290b  leave      
0048290c  ret        
