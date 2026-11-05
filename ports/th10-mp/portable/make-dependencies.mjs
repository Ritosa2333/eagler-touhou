// Clang's Make depfiles escape drive colons and spaces, but Windows directory
// separators remain backslashes. Decode Make escapes without deleting those.
export function parseMakeDependencies(contents) {
  const logical = contents.replace(/\\\r?\n/g, '');
  if (!logical.startsWith('source:')) throw Error('Unexpected Clang dependency target');
  const body = logical.slice('source:'.length);
  return (body.match(/(?:\\.|[^\s])+/g) ?? []).map(token =>
    token.replace(/\\([\\ \t:#])/g, '$1').replace(/\$\$/g, '$'));
}
