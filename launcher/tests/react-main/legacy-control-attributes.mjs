// Snapshot the original DOM contract while allowing the deliberate shadcn
// presentation markers. IDs, roles, labels, native attributes and old classes
// remain part of the pinned oracle; interaction has separate mounted tests.
const inputUtilities = new Set(['flex', 'h-9', 'w-full', 'min-w-0', 'rounded-md',
  'border', 'border-input', 'bg-background', 'px-3', 'py-1', 'text-base', 'text-foreground', 'md:text-sm']);
export function legacyControlAttributes(element) {
  const slot = element.getAttribute('data-slot');
  return [...element.attributes].flatMap(attribute => {
    if (attribute.name === 'data-slot') return [];
    // Decorative SVGs now inherit the containing control's translated accessible name.
    if (element.localName === 'svg' && attribute.name === 'aria-hidden' && attribute.value === 'true') return [];
    let value = attribute.value;
    if (attribute.name === 'class' && ['button', 'input'].includes(slot)) {
      value = value.split(/\s+/).filter(name => !/^(focus-visible:|disabled:)/.test(name) &&
        !(slot === 'input' && (inputUtilities.has(name) || name === 'placeholder:text-muted-foreground'))).join(' ');
      if (!value) return [];
    }
    return [{name: attribute.name, value}];
  });
}
