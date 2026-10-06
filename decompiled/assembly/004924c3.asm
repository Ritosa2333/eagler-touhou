
// block 004924c3
004924c3  push       8
004924c5  push       0x4ab458
004924ca  call       0x46fcec

// block 004924cf
004924cf  and        dword ptr [ebp - 4], 0
004924d3  push       dword ptr [ebp + 0xc]
004924d6  call       dword ptr [ebp + 8]

// block 004924d9
004924d9  jmp        0x4924e8

// block 004924e8
004924e8  mov        dword ptr [ebp - 4], 0xfffffffe
004924ef  call       0x46fd31

// block 004924f4
004924f4  ret        
