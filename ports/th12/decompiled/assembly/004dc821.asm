
// block 004dc821
004dc821  push       ecx
004dc822  push       ebx
004dc823  push       esi
004dc824  mov        esi, ecx
004dc826  push       edi
004dc827  mov        eax, dword ptr [esi]
004dc829  cmp        dword ptr [eax + 4], 8
004dc82d  jb         0x4dc85f

// block 004dc82f
004dc82f  mov        ecx, dword ptr [eax]
004dc831  mov        dl, byte ptr [ecx]
004dc833  inc        ecx
004dc834  mov        byte ptr [esp + 0xc], dl
004dc838  mov        dword ptr [eax], ecx
004dc83a  mov        ecx, dword ptr [eax + 8]
004dc83d  mov        edx, dword ptr [esp + 0xc]
004dc841  shl        ecx, 8
004dc844  and        edx, 0xff
004dc84a  or         ecx, edx
004dc84c  mov        edx, dword ptr [eax + 4]
004dc84f  add        edx, -8
004dc852  mov        dword ptr [eax + 8], ecx
004dc855  mov        ecx, edx
004dc857  mov        dword ptr [eax + 4], edx
004dc85a  cmp        ecx, 8
004dc85d  jae        0x4dc82f

// block 004dc85f
004dc85f  mov        edx, dword ptr [eax + 4]
004dc862  mov        eax, dword ptr [eax + 8]
004dc865  mov        ecx, 8
004dc86a  sub        ecx, edx
004dc86c  shr        eax, cl
004dc86e  mov        ecx, dword ptr [esi + 0x24]
004dc871  and        eax, 0xfffe00
004dc876  cmp        eax, ecx
004dc878  jae        0x4dc88e

// block 004dc87a
004dc87a  mov        edx, dword ptr [esi + 0x8c]
004dc880  mov        ecx, eax
004dc882  shr        ecx, 0x10
004dc885  xor        ebx, ebx
004dc887  mov        bl, byte ptr [ecx + edx]
004dc88a  mov        edx, ebx
004dc88c  jmp        0x4dc8c9

// block 004dc88e
004dc88e  cmp        eax, dword ptr [esi + 0x2c]
004dc891  jae        0x4dc89d

// block 004dc893
004dc893  cmp        eax, dword ptr [esi + 0x28]
004dc896  sbb        edx, edx
004dc898  add        edx, 0xa
004dc89b  jmp        0x4dc8c9

// block 004dc89d
004dc89d  cmp        eax, dword ptr [esi + 0x30]
004dc8a0  jae        0x4dc8a9

// block 004dc8a2
004dc8a2  mov        edx, 0xb
004dc8a7  jmp        0x4dc8c9

// block 004dc8a9
004dc8a9  cmp        eax, dword ptr [esi + 0x34]
004dc8ac  jae        0x4dc8b5

// block 004dc8ae
004dc8ae  mov        edx, 0xc
004dc8b3  jmp        0x4dc8c9

// block 004dc8b5
004dc8b5  cmp        eax, dword ptr [esi + 0x38]
004dc8b8  jae        0x4dc8c1

// block 004dc8ba
004dc8ba  mov        edx, 0xd
004dc8bf  jmp        0x4dc8c9

// block 004dc8c1
004dc8c1  cmp        eax, dword ptr [esi + 0x3c]
004dc8c4  sbb        edx, edx
004dc8c6  add        edx, 0xf

// block 004dc8c9
004dc8c9  mov        ecx, dword ptr [esi]
004dc8cb  mov        edi, dword ptr [ecx + 4]
004dc8ce  add        edi, edx
004dc8d0  mov        dword ptr [ecx + 4], edi
004dc8d3  mov        ebx, dword ptr [esi + edx*4]
004dc8d6  mov        ecx, 0x18
004dc8db  sub        eax, ebx
004dc8dd  sub        ecx, edx
004dc8df  pop        edi
004dc8e0  shr        eax, cl
004dc8e2  mov        ecx, dword ptr [esi + edx*4 + 0x44]
004dc8e6  add        eax, ecx
004dc8e8  mov        ecx, dword ptr [esi + 0x88]
004dc8ee  pop        esi
004dc8ef  pop        ebx
004dc8f0  mov        eax, dword ptr [ecx + eax*4]
004dc8f3  pop        ecx
004dc8f4  ret        
