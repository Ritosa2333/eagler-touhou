import { lazy, Suspense } from 'react';

const FlagGraphic = lazy(async () => {
  const flags = await import('country-flag-icons/react/3x2');
  return {
    default: function FlagGraphic({ code }: { code: string }) {
      const Graphic = flags[code as keyof typeof flags];
      return Graphic ? <Graphic style={{ width: '100%', height: '100%' }} /> : null;
    },
  };
});

export function CountryFlag({ code, label }: { code: string; label?: string }) {
  return (
    <span
      className="country-flag"
      role="img"
      aria-label={label}
      aria-hidden={label ? undefined : true}
      title={label}
      style={{ display: 'inline-flex', width: 18, height: 12, flex: 'none' }}
    >
      <Suspense fallback={null}>
        <FlagGraphic code={code === 'TW' ? 'CN' : code} />
      </Suspense>
    </span>
  );
}
