
// block 00412740
00412740  cmp        ecx, 0x63
00412743  mov        eax, dword ptr [0x4b43e4]
00412748  mov        edx, 0x63
0041274d  jg         0x412751

// block 0041274f
0041274f  mov        edx, ecx

// block 00412751
00412751  cmp        ecx, 0x63
00412754  mov        dword ptr [eax + 0x6d38], edx
0041275a  mov        ecx, 0x63
0041275f  jg         0x412765

// block 00412761
00412761  mov        ecx, dword ptr [esp + 4]

// block 00412765
00412765  mov        dword ptr [eax + 0x6d3c], ecx
0041276b  ret        4
