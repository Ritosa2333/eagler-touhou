// Build-time adapter: expose the existing browser controller to the managed
// input owner, without adding keyboard emulation inside DOS.
export async function adaptPC98Practice({patch, replaceOnce, game}) {
  await patch('practice-ui.js', text => {
    text = replaceOnce(text, 'return {configuration,', 'return {configuration,handleKey:onKey,show,', 'practice browser controller');
    text = replaceOnce(text, "window.addEventListener('keydown',onKey,true);", '', 'managed practice keyboard ownership');
    text = replaceOnce(text, 'function show(advanced=false){', 'function show(advanced=false){globalThis.__eaglerPC98Input.releaseGameplay();', 'release managed input for practice');
    return text;
  });
}
