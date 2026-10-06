
// block 0046d608
0046d608  mov        edi, edi
0046d60a  push       ebp
0046d60b  mov        ebp, esp
0046d60d  cmp        dword ptr [ebp + 8], 0
0046d611  je         0x46d62a

// block 0046d613
0046d613  push       dword ptr [ebp + 0x1c]
0046d616  push       dword ptr [ebp + 0x18]
0046d619  push       dword ptr [ebp + 0x14]
0046d61c  push       dword ptr [ebp + 0x10]
0046d61f  push       dword ptr [ebp + 0xc]
0046d622  call       0x470dc6

// block 0046d627
0046d627  add        esp, 0x14

// block 0046d62a
0046d62a  pop        ebp
0046d62b  ret        
