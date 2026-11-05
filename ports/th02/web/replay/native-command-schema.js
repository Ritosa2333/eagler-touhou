export const NATIVE_COMMANDS=new Set(['th_init','th_set_netplay_rules','th_start_stage_entrance','th_step','th_set_difficulty','th_set_pause_overlay','th_set_bomb_reduced_flash',
  'th_continue','th_debug_hit','th_debug_position','th_debug_invincible','th_menu_open','th_menu_input','th_stage_command','th_score_load']);
export function validNativeCommand(command){
  return command&&NATIVE_COMMANDS.has(command.name)&&Array.isArray(command.args)&&command.args.length<=8&&
    command.args.every(n=>Number.isInteger(n)&&n>=-2147483648&&n<=4294967295)&&
    (command.name==='th_score_load'?typeof command.scores==='string'&&command.scores.length<=8192&&command.scores.length%4===0&&/^[A-Za-z0-9+/]*={0,2}$/.test(command.scores):command.scores===undefined);
}
