export function validTh15ChapterThreshold(value) {
    return typeof value === 'number' && Number.isInteger(value) && value >= 500000 && value <= 5000000 && value % 250000 === 0;
}
export const defaultTh15ChapterThreshold = (players) => players === 3 ? 1500000 : 1250000;
