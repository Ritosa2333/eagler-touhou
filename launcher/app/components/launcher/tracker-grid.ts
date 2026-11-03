export type TrackerGridCell = {x: number; y: number};
export type TrackerGridEdge = {x1: number; y1: number; x2: number; y2: number; outer: boolean};

/** Shared borders use the same endpoints as the cells; exposed edges form the outline. */
export function trackerCellEdges(cells: TrackerGridCell[], size = 17): TrackerGridEdge[] {
  const edges = new Map<string, TrackerGridEdge & {uses: number}>();
  const coordinate = (value: number) => Math.round(value * 1e6) / 1e6;
  const add = (x1: number, y1: number, x2: number, y2: number) => {
    [x1, y1, x2, y2] = [x1, y1, x2, y2].map(coordinate);
    const key = `${x1},${y1}:${x2},${y2}`;
    const edge = edges.get(key);
    if (edge) edge.uses++;
    else edges.set(key, {x1, y1, x2, y2, outer: true, uses: 1});
  };
  for (const {x, y} of cells) {
    add(x, y, x + size, y);
    add(x, y + size, x + size, y + size);
    add(x, y, x, y + size);
    add(x + size, y, x + size, y + size);
  }
  return [...edges.values()].map(({uses, ...edge}) => ({...edge, outer: uses === 1}));
}
