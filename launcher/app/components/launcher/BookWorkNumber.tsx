import './book-work-number.css';

export function BookWorkNumber({gameId,number}:{gameId:string;number:string}){
  return <small className="book-work-number" data-work-number={gameId}>th{number}</small>;
}
