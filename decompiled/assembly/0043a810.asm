
// block 0043a810
0043a810  push       ecx
0043a811  push       esi
0043a812  mov        esi, edx
0043a814  cmp        dword ptr [esi + 0x48], 2
0043a818  je         0x43a930

// block 0043a81e
0043a81e  mov        ecx, dword ptr [0x4b43dc]
0043a824  push       ebx
0043a825  xor        ebx, ebx
0043a827  cmp        ecx, ebx
0043a829  jne        0x43a82e

// block 0043a82b
0043a82b  mov        dword ptr [esi + 0x54], ebx

// block 0043a82e
0043a82e  mov        eax, dword ptr [esi + 4]
0043a831  cmp        eax, 0x3e8
0043a836  jge        0x43a905

// block 0043a83c
0043a83c  cmp        eax, 5
0043a83f  jl         0x43a905

// block 0043a845
0043a845  cmp        ecx, ebx
0043a847  je         0x43a86d

// block 0043a849
0043a849  fld        dword ptr [0x4a3d88]
0043a84f  push       edi
0043a850  push       ecx
0043a851  lea        edi, [esi + 0x14]
0043a854  fstp       dword ptr [esp]
0043a857  call       0x41a9f0

// block 0043a85c
0043a85c  mov        dword ptr [esi + 0x54], eax
0043a85f  pop        edi
0043a860  cmp        eax, ebx
0043a862  je         0x43a86d

// block 0043a864
0043a864  mov        eax, dword ptr [eax + 0x27b4]
0043a86a  mov        dword ptr [esi + 0x64], eax

// block 0043a86d
0043a86d  mov        ecx, dword ptr [esi + 0x64]
0043a870  call       0x412500

// block 0043a875
0043a875  test       eax, eax
0043a877  jne        0x43a87f

// block 0043a879
0043a879  mov        dword ptr [esi + 0x54], ebx
0043a87c  mov        dword ptr [esi + 0x64], ebx

// block 0043a87f
0043a87f  mov        ecx, dword ptr [esi + 0x54]
0043a882  cmp        ecx, ebx
0043a884  je         0x43a905

// block 0043a886
0043a886  fld        dword ptr [ecx + 0x1078]
0043a88c  fld        dword ptr [esi + 0x18]
0043a88f  fsub       qword ptr [0x4a3d68]
0043a895  fcompp     
0043a897  fnstsw     ax
0043a899  test       ah, 0x41
0043a89c  jp         0x43a905

// block 0043a89e
0043a89e  fld        dword ptr [ecx + 0x1078]
0043a8a4  fld        dword ptr [esi + 0x18]
0043a8a7  fadd       qword ptr [0x4a3d70]
0043a8ad  fcompp     
0043a8af  fnstsw     ax
0043a8b1  test       ah, 1
0043a8b4  jne        0x43a905

// block 0043a8b6
0043a8b6  fldz       
0043a8b8  mov        ecx, dword ptr [esi + 0x4c]
0043a8bb  fst        dword ptr [esi + 0x2c]
0043a8be  push       ecx
0043a8bf  fst        dword ptr [esi + 0x20]
0043a8c2  mov        ebx, 2
0043a8c7  fstp       dword ptr [esi + 0x24]
0043a8ca  call       0x461970

// block 0043a8cf
0043a8cf  push       0x3e8
0043a8d4  mov        eax, esi
0043a8d6  call       0x4067e0

// block 0043a8db
0043a8db  mov        edx, dword ptr [esi + 0x54]
0043a8de  fld        dword ptr [edx + 0x1074]
0043a8e4  fld        dword ptr [esi + 0x14]
0043a8e7  fcompp     
0043a8e9  fnstsw     ax
0043a8eb  test       ah, 0x41
0043a8ee  jne        0x43a8f8

// block 0043a8f0
0043a8f0  fld        dword ptr [0x4a4098]
0043a8f6  jmp        0x43a8fa

// block 0043a8f8
0043a8f8  fldz       

// block 0043a8fa
0043a8fa  fstp       dword ptr [esp + 8]
0043a8fe  fld        dword ptr [esp + 8]
0043a902  fstp       dword ptr [esi + 0x70]

// block 0043a905
0043a905  cmp        dword ptr [esi + 4], 0x3ec
0043a90c  pop        ebx
0043a90d  jne        0x43a930

// block 0043a90f
0043a90f  fld        dword ptr [0x4a3e00]
0043a915  push       ecx
0043a916  fstp       dword ptr [esi + 0x2c]
0043a919  fld        dword ptr [esi + 0x70]
0043a91c  fstp       dword ptr [esp]
0043a91f  call       0x4646e0

// block 0043a924
0043a924  push       ecx
0043a925  fstp       dword ptr [esp]
0043a928  call       0x4646e0

// block 0043a92d
0043a92d  fstp       dword ptr [esi + 0x30]

// block 0043a930
0043a930  xor        eax, eax
0043a932  pop        esi
0043a933  pop        ecx
0043a934  ret        
