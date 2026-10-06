
// block 00496673
00496673  mov        edi, edi
00496675  push       ebp
00496676  mov        ebp, esp
00496678  cmp        dword ptr [ebp + 8], 1
0049667c  je         0x496693

// block 0049667e
0049667e  jle        0x49669e

// block 00496680
00496680  cmp        dword ptr [ebp + 8], 3
00496684  jg         0x49669e

// block 00496686
00496686  call       0x46e96f

// block 0049668b
0049668b  mov        dword ptr [eax], 0x22
00496691  pop        ebp
00496692  ret        

// block 00496693
00496693  call       0x46e96f

// block 00496698
00496698  mov        dword ptr [eax], 0x21

// block 0049669e
0049669e  pop        ebp
0049669f  ret        
