
// block 0043d020
0043d020  mov        eax, dword ptr [esi]
0043d022  test       eax, eax
0043d024  je         0x43d035

// block 0043d026
0043d026  push       eax
0043d027  call       0x46c941

// block 0043d02c
0043d02c  add        esp, 4
0043d02f  mov        dword ptr [esi], 0

// block 0043d035
0043d035  mov        eax, dword ptr [esi + 4]
0043d038  test       eax, eax
0043d03a  je         0x43d04c

// block 0043d03c
0043d03c  push       eax
0043d03d  call       0x46c941

// block 0043d042
0043d042  add        esp, 4
0043d045  mov        dword ptr [esi + 4], 0

// block 0043d04c
0043d04c  ret        
