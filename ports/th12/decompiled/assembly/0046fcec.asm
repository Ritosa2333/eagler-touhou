
// block 0046fcec
0046fcec  push       0x46fd80
0046fcf1  push       dword ptr fs:[0]
0046fcf8  mov        eax, dword ptr [esp + 0x10]
0046fcfc  mov        dword ptr [esp + 0x10], ebp
0046fd00  lea        ebp, [esp + 0x10]
0046fd04  sub        esp, eax
0046fd06  push       ebx
0046fd07  push       esi
0046fd08  push       edi
0046fd09  mov        eax, dword ptr [0x4ad138]
0046fd0e  xor        dword ptr [ebp - 4], eax
0046fd11  xor        eax, ebp
0046fd13  push       eax
0046fd14  mov        dword ptr [ebp - 0x18], esp
0046fd17  push       dword ptr [ebp - 8]
0046fd1a  mov        eax, dword ptr [ebp - 4]
0046fd1d  mov        dword ptr [ebp - 4], 0xfffffffe
0046fd24  mov        dword ptr [ebp - 8], eax
0046fd27  lea        eax, [ebp - 0x10]
0046fd2a  mov        dword ptr fs:[0], eax
0046fd30  ret        
