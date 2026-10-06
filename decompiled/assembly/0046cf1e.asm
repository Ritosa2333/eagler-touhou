
// block 0046cf1e
0046cf1e  push       0xc
0046cf20  push       0x4aaa58
0046cf25  call       0x46fcec

// block 0046cf2a
0046cf2a  and        dword ptr [ebp - 0x1c], 0
0046cf2e  mov        esi, dword ptr [ebp + 0xc]
0046cf31  mov        eax, esi
0046cf33  imul       eax, dword ptr [ebp + 0x10]
0046cf37  add        dword ptr [ebp + 8], eax
0046cf3a  and        dword ptr [ebp - 4], 0

// block 0046cf3e
0046cf3e  dec        dword ptr [ebp + 0x10]
0046cf41  js         0x46cf4e

// block 0046cf43
0046cf43  sub        dword ptr [ebp + 8], esi
0046cf46  mov        ecx, dword ptr [ebp + 8]
0046cf49  call       dword ptr [ebp + 0x14]

// block 0046cf4c
0046cf4c  jmp        0x46cf3e

// block 0046cf4e
0046cf4e  mov        dword ptr [ebp - 0x1c], 1
0046cf55  mov        dword ptr [ebp - 4], 0xfffffffe
0046cf5c  call       0x46cf69

// block 0046cf61
0046cf61  call       0x46fd31

// block 0046cf66
0046cf66  ret        0x10
