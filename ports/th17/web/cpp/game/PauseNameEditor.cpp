#include "PauseNameEditor.hpp"
namespace th17 {
namespace {constexpr char alphabet[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*   ";}
bool PauseNameEditor::check(bool ok,const char* reason){if(!ok&&error.empty())error=reason;return ok;}
bool PauseNameEditor::sound(i32 value){return check(host.sound(value),"Name-entry sound failed");}
bool PauseNameEditor::prepare(){
 if(!error.empty())return false;if(records.name[8])return check(false,"Stored score name exceeds original eight-byte limit");
 state.names.select(0);state.names.count=91;state.names.wrap=true;for(u32 n=0;n<9;n++){state.name[n]=records.name[n];if(!records.name[n])break;}state.name_length=0;
 constexpr std::array<char,9> empty{' ',' ',' ',' ',' ',' ',' ',' ',0};if(state.name!=empty)state.names.step(-1);
 i32 n=8;while(n>0&&state.name[n-1]==' ')n--;state.name_length=n;return true;
}
bool PauseNameEditor::erase(){if(state.name_length>0){state.name[--state.name_length]=' ';return true;}return false;}
bool PauseNameEditor::append(char value){
 if(state.name_length>=8){state.name[state.name_length-1]=value;return sound(7);}
 state.name[state.name_length++]=value;if(state.name_length>=8)state.names.select(90);return sound(7);
}
bool PauseNameEditor::update(u32 pressed,u32 repeated){
 if(!error.empty())return false;if(state.phase!=12&&state.phase!=15)return check(false,"Name entry requested outside score/replay editing");
 if(state.age.current<10)return true;if(state.name_length<0||state.name_length>8)return check(false,"Name length outside original eight-byte limit");const bool replay=state.phase==12;auto& grid=state.names;grid.reserved=grid.selected;
 if((pressed|repeated)&0x10)grid.step(-13);if((pressed|repeated)&0x20)grid.step(13);
 if((pressed|repeated)&0x40)grid.step(grid.selected%13==0?12:-1);if((pressed|repeated)&0x80)grid.step(grid.selected%13==12?-12:1);
 if(grid.reserved!=grid.selected&&!sound(10))return false;
 if(!(pressed&0x80001)){
  if(pressed&0x102){if(!sound(9))return false;if(!erase()&&replay){state.flags=(state.flags&~2u)|1;state.select_phase(11);}}return true;
 }
 if(grid.selected<0||grid.selected>=91)return check(false,"Name grid cursor outside original range");
 if(grid.selected<88)return append(alphabet[grid.selected]);
 if(grid.selected==88)return append(' ');
 if(grid.selected==89)return !erase()||sound(9);
 if(replay){
  state.flags=(state.flags&~2u)|1;if(!sound(17)||!check(host.write_replay(state.menu.selected,state.name),"Named replay write failed"))return false;
  state.select_phase(11);for(u32 n=0;n<9;n++){records.name[n]=state.name[n];if(!state.name[n])break;}return sound(7);
 }
 if(!sound(7)||!check(records.score_name(progress.character*3+progress.animal,progress.difficulty,state.menu.selected,state.name),"Registered score name failed"))return false;
 return check(host.prepare_results_menu(),"Named score result menu failed");
}
}
