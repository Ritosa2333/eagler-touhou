
// block 0048ad03
0048ad03  mov        edi, edi
0048ad05  push       ebp
0048ad06  mov        ebp, esp
0048ad08  mov        eax, dword ptr [ebp + 8]
0048ad0b  mov        ecx, dword ptr [eax]
0048ad0d  push       ebx
0048ad0e  push       esi
0048ad0f  push       edi
0048ad10  mov        edi, dword ptr [ebp + 0xc]
0048ad13  mov        edx, dword ptr [edi]
0048ad15  lea        esi, [ecx + edx]
0048ad18  xor        ebx, ebx
0048ad1a  cmp        esi, ecx
0048ad1c  jb         0x48ad22

// block 0048ad1e
0048ad1e  cmp        esi, edx
0048ad20  jae        0x48ad25

// block 0048ad22
0048ad22  xor        ebx, ebx
0048ad24  inc        ebx

// block 0048ad25
0048ad25  mov        dword ptr [eax], esi
0048ad27  test       ebx, ebx
0048ad29  je         0x48ad49

// block 0048ad2b
0048ad2b  mov        ecx, dword ptr [eax + 4]
0048ad2e  lea        edx, [ecx + 1]
0048ad31  xor        esi, esi
0048ad33  cmp        edx, ecx
0048ad35  jb         0x48ad3c

// block 0048ad37
0048ad37  cmp        edx, 1
0048ad3a  jae        0x48ad3f

// block 0048ad3c
0048ad3c  xor        esi, esi
0048ad3e  inc        esi

// block 0048ad3f
0048ad3f  mov        dword ptr [eax + 4], edx
0048ad42  test       esi, esi
0048ad44  je         0x48ad49

// block 0048ad46
0048ad46  inc        dword ptr [eax + 8]

// block 0048ad49
0048ad49  mov        ecx, dword ptr [eax + 4]
0048ad4c  mov        edx, dword ptr [edi + 4]
0048ad4f  lea        esi, [ecx + edx]
0048ad52  xor        ebx, ebx
0048ad54  cmp        esi, ecx
0048ad56  jb         0x48ad5c

// block 0048ad58
0048ad58  cmp        esi, edx
0048ad5a  jae        0x48ad5f

// block 0048ad5c
0048ad5c  xor        ebx, ebx
0048ad5e  inc        ebx

// block 0048ad5f
0048ad5f  mov        dword ptr [eax + 4], esi
0048ad62  test       ebx, ebx
0048ad64  je         0x48ad69

// block 0048ad66
0048ad66  inc        dword ptr [eax + 8]

// block 0048ad69
0048ad69  mov        ecx, dword ptr [edi + 8]
0048ad6c  add        dword ptr [eax + 8], ecx
0048ad6f  pop        edi
0048ad70  pop        esi
0048ad71  pop        ebx
0048ad72  pop        ebp
0048ad73  ret        
