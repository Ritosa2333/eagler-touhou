#include "SakuyaState.hpp"
namespace th075 {u8 recognize_sakuya_command(const InputHistory& history,i8 facing){u8 result=0;
  i8 matched;
  
  result = 0;
  matched = history.match("236236D",7,0x1e,facing);
  if (matched == '\0') {
    matched = history.match("421C",4,0xf,facing);
    if (matched == '\0') {
      matched = history.match("421B",4,0xf,facing);
      if (matched == '\0') {
        matched = history.match("421A",4,0xf,facing);
        if (matched == '\0') {
          matched = history.match("623B",4,0xf,facing);
          if (matched == '\0') {
            matched = history.match("623A",4,0xf,facing);
            if (matched == '\0') {
              matched = history.match("214D",4,0xf,facing);
              if (matched == '\0') {
                matched = history.match("214B",4,0xf,facing);
                if (matched == '\0') {
                  matched = history.match("214A",4,0xf,facing);
                  if (matched == '\0') {
                    matched = history.match("236D",4,0xf,facing);
                    if (matched == '\0') {
                      matched = history.match("236C",4,0xf,facing);
                      if (matched == '\0') {
                        matched = history.match("236B",4,0xf,facing);
                        if (matched == '\0') {
                          matched = history.match("236A",4,0xf,facing);
                          if (matched == '\0') {
                            matched = history.match("2N2D",4,0xf,facing);
                            if (matched == '\0') {
                              matched = history.match("2N2C",4,0xf,facing);
                              if (matched == '\0') {
                                matched = history.match("2N2B",4,0xf,facing);
                                if (matched == '\0') {
                                  matched = history.match("2N2A",4,0xf,facing);
                                  if (matched == '\0') {
                                    matched = history.match("N29",3,10,facing);
                                    if (matched == '\0') {
                                      matched = history.match("N39",3,10,facing);
                                      if (matched == '\0') {
                                        matched = history.match("N19",3,10,facing);
                                        if (matched == '\0') {
                                          matched = history.match("N27",3,10,facing);
                                          if (matched == '\0') {
                                            matched = history.match("N37",3,10,facing);
                                            if (matched == '\0') {
                                              matched = history.match("N17",3,10,facing);
                                              if (matched == '\0') {
                                                matched = history.match("N28",3,10,facing);
                                                if (matched == '\0') {
                                                  matched = history.match("N38",3,10,facing);
                                                  if (matched == '\0') {
                                                    matched = history.match("N18",3,10,facing);
                                                    if (matched == '\0') {
                                                      matched = history.match("NRNR",4,0xf,facing);
                                                      if (matched == '\0') {
                                                        matched = history.match("NLNL",4,0xf,facing);
                                                        if (matched != '\0') {
                                                          result = 2;
                                                        }
                                                      }
                                                      else {
                                                        result = 1;
                                                      }
                                                    }
                                                    else {
                                                      result = 3;
                                                    }
                                                  }
                                                  else {
                                                    result = 3;
                                                  }
                                                }
                                                else {
                                                  result = 3;
                                                }
                                              }
                                              else {
                                                result = 4;
                                              }
                                            }
                                            else {
                                              result = 4;
                                            }
                                          }
                                          else {
                                            result = 4;
                                          }
                                        }
                                        else {
                                          result = 5;
                                        }
                                      }
                                      else {
                                        result = 5;
                                      }
                                    }
                                    else {
                                      result = 5;
                                    }
                                  }
                                  else {
                                    result = 100;
                                  }
                                }
                                else {
                                  result = 0x65;
                                }
                              }
                              else {
                                result = 0x66;
                              }
                            }
                            else {
                              result = 0x67;
                            }
                          }
                          else {
                            result = 10;
                          }
                        }
                        else {
                          result = 0xb;
                        }
                      }
                      else {
                        result = 0xc;
                      }
                    }
                    else {
                      result = 0xd;
                    }
                  }
                  else {
                    result = 0x14;
                  }
                }
                else {
                  result = 0x15;
                }
              }
              else {
                result = 0x17;
              }
            }
            else {
              result = 0x1e;
            }
          }
          else {
            result = 0x1f;
          }
        }
        else {
          result = 0x28;
        }
      }
      else {
        result = 0x29;
      }
    }
    else {
      result = 0x2a;
    }
  }
  else {
    result = 0x71;
  }
  return result;
}}
