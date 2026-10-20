
// block 004123c0
004123c0  push       ebx
004123c1  push       ebp
004123c2  mov        ebp, 0xfffffffe
004123c7  and        dword ptr [esi + 0x27c], ebp
004123cd  and        dword ptr [esi + 0x2cc], ebp
004123d3  and        dword ptr [esi + 0x318], ebp
004123d9  and        dword ptr [esi + 0x354], ebp
004123df  and        dword ptr [esi + 0x390], ebp
004123e5  and        dword ptr [esi + 0x3cc], ebp
004123eb  and        dword ptr [esi + 0x408], ebp
004123f1  and        dword ptr [esi + 0x444], ebp
004123f7  and        dword ptr [esi + 0x480], ebp
004123fd  push       edi
004123fe  lea        edi, [esi + 0x48c]
00412404  lea        ebx, [ebp + 9]

// block 00412407
00412407  push       0x214
0041240c  push       0
0041240e  push       edi
0041240f  call       0x477420

// block 00412414
00412414  mov        dword ptr [edi + 0x208], 0xffffffff
0041241e  add        esp, 0xc
00412421  add        edi, 0x214
00412427  sub        ebx, 1
0041242a  jns        0x412407

// block 0041242c
0041242c  and        dword ptr [esi + 0x16a0], ebp
00412432  and        dword ptr [esi + 0x16b4], ebp
00412438  pop        edi
00412439  pop        ebp
0041243a  mov        eax, esi
0041243c  pop        ebx
0041243d  ret        
