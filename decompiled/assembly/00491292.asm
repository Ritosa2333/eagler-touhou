
// block 00491292
00491292  mov        edi, edi
00491294  push       ebp
00491295  mov        ebp, esp
00491297  push       ecx
00491298  cmp        dword ptr [0x4d52dc], 0
0049129f  je         0x4912ad

// block 004912a1
004912a1  stmxcsr    dword ptr [ebp - 4]
004912a5  and        dword ptr [ebp - 4], 0xffffffc0
004912a9  ldmxcsr    dword ptr [ebp - 4]

// block 004912ad
004912ad  leave      
004912ae  ret        
