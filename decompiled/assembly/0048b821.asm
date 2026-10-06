
// block 0048b821
0048b821  mov        edi, edi
0048b823  push       ebp
0048b824  mov        ebp, esp
0048b826  mov        ecx, dword ptr [ebp + 0xc]
0048b829  push       ebx
0048b82a  xor        ebx, ebx
0048b82c  cmp        ecx, ebx
0048b82e  jbe        0x48b858

// block 0048b830
0048b830  push       -0x20
0048b832  xor        edx, edx
0048b834  pop        eax
0048b835  div        ecx
0048b837  cmp        eax, dword ptr [ebp + 0x10]
0048b83a  jae        0x48b858

// block 0048b83c
0048b83c  call       0x46e96f

// block 0048b841
0048b841  push       ebx
0048b842  push       ebx
0048b843  push       ebx
0048b844  push       ebx
0048b845  push       ebx
0048b846  mov        dword ptr [eax], 0xc
0048b84c  call       0x470f2d

// block 0048b851
0048b851  add        esp, 0x14
0048b854  xor        eax, eax
0048b856  jmp        0x48b899

// block 0048b858
0048b858  imul       ecx, dword ptr [ebp + 0x10]
0048b85c  push       esi
0048b85d  push       edi
0048b85e  mov        esi, ecx
0048b860  cmp        dword ptr [ebp + 8], ebx
0048b863  je         0x48b870

// block 0048b865
0048b865  push       dword ptr [ebp + 8]
0048b868  call       0x4778ff

// block 0048b86d
0048b86d  pop        ecx
0048b86e  mov        ebx, eax

// block 0048b870
0048b870  push       esi
0048b871  push       dword ptr [ebp + 8]
0048b874  call       0x48b606

// block 0048b879
0048b879  mov        edi, eax
0048b87b  pop        ecx
0048b87c  pop        ecx
0048b87d  test       edi, edi
0048b87f  je         0x48b895

// block 0048b881
0048b881  cmp        ebx, esi
0048b883  jae        0x48b895

// block 0048b885
0048b885  sub        esi, ebx
0048b887  push       esi
0048b888  push       0
0048b88a  add        ebx, edi
0048b88c  push       ebx
0048b88d  call       0x477420

// block 0048b892
0048b892  add        esp, 0xc

// block 0048b895
0048b895  mov        eax, edi
0048b897  pop        edi
0048b898  pop        esi

// block 0048b899
0048b899  pop        ebx
0048b89a  pop        ebp
0048b89b  ret        
