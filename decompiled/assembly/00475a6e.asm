
// block 00475a6e
00475a6e  mov        edi, edi
00475a70  push       ebp
00475a71  mov        ebp, esp
00475a73  mov        edx, dword ptr [ebp + 8]
00475a76  push       esi
00475a77  mov        esi, dword ptr [ebp + 0xc]
00475a7a  lea        ecx, [edx + esi]
00475a7d  xor        eax, eax
00475a7f  cmp        ecx, edx
00475a81  jb         0x475a87

// block 00475a83
00475a83  cmp        ecx, esi
00475a85  jae        0x475a8a

// block 00475a87
00475a87  xor        eax, eax
00475a89  inc        eax

// block 00475a8a
00475a8a  mov        edx, dword ptr [ebp + 0x10]
00475a8d  mov        dword ptr [edx], ecx
00475a8f  pop        esi
00475a90  pop        ebp
00475a91  ret        
