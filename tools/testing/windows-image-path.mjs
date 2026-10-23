/** Windows Job Object cleanup requires an image path anchored at a drive or UNC share. */
export function isFullWindowsImagePath(path) {
  if(typeof path!=='string'||!path||path.trim()!==path)return false;
  if(/^[a-z]:[\\/]/i.test(path))return true;
  return /^(?:\\\\|\/\/)(?![?.][\\/])[^\\/]+[\\/][^\\/]+(?:[\\/]|$)/.test(path);
}
