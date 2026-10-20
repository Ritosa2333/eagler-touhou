
// block 00404620
00404620  sub        esp, 8
00404623  push       ebx
00404624  push       ebp
00404625  mov        ebp, dword ptr [esp + 0x14]
00404629  mov        ecx, dword ptr [ebp + 0x10]
0040462c  xor        eax, eax
0040462e  xor        edx, edx
00404630  cmp        dx, word ptr [ecx]
00404633  push       esi
00404634  push       edi
00404635  mov        dword ptr [esp + 0x14], eax
00404639  jge        0x4046ad

// block 0040463b
0040463b  mov        ecx, dword ptr [ebp + 0x14]
0040463e  mov        ebx, dword ptr [ecx + eax*4]
00404641  test       byte ptr [ebx + 3], 1
00404645  je         0x40469a

// block 00404647
00404647  cmp        word ptr [ebx + 0x1c], 0
0040464c  lea        edi, [ebx + 0x1c]
0040464f  mov        dword ptr [esp + 0x1c], 0
00404657  jl         0x404696

// block 00404659
00404659  lea        esp, [esp]

// block 00404660
00404660  movsx      esi, word ptr [edi + 6]
00404664  imul       esi, esi, 0x4b4
0040466a  add        esi, dword ptr [ebp + 0x1c8]
00404670  push       esi
00404671  call       0x455630

// block 00404676
00404676  cmp        dword ptr [esi + 0x3f0], 0
0040467d  je         0x404683

// block 0040467f
0040467f  inc        dword ptr [esp + 0x1c]

// block 00404683
00404683  movsx      edx, word ptr [edi + 2]
00404687  add        edi, edx
00404689  cmp        word ptr [edi], 0
0040468d  jge        0x404660

// block 0040468f
0040468f  cmp        dword ptr [esp + 0x1c], 0
00404694  jne        0x40469a

// block 00404696
00404696  and        byte ptr [ebx + 3], 0xfe

// block 0040469a
0040469a  mov        ecx, dword ptr [ebp + 0x10]
0040469d  mov        eax, dword ptr [esp + 0x14]
004046a1  movsx      edx, word ptr [ecx]
004046a4  inc        eax
004046a5  cmp        eax, edx
004046a7  mov        dword ptr [esp + 0x14], eax
004046ab  jl         0x40463b

// block 004046ad
004046ad  pop        edi
004046ae  pop        esi
004046af  pop        ebp
004046b0  xor        eax, eax
004046b2  pop        ebx
004046b3  add        esp, 8
004046b6  ret        4
