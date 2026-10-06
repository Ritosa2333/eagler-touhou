
// block 0047f037
0047f037  mov        edi, edi
0047f039  push       ebp
0047f03a  mov        ebp, esp
0047f03c  sub        esp, 0x28
0047f03f  and        dword ptr [ebp - 4], 0xffff0000
0047f046  push       ebx
0047f047  push       edi
0047f048  mov        edi, dword ptr [0x4b4318]
0047f04e  xor        ebx, ebx
0047f050  inc        edi
0047f051  mov        dword ptr [0x4b4318], edi
0047f057  mov        cl, byte ptr [edi]
0047f059  movsx      eax, cl
0047f05c  mov        edx, eax
0047f05e  sub        edx, 0x41
0047f061  mov        dword ptr [ebp - 8], ebx
0047f064  je         0x47f183

// block 0047f06a
0047f06a  dec        edx
0047f06b  je         0x47f15d

// block 0047f071
0047f071  dec        edx
0047f072  je         0x47f155

// block 0047f078
0047f078  cmp        cl, bl
0047f07a  je         0x47f151

// block 0047f080
0047f080  mov        cl, byte ptr [edi + 1]
0047f083  cmp        cl, bl
0047f085  je         0x47f151

// block 0047f08b
0047f08b  cmp        dword ptr [ebp + 0x14], ebx
0047f08e  jne        0x47f162

// block 0047f094
0047f094  add        eax, -0x30
0047f097  shl        eax, 4
0047f09a  movsx      ecx, cl
0047f09d  inc        edi
0047f09e  push       esi
0047f09f  lea        esi, [eax + ecx - 0x30]
0047f0a3  inc        edi
0047f0a4  mov        dword ptr [0x4b4318], edi
0047f0aa  cmp        esi, 1
0047f0ad  jbe        0x47f0db

// block 0047f0af
0047f0af  push       0x2c
0047f0b1  lea        ecx, [ebp - 8]
0047f0b4  call       0x47e869

// block 0047f0b9
0047f0b9  push       ebx
0047f0ba  push       esi
0047f0bb  lea        ecx, [ebp - 0x10]
0047f0be  call       0x47e692

// block 0047f0c3
0047f0c3  push       eax
0047f0c4  lea        eax, [ebp - 0x18]
0047f0c7  push       eax
0047f0c8  lea        ecx, [ebp - 8]
0047f0cb  call       0x47e9ae

// block 0047f0d0
0047f0d0  mov        ecx, dword ptr [eax]
0047f0d2  mov        eax, dword ptr [eax + 4]
0047f0d5  mov        dword ptr [ebp - 8], ecx
0047f0d8  mov        dword ptr [ebp - 4], eax

// block 0047f0db
0047f0db  push       0x3e
0047f0dd  lea        eax, [ebp - 0x20]
0047f0e0  push       eax
0047f0e1  lea        ecx, [ebp - 8]
0047f0e4  call       0x47ec6e

// block 0047f0e9
0047f0e9  mov        ecx, dword ptr [eax]
0047f0eb  mov        dword ptr [ebp - 8], ecx
0047f0ee  mov        ecx, dword ptr [eax + 4]
0047f0f1  mov        eax, dword ptr [0x4b4318]
0047f0f6  cmp        byte ptr [eax], 0x24
0047f0f9  mov        dword ptr [ebp - 4], ecx
0047f0fc  pop        esi
0047f0fd  jne        0x47f107

// block 0047f0ff
0047f0ff  inc        dword ptr [0x4b4318]
0047f105  jmp        0x47f120

// block 0047f107
0047f107  push       0x5e
0047f109  lea        eax, [ebp - 0x28]
0047f10c  push       eax
0047f10d  lea        ecx, [ebp - 8]
0047f110  call       0x47ec6e

// block 0047f115
0047f115  mov        ecx, dword ptr [eax]
0047f117  mov        dword ptr [ebp - 8], ecx
0047f11a  mov        ecx, dword ptr [eax + 4]
0047f11d  mov        dword ptr [ebp - 4], ecx

// block 0047f120
0047f120  mov        eax, dword ptr [0x4b4318]
0047f125  cmp        byte ptr [eax], bl
0047f127  je         0x47f131

// block 0047f129
0047f129  inc        dword ptr [0x4b4318]
0047f12f  jmp        0x47f13e

// block 0047f131
0047f131  push       1
0047f133  lea        ecx, [ebp - 8]
0047f136  call       0x47e4af

// block 0047f13b
0047f13b  mov        ecx, dword ptr [ebp - 4]

// block 0047f13e
0047f13e  mov        eax, dword ptr [ebp + 8]
0047f141  mov        edx, dword ptr [ebp - 8]
0047f144  or         ecx, 0x4000
0047f14a  mov        dword ptr [eax], edx
0047f14c  mov        dword ptr [eax + 4], ecx
0047f14f  jmp        0x47f1b0

// block 0047f151
0047f151  push       1
0047f153  jmp        0x47f164

// block 0047f155
0047f155  mov        eax, dword ptr [ebp + 0xc]
0047f158  mov        byte ptr [eax], 0x25
0047f15b  jmp        0x47f19b

// block 0047f15d
0047f15d  cmp        dword ptr [ebp + 0x14], ebx
0047f160  je         0x47f171

// block 0047f162
0047f162  push       2

// block 0047f164
0047f164  mov        ecx, dword ptr [ebp + 8]
0047f167  call       0x47e1ce

// block 0047f16c
0047f16c  mov        eax, dword ptr [ebp + 8]
0047f16f  jmp        0x47f1b0

// block 0047f171
0047f171  mov        eax, dword ptr [ebp + 0x10]
0047f174  push       0x3e
0047f176  lea        ecx, [ebp - 8]
0047f179  mov        byte ptr [eax], 1
0047f17c  call       0x47e869

// block 0047f181
0047f181  jmp        0x47f19b

// block 0047f183
0047f183  cmp        dword ptr [ebp + 0x14], ebx
0047f186  jne        0x47f19b

// block 0047f188
0047f188  mov        eax, dword ptr [ebp + 0xc]
0047f18b  cmp        byte ptr [eax], 0x26
0047f18e  setne      cl
0047f191  dec        cl
0047f193  and        cl, 0xc7
0047f196  add        cl, 0x5e
0047f199  mov        byte ptr [eax], cl

// block 0047f19b
0047f19b  mov        eax, dword ptr [ebp + 8]
0047f19e  inc        dword ptr [0x4b4318]
0047f1a4  mov        byte ptr [eax + 4], bl
0047f1a7  and        dword ptr [eax + 4], 0xffff00ff
0047f1ae  mov        dword ptr [eax], ebx

// block 0047f1b0
0047f1b0  pop        edi
0047f1b1  pop        ebx
0047f1b2  leave      
0047f1b3  ret        
