
// block 00477007
00477007  push       0xc
00477009  push       0x4aadd8
0047700e  call       0x46fcec

// block 00477013
00477013  push       6
00477015  call       0x46ec9a

// block 0047701a
0047701a  pop        ecx
0047701b  and        dword ptr [ebp - 4], 0
0047701f  mov        edi, dword ptr [ebp + 8]
00477022  call       0x476da8

// block 00477027
00477027  mov        dword ptr [ebp - 0x1c], eax
0047702a  mov        dword ptr [ebp - 4], 0xfffffffe
00477031  call       0x47703f

// block 00477036
00477036  mov        eax, dword ptr [ebp - 0x1c]
00477039  call       0x46fd31

// block 0047703e
0047703e  ret        
