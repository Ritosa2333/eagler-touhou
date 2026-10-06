
// block 0047e869
0047e869  mov        edi, edi
0047e86b  push       ebp
0047e86c  mov        ebp, esp
0047e86e  xor        eax, eax
0047e870  push       esi
0047e871  mov        esi, ecx
0047e873  mov        byte ptr [esi + 4], al
0047e876  and        dword ptr [esi + 4], 0xffff00ff
0047e87d  mov        dword ptr [esi], eax
0047e87f  cmp        byte ptr [ebp + 8], al
0047e882  je         0x47e88f

// block 0047e884
0047e884  push       1
0047e886  lea        eax, [ebp + 8]
0047e889  push       eax
0047e88a  call       0x47e4f1

// block 0047e88f
0047e88f  mov        eax, esi
0047e891  pop        esi
0047e892  pop        ebp
0047e893  ret        4
