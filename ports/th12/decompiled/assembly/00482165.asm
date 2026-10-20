
// block 00482165
00482165  mov        edi, edi
00482167  push       ebp
00482168  mov        ebp, esp
0048216a  push       0x2a
0048216c  push       dword ptr [ebp + 0x10]
0048216f  push       dword ptr [ebp + 0xc]
00482172  push       dword ptr [ebp + 8]
00482175  call       0x48206c

// block 0048217a
0048217a  mov        eax, dword ptr [ebp + 8]
0048217d  add        esp, 0x10
00482180  pop        ebp
00482181  ret        
