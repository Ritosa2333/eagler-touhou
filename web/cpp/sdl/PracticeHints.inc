// Purple TH10 Hint editor: native StageHints animations at 0xdc, ANM fields
// scale 0x3c, RGB 0x2fc, alpha variable 0x310, position 0x340, size 0x3a0.
bool move_hints=false;int selected_hint=-1;u32 selected_hint_id=0;
AnmVm* hint_vm(Application& app,int index){
 if(!app.world||!app.world->actors.hints)return nullptr;
 const auto id=app.world->actors.hints->animations[index];
 return id?app.engine.manager.registry.find(id):nullptr;
}
Vec2 hint_scale(const AnmVm& vm){const float factor=vm.text_settings[0]>15?2.f:1.f;return {vm.scale.x*factor,vm.scale.y*factor};}
void hint_options(Application& app){
 if(!ImGui::CollapsingHeader(label(practice_TH10_HINT_EDIT))||!app.world||!app.world->actors.hints)return;
 ImGui::Checkbox(label(practice_TH10_HINT_EDIT_DRAG),&move_hints);
 if(!ImGui::BeginTable("hints",4,ImGuiTableFlags_Resizable))return;
 ImGui::TableSetupColumn("id",ImGuiTableColumnFlags_WidthFixed);
 ImGui::TableSetupColumn(label(practice_TH10_HINT_EDIT_POS));ImGui::TableSetupColumn(label(practice_TH10_HINT_EDIT_SCALE));ImGui::TableSetupColumn(label(practice_TH10_HINT_EDIT_COLOR));ImGui::TableHeadersRow();
 for(int i=0;i<10;++i){auto* vm=hint_vm(app,i);if(!vm)continue;ImGui::PushID(i);
  const auto scale=hint_scale(*vm);float pos[]{vm->position.x-224,vm->position.y-16-scale.y*8};
  ImGui::TableNextRow();ImGui::TableNextColumn();ImGui::Text("%x(%d)",unsigned(uintptr_t(vm)),int(vm->id));ImGui::SameLine();
  if(ImGui::Button(label(practice_TH10_HINT_EDIT_COPY))){char text[256];std::snprintf(text,sizeof text,"   Pos:%g, %g\r\n   Alpha:%u\r\n   Scale:%g\r\n   Color:%u, %u, %u\r\n",pos[0],pos[1],unsigned(vm->integer_variables[1]),scale.x,(vm->color>>16)&255,(vm->color>>8)&255,vm->color&255);ImGui::SetClipboardText(text);}
  ImGui::SameLine();ImGui::Text("hint %d",i);ImGui::TableNextColumn();
  if(ImGui::DragFloat2("##poshint",pos,1)){vm->position.x=pos[0]+224;vm->position.y=pos[1]+16+scale.y*8;}
  ImGui::TableNextColumn();float input=scale.x;const bool large=vm->text_settings[0]>15;
  if(ImGui::DragFloat("##scalehint",&input,.1f,large?1.07f:0.f,large?4.f:1.f)){input=std::clamp(input,large?1.07f:0.f,large?4.f:1.f);vm->scale.x=vm->scale.y=input*(large?.5f:1.f);}
  ImGui::TableNextColumn();ImVec4 color{float((vm->color>>16)&255)/255,float((vm->color>>8)&255)/255,float(vm->color&255)/255,vm->integer_variables[1]/255.f};
  if(ImGui::ColorEdit4("##colorhintpicker",&color.x,ImGuiColorEditFlags_AlphaPreview)){vm->integer_variables[1]=int(std::floor(255.f*color.w))&255;vm->color=(u32(std::floor(255.f*color.x))<<16)|(u32(std::floor(255.f*color.y))<<8)|u32(std::floor(255.f*color.z))|(u32(vm->integer_variables[1])<<24);}
  ImGui::PopID();
 }
 ImGui::EndTable();
}
void drag_hints(Application& app){
 if(advanced_open||!move_hints||!app.world||!app.world->actors.player){selected_hint=-1;selected_hint_id=0;return;}
 if(!ImGui::IsMouseDown(0)){selected_hint=-1;selected_hint_id=0;}
 auto* draw=ImGui::GetForegroundDrawList();
 for(int i=0;i<10;++i){auto* vm=hint_vm(app,i);if(!vm)continue;
  if(selected_hint==i&&selected_hint_id!=vm->id){selected_hint=-1;selected_hint_id=0;}
  ImVec2 lo{vm->position.x-8,vm->position.y-8},hi{vm->position.x+8,vm->position.y+8};ImU32 color=0x22CCCCFF;
  if(ImGui::IsMouseHoveringRect(lo,hi,false)&&selected_hint==-1&&(!ImGui::IsMouseDown(0)||ImGui::IsMouseClicked(0))){selected_hint=i;selected_hint_id=vm->id;}
  if(selected_hint==i){color=0x88FFCCCC;if(ImGui::IsMouseDown(0)){const auto delta=ImGui::GetMouseDragDelta(0,0);ImGui::ResetMouseDragDelta(0);vm->position.x+=delta.x;vm->position.y+=delta.y;}}
  draw->AddRectFilled(lo,hi,color);draw->AddRect(lo,hi,0xffffffff);
 }
}
