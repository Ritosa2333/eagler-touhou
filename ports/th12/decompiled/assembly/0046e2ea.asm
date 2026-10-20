
// block 0046e2ea
0046e2ea  mov        edi, edi
0046e2ec  push       ebp
0046e2ed  mov        ebp, esp
0046e2ef  mov        edx, dword ptr [ebp + 0xc]
0046e2f2  push       esi
0046e2f3  mov        esi, dword ptr [ebp + 8]
0046e2f6  push       edi

// block 0046e2f7
0046e2f7  movzx      eax, byte ptr [esi]
0046e2fa  lea        ecx, [eax - 0x41]
0046e2fd  inc        esi
0046e2fe  cmp        ecx, 0x19
0046e301  ja         0x46e306

// block 0046e303
0046e303  add        eax, 0x20

// block 0046e306
0046e306  movzx      ecx, byte ptr [edx]
0046e309  lea        edi, [ecx - 0x41]
0046e30c  inc        edx
0046e30d  cmp        edi, 0x19
0046e310  ja         0x46e315

// block 0046e312
0046e312  add        ecx, 0x20

// block 0046e315
0046e315  test       eax, eax
0046e317  je         0x46e31d

// block 0046e319
0046e319  cmp        eax, ecx
0046e31b  je         0x46e2f7

// block 0046e31d
0046e31d  pop        edi
0046e31e  sub        eax, ecx
0046e320  pop        esi
0046e321  pop        ebp
0046e322  ret        
