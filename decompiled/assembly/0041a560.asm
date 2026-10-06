
// block 0041a560
0041a560  mov        eax, dword ptr [esp + 4]
0041a564  add        eax, 0x26fd
0041a569  cmp        eax, 0x31
0041a56c  ja         0x41a60b

// block 0041a572
0041a572  movzx      eax, byte ptr [eax + 0x41a644]
0041a579  jmp        dword ptr [eax*4 + 0x41a610]

// block 0041a580
0041a580  lea        eax, [ecx + 0x128c]
0041a586  ret        4

// block 0041a589
0041a589  lea        eax, [ecx + 0x1290]
0041a58f  ret        4

// block 0041a592
0041a592  lea        eax, [ecx + 0x1294]
0041a598  ret        4

// block 0041a59b
0041a59b  lea        eax, [ecx + 0x1298]
0041a5a1  ret        4

// block 0041a5a4
0041a5a4  lea        eax, [ecx + 0x129c]
0041a5aa  ret        4

// block 0041a5ad
0041a5ad  lea        eax, [ecx + 0x12a0]
0041a5b3  ret        4

// block 0041a5b6
0041a5b6  lea        eax, [ecx + 0x12a4]
0041a5bc  ret        4

// block 0041a5bf
0041a5bf  lea        eax, [ecx + 0x12a8]
0041a5c5  ret        4

// block 0041a5c8
0041a5c8  mov        ecx, dword ptr [0x4b43dc]
0041a5ce  mov        eax, dword ptr [ecx + 0x1c]
0041a5d1  add        eax, 0x128c
0041a5d6  ret        4

// block 0041a5d9
0041a5d9  mov        edx, dword ptr [0x4b43dc]
0041a5df  mov        eax, dword ptr [edx + 0x1c]
0041a5e2  add        eax, 0x1290
0041a5e7  ret        4

// block 0041a5ea
0041a5ea  mov        eax, dword ptr [0x4b43dc]
0041a5ef  mov        eax, dword ptr [eax + 0x1c]
0041a5f2  add        eax, 0x1294
0041a5f7  ret        4

// block 0041a5fa
0041a5fa  mov        ecx, dword ptr [0x4b43dc]
0041a600  mov        eax, dword ptr [ecx + 0x1c]
0041a603  add        eax, 0x1298
0041a608  ret        4

// block 0041a60b
0041a60b  xor        eax, eax
0041a60d  ret        4
