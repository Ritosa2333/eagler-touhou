
// block 0047e700
0047e700  mov        edi, edi
0047e702  push       ebp
0047e703  mov        ebp, esp
0047e705  sub        esp, 0x28
0047e708  mov        eax, dword ptr [0x4ad138]
0047e70d  xor        eax, ebp
0047e70f  mov        dword ptr [ebp - 4], eax
0047e712  mov        eax, dword ptr [ebp + 0xc]
0047e715  push       esi
0047e716  mov        esi, ecx
0047e718  xor        ecx, ecx
0047e71a  mov        byte ptr [esi + 4], cl
0047e71d  and        dword ptr [esi + 4], 0xffff00ff
0047e724  cmp        eax, ecx
0047e726  push       edi
0047e727  lea        edi, [ebp - 7]
0047e72a  mov        dword ptr [esi], ecx
0047e72c  mov        byte ptr [ebp - 7], cl
0047e72f  mov        byte ptr [ebp - 0x1d], cl
0047e732  jg         0x47e74b

// block 0047e734
0047e734  jl         0x47e73b

// block 0047e736
0047e736  cmp        dword ptr [ebp + 8], ecx
0047e739  jae        0x47e74b

// block 0047e73b
0047e73b  mov        edx, dword ptr [ebp + 8]
0047e73e  neg        edx
0047e740  adc        eax, ecx
0047e742  mov        byte ptr [ebp - 0x1d], 1
0047e746  neg        eax
0047e748  mov        dword ptr [ebp + 8], edx

// block 0047e74b
0047e74b  push       ebx
0047e74c  jmp        0x47e750

// block 0047e74e
0047e74e  xor        ecx, ecx

// block 0047e750
0047e750  push       ecx
0047e751  push       0xa
0047e753  push       eax
0047e754  push       dword ptr [ebp + 8]
0047e757  dec        edi
0047e758  call       0x47c890

// block 0047e75d
0047e75d  add        cl, 0x30
0047e760  mov        dword ptr [ebp + 8], eax
0047e763  mov        byte ptr [edi], cl
0047e765  mov        ecx, dword ptr [ebp + 8]
0047e768  mov        eax, edx
0047e76a  or         ecx, eax
0047e76c  mov        dword ptr [ebp - 0x24], ebx
0047e76f  jne        0x47e74e

// block 0047e771
0047e771  pop        ebx
0047e772  cmp        byte ptr [ebp - 0x1d], cl
0047e775  je         0x47e77b

// block 0047e777
0047e777  dec        edi
0047e778  mov        byte ptr [edi], 0x2d

// block 0047e77b
0047e77b  lea        eax, [ebp - 7]
0047e77e  sub        eax, edi
0047e780  push       eax
0047e781  push       edi
0047e782  mov        ecx, esi
0047e784  call       0x47e4f1

// block 0047e789
0047e789  mov        ecx, dword ptr [ebp - 4]
0047e78c  pop        edi
0047e78d  mov        eax, esi
0047e78f  xor        ecx, ebp
0047e791  pop        esi
0047e792  call       0x46c932

// block 0047e797
0047e797  leave      
0047e798  ret        8
