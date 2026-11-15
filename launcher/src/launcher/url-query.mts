/** Update owned fields while preserving unrelated query bytes and duplicates. */
export function updateSearchParameters(search: string, fields: Readonly<Record<string, string | null>>): string {
  const parts = search && search !== '?' ? search.replace(/^\?/, '').split('&').filter(part =>
    ![...new URLSearchParams(part).keys()].some(key => Object.hasOwn(fields, key))) : [];
  const owned = new URLSearchParams();
  for (const [key, value] of Object.entries(fields)) if (value !== null) owned.append(key, value);
  if (owned.size) parts.push(owned.toString());
  return parts.length ? '?' + parts.join('&') : '';
}
