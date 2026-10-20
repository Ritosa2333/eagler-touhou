
// block 00459830
00459830  push       ebx
00459831  fldz       
00459833  push       ebp
00459834  push       esi
00459835  mov        esi, dword ptr [esp + 0x10]
00459839  mov        dword ptr [eax + 0x12c], esi
0045983f  xor        esi, esi
00459841  mov        dword ptr [eax + 0x100], esi
00459847  mov        dword ptr [eax + 0x10c], esi
0045984d  movzx      esi, byte ptr [esp + 0x14]
00459852  push       edi
00459853  xor        edi, edi
00459855  mov        dword ptr [eax + 0x104], edi
0045985b  mov        dword ptr [eax + 0x110], edi
00459861  mov        dword ptr [eax + 0x130], esi
00459867  xor        ebx, ebx
00459869  mov        dword ptr [eax + 0x108], ebx
0045986f  mov        dword ptr [eax + 0x114], ebx
00459875  movzx      edi, byte ptr [ecx + 2]
00459879  movzx      ebx, byte ptr [ecx + 1]
0045987d  movzx      ecx, byte ptr [ecx]
00459880  movzx      ebp, byte ptr [edx + 2]
00459884  movzx      esi, byte ptr [edx + 1]
00459888  movzx      edx, byte ptr [edx]
0045988b  mov        dword ptr [eax + 0xf4], ecx
00459891  mov        dword ptr [eax + 0xf8], ebx
00459897  mov        dword ptr [eax + 0xe8], edx
0045989d  mov        dword ptr [eax + 0xfc], edi
004598a3  pop        edi
004598a4  mov        dword ptr [eax + 0xec], esi
004598aa  mov        dword ptr [eax + 0xf0], ebp
004598b0  mov        ecx, dword ptr [eax + 0x128]
004598b6  pop        esi
004598b7  pop        ebp
004598b8  pop        ebx
004598b9  test       cl, 1
004598bc  jne        0x4598eb

// block 004598be
004598be  or         ecx, 1
004598c1  fst        dword ptr [eax + 0x120]
004598c7  mov        dword ptr [eax + 0x11c], 0
004598d1  mov        dword ptr [eax + 0x118], 0xfff0bdc1
004598db  mov        dword ptr [eax + 0x124], 0x4b2ed0
004598e5  mov        dword ptr [eax + 0x128], ecx

// block 004598eb
004598eb  fstp       dword ptr [eax + 0x120]
004598f1  mov        dword ptr [eax + 0x11c], 0
004598fb  mov        dword ptr [eax + 0x118], 0xffffffff
00459905  ret        8
