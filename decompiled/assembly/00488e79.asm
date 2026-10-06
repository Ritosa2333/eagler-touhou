
// block 00488e79
00488e79  mov        edi, edi
00488e7b  push       ebp
00488e7c  mov        ebp, esp
00488e7e  mov        eax, dword ptr [ebp + 0xc]
00488e81  push       ebx
00488e82  push       esi
00488e83  push       edi
00488e84  cdq        
00488e85  push       0x1f
00488e87  pop        ecx
00488e88  and        edx, ecx
00488e8a  add        eax, edx
00488e8c  mov        edx, dword ptr [ebp + 0xc]
00488e8f  sar        eax, 5
00488e92  and        edx, 0x8000001f
00488e98  jns        0x488e9f

// block 00488e9a
00488e9a  dec        edx
00488e9b  or         edx, 0xffffffe0
00488e9e  inc        edx

// block 00488e9f
00488e9f  mov        edi, dword ptr [ebp + 8]
00488ea2  sub        ecx, edx
00488ea4  xor        edx, edx
00488ea6  inc        edx
00488ea7  shl        edx, cl
00488ea9  mov        ecx, dword ptr [edi + eax*4]
00488eac  xor        ebx, ebx
00488eae  lea        esi, [ecx + edx]
00488eb1  cmp        esi, ecx
00488eb3  jb         0x488eb9

// block 00488eb5
00488eb5  cmp        esi, edx
00488eb7  jae        0x488ebc

// block 00488eb9
00488eb9  xor        ebx, ebx
00488ebb  inc        ebx

// block 00488ebc
00488ebc  mov        dword ptr [edi + eax*4], esi
00488ebf  jmp        0x488edc

// block 00488ec1
00488ec1  test       ebx, ebx
00488ec3  je         0x488edf

// block 00488ec5
00488ec5  mov        ecx, dword ptr [edi + eax*4]
00488ec8  lea        edx, [ecx + 1]
00488ecb  xor        ebx, ebx
00488ecd  cmp        edx, ecx
00488ecf  jb         0x488ed6

// block 00488ed1
00488ed1  cmp        edx, 1
00488ed4  jae        0x488ed9

// block 00488ed6
00488ed6  xor        ebx, ebx
00488ed8  inc        ebx

// block 00488ed9
00488ed9  mov        dword ptr [edi + eax*4], edx

// block 00488edc
00488edc  dec        eax
00488edd  jns        0x488ec1

// block 00488edf
00488edf  pop        edi
00488ee0  pop        esi
00488ee1  mov        eax, ebx
00488ee3  pop        ebx
00488ee4  pop        ebp
00488ee5  ret        
