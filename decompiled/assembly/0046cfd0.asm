
// block 0046cfd0
0046cfd0  mov        edi, edi
0046cfd2  push       ebp
0046cfd3  mov        ebp, esp
0046cfd5  cmp        dword ptr [0x4b3c04], 0
0046cfdc  jne        0x46cff6

// block 0046cfde
0046cfde  call       0x47315e

// block 0046cfe3
0046cfe3  push       0x1e
0046cfe5  call       0x472f8d

// block 0046cfea
0046cfea  push       0xff
0046cfef  call       0x472c61

// block 0046cff6
0046cff6  mov        eax, dword ptr [0x4d6458]
0046cffb  cmp        eax, 1
0046cffe  jne        0x46d019

// block 0046d000
0046d000  mov        eax, dword ptr [ebp + 8]
0046d003  test       eax, eax
0046d005  jne        0x46d008

// block 0046d007
0046d007  inc        eax

// block 0046d008
0046d008  push       eax
0046d009  push       0
0046d00b  push       dword ptr [0x4b3c04]
0046d011  call       dword ptr [0x4981d4]

// block 0046d017
0046d017  pop        ebp
0046d018  ret        

// block 0046d019
0046d019  push       esi
0046d01a  mov        esi, dword ptr [ebp + 8]
0046d01d  cmp        eax, 3
0046d020  jne        0x46d02d

// block 0046d022
0046d022  push       esi
0046d023  call       0x46cf81

// block 0046d028
0046d028  pop        ecx
0046d029  test       eax, eax
0046d02b  jne        0x46d047

// block 0046d02d
0046d02d  test       esi, esi
0046d02f  jne        0x46d032

// block 0046d031
0046d031  inc        esi

// block 0046d032
0046d032  add        esi, 0xf
0046d035  and        esi, 0xfffffff0
0046d038  push       esi
0046d039  push       0
0046d03b  push       dword ptr [0x4b3c04]
0046d041  call       dword ptr [0x4981d4]

// block 0046d047
0046d047  pop        esi
0046d048  pop        ebp
0046d049  ret        
