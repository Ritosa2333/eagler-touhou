
// block 004554d0
004554d0  fld        dword ptr [edi]
004554d2  call       0x4931e0

// block 004554d7
004554d7  add        eax, 0xffffd8ec
004554dc  cmp        eax, 0x15
004554df  ja         0x45552c

// block 004554e1
004554e1  movzx      eax, byte ptr [eax + 0x45555c]
004554e8  jmp        dword ptr [eax*4 + 0x455530]

// block 004554ef
004554ef  lea        eax, [esi + 0x40c]
004554f5  ret        

// block 004554f6
004554f6  lea        eax, [esi + 0x410]
004554fc  ret        

// block 004554fd
004554fd  lea        eax, [esi + 0x414]
00455503  ret        

// block 00455504
00455504  lea        eax, [esi + 0x418]
0045550a  ret        

// block 0045550b
0045550b  lea        eax, [esi + 0x424]
00455511  ret        

// block 00455512
00455512  lea        eax, [esi + 0x428]
00455518  ret        

// block 00455519
00455519  lea        eax, [esi + 0x42c]
0045551f  ret        

// block 00455520
00455520  lea        eax, [esi + 0x24]
00455523  ret        

// block 00455524
00455524  lea        eax, [esi + 0x28]
00455527  ret        

// block 00455528
00455528  lea        eax, [esi + 0x2c]
0045552b  ret        

// block 0045552c
0045552c  mov        eax, edi
0045552e  ret        
