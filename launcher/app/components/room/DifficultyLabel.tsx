import './room-presentation.css';
export function DifficultyLabel({ name }: { name: string; assetUrl?(path: string): string }) {
  return (
    <span className="difficulty-label" data-difficulty={name.toLowerCase()}>
      {name}
    </span>
  );
}
