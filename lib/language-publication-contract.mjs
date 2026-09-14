export function assertLanguagePublicationConsistency(game, hostEntry, descriptor) {
  const hostLanguages = Array.isArray(hostEntry?.languages) ? hostEntry.languages : [];
  const descriptorEntries = descriptor?.components?.language?.entries;
  if (!hostLanguages.length) {
    if (descriptorEntries !== undefined) throw new Error(`${game}: Descriptor publishes undeclared language entries`);
    return;
  }
  if (!Array.isArray(descriptorEntries) || descriptorEntries.length !== hostLanguages.length) {
    throw new Error(`${game}: Host / Descriptor language entry set mismatch`);
  }
  const hostById = new Map(hostLanguages.map(language => [language.id, language]));
  for (const language of descriptorEntries) {
    const hostLanguage = hostById.get(language?.id);
    const file = descriptor?.files?.[language?.file];
    const pack = hostLanguage?.pack;
    if (!hostLanguage || !file || file.source !== pack.url || file.bytes !== pack.bytes ||
        String(file.sha256 || "").toLowerCase() !== String(pack.sha256).toLowerCase() ||
        file.revision !== String(pack.sha256).slice(0, 16).toLowerCase() ||
        file.target !== `/__eagler/language/${language.id}.zip`) {
      throw new Error(`${game}: Host / Descriptor language identity mismatch: ${language?.id || "unknown"}`);
    }
    hostById.delete(language.id);
  }
  if (hostById.size) throw new Error(`${game}: Descriptor omitted Host language entries`);
}
