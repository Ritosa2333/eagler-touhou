
// block 0044ef10
0044ef10  push       ebp
0044ef11  mov        ebp, esp
0044ef13  push       -1
0044ef15  push       0x496eb0
0044ef1a  mov        eax, dword ptr fs:[0]
0044ef20  push       eax
0044ef21  sub        esp, 0xc
0044ef24  push       ebx
0044ef25  push       esi
0044ef26  push       edi
0044ef27  mov        eax, dword ptr [0x4ad138]
0044ef2c  xor        eax, ebp
0044ef2e  push       eax
0044ef2f  lea        eax, [ebp - 0xc]
0044ef32  mov        dword ptr fs:[0], eax
0044ef38  mov        dword ptr [ebp - 0x10], esp
0044ef3b  mov        edi, ecx
0044ef3d  mov        dword ptr [ebp - 0x14], edi
0044ef40  mov        eax, dword ptr [ebp + 8]
0044ef43  mov        esi, eax
0044ef45  or         esi, 0xf
0044ef48  cmp        esi, -2
0044ef4b  jbe        0x44ef51

// block 0044ef4d
0044ef4d  mov        esi, eax
0044ef4f  jmp        0x44ef73

// block 0044ef51
0044ef51  mov        ebx, dword ptr [edi + 0x18]
0044ef54  mov        eax, 0xaaaaaaab
0044ef59  mul        esi
0044ef5b  mov        ecx, ebx
0044ef5d  shr        ecx, 1
0044ef5f  shr        edx, 1
0044ef61  cmp        edx, ecx
0044ef63  jae        0x44ef73

// block 0044ef65
0044ef65  mov        eax, 0xfffffffe
0044ef6a  sub        eax, ecx
0044ef6c  cmp        ebx, eax
0044ef6e  ja         0x44ef73

// block 0044ef70
0044ef70  lea        esi, [ecx + ebx]

// block 0044ef73
0044ef73  lea        ecx, [esi + 1]
0044ef76  push       ecx
0044ef77  mov        ecx, edi
0044ef79  mov        dword ptr [ebp - 4], 0
0044ef80  call       0x44f0a0

// block 0044ef85
0044ef85  mov        dword ptr [ebp + 8], eax
0044ef88  mov        dword ptr [ebp - 4], 0xffffffff
0044ef8f  jmp        0x44efbe

// block 0044efbe
0044efbe  mov        ebx, dword ptr [ebp + 0xc]
0044efc1  test       ebx, ebx
0044efc3  jbe        0x44efe5

// block 0044efc5
0044efc5  cmp        dword ptr [edi + 0x18], 0x10
0044efc9  jb         0x44efd0

// block 0044efcb
0044efcb  mov        eax, dword ptr [edi + 4]
0044efce  jmp        0x44efd3

// block 0044efd0
0044efd0  lea        eax, [edi + 4]

// block 0044efd3
0044efd3  push       ebx
0044efd4  push       eax
0044efd5  mov        eax, dword ptr [ebp + 8]
0044efd8  lea        edx, [esi + 1]
0044efdb  push       edx
0044efdc  push       eax
0044efdd  call       0x46e210

// block 0044efe2
0044efe2  add        esp, 0x10

// block 0044efe5
0044efe5  cmp        dword ptr [edi + 0x18], 0x10
0044efe9  jb         0x44eff7

// block 0044efeb
0044efeb  mov        ecx, dword ptr [edi + 4]
0044efee  push       ecx
0044efef  call       0x46ca4f

// block 0044eff4
0044eff4  add        esp, 4

// block 0044eff7
0044eff7  mov        ecx, dword ptr [ebp + 8]
0044effa  lea        eax, [edi + 4]
0044effd  mov        byte ptr [eax], 0
0044f000  mov        dword ptr [eax], ecx
0044f002  mov        dword ptr [edi + 0x18], esi
0044f005  mov        dword ptr [edi + 0x14], ebx
0044f008  cmp        esi, 0x10
0044f00b  jb         0x44f00f

// block 0044f00d
0044f00d  mov        eax, ecx

// block 0044f00f
0044f00f  mov        byte ptr [eax + ebx], 0
0044f013  mov        ecx, dword ptr [ebp - 0xc]
0044f016  mov        dword ptr fs:[0], ecx
0044f01d  pop        ecx
0044f01e  pop        edi
0044f01f  pop        esi
0044f020  pop        ebx
0044f021  mov        esp, ebp
0044f023  pop        ebp
0044f024  ret        8
