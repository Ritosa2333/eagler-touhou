
// block 00492b9e
00492b9e  mov        edi, edi
00492ba0  push       ebp
00492ba1  mov        ebp, esp
00492ba3  cmp        dword ptr [ebp + 0x18], 0
00492ba7  je         0x492bb9

// block 00492ba9
00492ba9  push       dword ptr [ebp + 0x18]
00492bac  push       ebx
00492bad  push       esi
00492bae  push       dword ptr [ebp + 8]
00492bb1  call       0x4929c7

// block 00492bb6
00492bb6  add        esp, 0x10

// block 00492bb9
00492bb9  cmp        dword ptr [ebp + 0x20], 0
00492bbd  push       dword ptr [ebp + 8]
00492bc0  jne        0x492bc5

// block 00492bc2
00492bc2  push       esi
00492bc3  jmp        0x492bc8

// block 00492bc5
00492bc5  push       dword ptr [ebp + 0x20]

// block 00492bc8
00492bc8  call       0x491a21

// block 00492bcd
00492bcd  push       dword ptr [edi]
00492bcf  push       dword ptr [ebp + 0x14]
00492bd2  push       dword ptr [ebp + 0x10]
00492bd5  push       esi
00492bd6  call       0x49205b

// block 00492bdb
00492bdb  mov        eax, dword ptr [edi + 4]
00492bde  push       0x100
00492be3  push       dword ptr [ebp + 0x1c]
00492be6  inc        eax
00492be7  push       dword ptr [ebp + 0x14]
00492bea  mov        dword ptr [esi + 8], eax
00492bed  push       dword ptr [ebp + 0xc]
00492bf0  mov        ecx, dword ptr [ebx + 0xc]
00492bf3  push       esi
00492bf4  push       dword ptr [ebp + 8]
00492bf7  call       0x4926ac

// block 00492bfc
00492bfc  add        esp, 0x28
00492bff  test       eax, eax
00492c01  je         0x492c0a

// block 00492c03
00492c03  push       esi
00492c04  push       eax
00492c05  call       0x4919da

// block 00492c0a
00492c0a  pop        ebp
00492c0b  ret        
