
// block 0047e0f7
0047e0f7  mov        edi, edi
0047e0f9  push       ebp
0047e0fa  mov        ebp, esp
0047e0fc  sub        esp, 0x10
0047e0ff  mov        eax, 0xffff0000
0047e104  and        dword ptr [ebp - 4], eax
0047e107  and        dword ptr [ebp - 0xc], eax
0047e10a  push       1
0047e10c  xor        ecx, ecx
0047e10e  lea        eax, [ebp - 8]
0047e111  push       eax
0047e112  push       ecx
0047e113  lea        eax, [ebp - 0x10]
0047e116  push       eax
0047e117  push       dword ptr [ebp + 8]
0047e11a  mov        dword ptr [ebp - 8], ecx
0047e11d  mov        dword ptr [ebp - 0x10], ecx
0047e120  call       0x481a74

// block 0047e125
0047e125  mov        eax, dword ptr [ebp + 8]
0047e128  add        esp, 0x14
0047e12b  leave      
0047e12c  ret        
