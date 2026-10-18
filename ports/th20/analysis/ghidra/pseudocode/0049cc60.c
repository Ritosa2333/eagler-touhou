/* Entry: 0x0049cc60; symbol: length; body bytes: 48 */

/* Library Function - Single Match
    public: virtual int __thiscall pDNameNode::length(void)const 
   
   Library: Visual Studio */

int __thiscall pDNameNode::length(pDNameNode *this)

{
  int local_c;
  
  if (*(int *)(this + 4) == 0) {
    local_c = 0;
  }
  else {
    local_c = FUN_0040ff90(*(int *)(this + 4));
  }
  return local_c;
}

