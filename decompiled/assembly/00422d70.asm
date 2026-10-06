
// block 00422d70
00422d70  push       ecx
00422d71  push       esi
00422d72  push       edi
00422d73  mov        edi, eax
00422d75  mov        eax, dword ptr [edi + 8]
00422d78  mov        ecx, dword ptr [edi + 0x90]
00422d7e  cmp        eax, ecx
00422d80  jl         0x422d88

// block 00422d82
00422d82  xor        eax, eax
00422d84  pop        edi
00422d85  pop        esi
00422d86  pop        ecx
00422d87  ret        

// block 00422d88
00422d88  add        eax, ebx
00422d8a  cmp        eax, ecx
00422d8c  mov        dword ptr [edi + 8], eax
00422d8f  jle        0x422da6

// block 00422d91
00422d91  mov        esi, dword ptr [0x4b43e4]
00422d97  push       0
00422d99  mov        eax, 2
00422d9e  mov        dword ptr [edi + 8], ecx
00422da1  call       0x420f90

// block 00422da6
00422da6  mov        ecx, dword ptr [edi + 8]
00422da9  mov        edi, dword ptr [edi + 0x94]
00422daf  mov        eax, ecx
00422db1  sub        eax, ebx
00422db3  cdq        
00422db4  idiv       edi
00422db6  mov        esi, eax
00422db8  mov        eax, ecx
00422dba  cdq        
00422dbb  idiv       edi
00422dbd  xor        ecx, ecx
00422dbf  pop        edi
00422dc0  cmp        esi, eax
00422dc2  setne      cl
00422dc5  pop        esi
00422dc6  mov        eax, ecx
00422dc8  pop        ecx
00422dc9  ret        
