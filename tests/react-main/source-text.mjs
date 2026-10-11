import {execFileSync} from 'node:child_process';
/** Git blobs use LF even when the Windows checkout uses CRLF. */
export const normalizeSourceText = value => value.replaceAll('\r\n', '\n');
/** The sole added UI label is covered separately by the shared-settings fixture.
 * All prior messages and domain authorities remain pinned byte for byte. */
export const pinnedUiAuthorityText = value => normalizeSourceText(value).replace(/^  \["settings.thpracMultiplayerDisabled".*\n/m, '');

/** Domain facts now include main's TH11 policy. The newer editable-nickname
 * label is retained from origin/main; do not bless the candidate as its own oracle. */
export function synchronizedMainAuthority(path, project, fallbackRevision) {
  const domain = ['src/contracts/product-catalog.mts', 'src/launcher/i18n.mts', 'src/launcher/multiplayer-guide.mts', 'public/content/MULTIPLAYER.html'].includes(path);
  const revision = domain ? '8bdf6dbf4a81032f0725cc841b86d03e1465fe07' : fallbackRevision;
  let source = execFileSync('git', ['show', `${revision}:${path}`], {cwd: project, encoding: 'utf8'});
  if (path === 'src/launcher/i18n.mts') source = source.replace(
    '  ["multiplayer.nameOneTimeHint", "昵称只能设置一次，保存后不可修改", "Your nickname can be saved only once and cannot be changed later"],',
    '  ["multiplayer.nameEditableHint", "昵称可随时修改，保存后立即生效", "You can change your nickname at any time; changes take effect immediately"],');
  return source;
}
