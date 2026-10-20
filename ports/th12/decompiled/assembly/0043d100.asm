
// block 0043d100
0043d100  mov        eax, dword ptr [esi]
0043d102  test       eax, eax
0043d104  je         0x43d115

// block 0043d106
0043d106  push       eax
0043d107  call       0x46c941

// block 0043d10c
0043d10c  add        esp, 4
0043d10f  mov        dword ptr [esi], 0

// block 0043d115
0043d115  mov        eax, dword ptr [esi + 4]
0043d118  test       eax, eax
0043d11a  je         0x43d12c

// block 0043d11c
0043d11c  push       eax
0043d11d  call       0x46c941

// block 0043d122
0043d122  add        esp, 4
0043d125  mov        dword ptr [esi + 4], 0

// block 0043d12c
0043d12c  push       esi
0043d12d  call       0x46ca4f

// block 0043d132
0043d132  add        esp, 4
0043d135  mov        eax, esi
0043d137  ret        
