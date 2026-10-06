
// block 00459640
00459640  push       ebx
00459641  fldz       
00459643  push       ebp
00459644  push       esi
00459645  mov        esi, dword ptr [esp + 0x10]
00459649  mov        dword ptr [eax + 0x268], esi
0045964f  xor        esi, esi
00459651  mov        dword ptr [eax + 0x23c], esi
00459657  mov        dword ptr [eax + 0x248], esi
0045965d  movzx      esi, byte ptr [esp + 0x14]
00459662  push       edi
00459663  xor        edi, edi
00459665  mov        dword ptr [eax + 0x240], edi
0045966b  mov        dword ptr [eax + 0x24c], edi
00459671  mov        dword ptr [eax + 0x26c], esi
00459677  xor        ebx, ebx
00459679  mov        dword ptr [eax + 0x244], ebx
0045967f  mov        dword ptr [eax + 0x250], ebx
00459685  movzx      edi, byte ptr [ecx + 2]
00459689  movzx      ebx, byte ptr [ecx + 1]
0045968d  movzx      ecx, byte ptr [ecx]
00459690  movzx      ebp, byte ptr [edx + 2]
00459694  movzx      esi, byte ptr [edx + 1]
00459698  movzx      edx, byte ptr [edx]
0045969b  mov        dword ptr [eax + 0x230], ecx
004596a1  mov        dword ptr [eax + 0x234], ebx
004596a7  mov        dword ptr [eax + 0x224], edx
004596ad  mov        dword ptr [eax + 0x238], edi
004596b3  pop        edi
004596b4  mov        dword ptr [eax + 0x228], esi
004596ba  mov        dword ptr [eax + 0x22c], ebp
004596c0  mov        ecx, dword ptr [eax + 0x264]
004596c6  pop        esi
004596c7  pop        ebp
004596c8  pop        ebx
004596c9  test       cl, 1
004596cc  jne        0x4596fb

// block 004596ce
004596ce  or         ecx, 1
004596d1  fst        dword ptr [eax + 0x25c]
004596d7  mov        dword ptr [eax + 0x258], 0
004596e1  mov        dword ptr [eax + 0x254], 0xfff0bdc1
004596eb  mov        dword ptr [eax + 0x260], 0x4b2ed0
004596f5  mov        dword ptr [eax + 0x264], ecx

// block 004596fb
004596fb  fstp       dword ptr [eax + 0x25c]
00459701  mov        dword ptr [eax + 0x258], 0
0045970b  mov        dword ptr [eax + 0x254], 0xffffffff
00459715  ret        8
