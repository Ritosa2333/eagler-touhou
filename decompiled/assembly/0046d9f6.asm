
// block 0046d9f6
0046d9f6  push       0xc
0046d9f8  push       0x4aaa98
0046d9fd  call       0x46fcec

// block 0046da02
0046da02  call       0x472c79

// block 0046da07
0046da07  and        dword ptr [ebp - 4], 0
0046da0b  push       dword ptr [ebp + 8]
0046da0e  call       0x46d90b

// block 0046da13
0046da13  pop        ecx
0046da14  mov        dword ptr [ebp - 0x1c], eax
0046da17  mov        dword ptr [ebp - 4], 0xfffffffe
0046da1e  call       0x46da2c

// block 0046da23
0046da23  mov        eax, dword ptr [ebp - 0x1c]
0046da26  call       0x46fd31

// block 0046da2b
0046da2b  ret        
