
// block 0047e053
0047e053  mov        edi, edi
0047e055  push       ebp
0047e056  mov        ebp, esp
0047e058  mov        edx, dword ptr [ebp + 0xc]
0047e05b  mov        eax, ecx
0047e05d  or         dword ptr [eax], 0xffffffff
0047e060  lea        ecx, [eax + 0x2c]
0047e063  or         dword ptr [ecx], 0xffffffff
0047e066  mov        dword ptr [0x4b431c], edx
0047e06c  mov        dword ptr [0x4b4318], edx
0047e072  mov        edx, dword ptr [ebp + 8]
0047e075  test       edx, edx
0047e077  je         0x47e08c

// block 0047e079
0047e079  push       esi
0047e07a  mov        esi, dword ptr [ebp + 0x10]
0047e07d  mov        dword ptr [0x4b4324], esi
0047e083  mov        dword ptr [0x4b4320], edx
0047e089  pop        esi
0047e08a  jmp        0x47e09a

// block 0047e08c
0047e08c  and        dword ptr [0x4b4320], 0
0047e093  and        dword ptr [0x4b4324], 0

// block 0047e09a
0047e09a  mov        dword ptr [0x4b4310], ecx
0047e0a0  mov        ecx, dword ptr [ebp + 0x18]
0047e0a3  mov        dword ptr [0x4b4328], ecx
0047e0a9  mov        ecx, dword ptr [ebp + 0x14]
0047e0ac  mov        dword ptr [0x4b430c], eax
0047e0b1  mov        dword ptr [0x4b432c], ecx
0047e0b7  mov        byte ptr [0x4b4330], 0
0047e0be  pop        ebp
0047e0bf  ret        0x14
