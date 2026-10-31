import copy,unittest
from scan import ScanBatch,compare
def row(cursor):return {'stage':6,'replayCursor':cursor,'replayActive':True,'scoreStored':100,'rng':[42,10],'piv':123000,'graze':42,'power':400,'playerFixed':[1,2],'bossLife':5,'bossPhaseAge':cursor,'spell':{'card':93,'flags':65,'age':cursor}}
class ScanTests(unittest.TestCase):
    def test_stops_on_earliest_difference_and_does_not_consume_later_rows(self):
        rows=[row(i) for i in range(11,15)];batch=ScanBatch(row(10),rows)
        self.assertFalse(batch.observe(row(11)))
        native=row(12);native['bossLife']=4
        self.assertTrue(batch.observe(native));self.assertEqual(batch.result['status'],'diverged')
        self.assertEqual(batch.result['firstDifference']['frame'],12);self.assertEqual(batch.cursor,12)
        self.assertEqual(batch.rows[0]['replayCursor'],12)
    def test_ignores_display_roll_and_native_spell_bookkeeping(self):
        a,b=row(10),row(10);a['displayScoreStored']=98;b['displayScoreStored']=100;b['spell']['flags']=1
        self.assertEqual(compare(a,b),{})
    def test_detects_phase_end_even_when_rng_and_score_match(self):
        a,b=row(10),row(10);b['spell']['flags']=0
        self.assertIn('spell',compare(a,b))
    def test_frame_gaps_fail_closed(self):
        with self.assertRaises(ValueError):ScanBatch(row(10),[row(12)])
        b=ScanBatch(row(10),[row(11)])
        with self.assertRaises(ValueError):b.observe(row(12))
    def test_menu_redraw_never_counts_as_advanced_tick(self):
        b=ScanBatch(row(10),[row(11)]);self.assertFalse(b.observe(row(10)));self.assertEqual(b.checked,0)
        self.assertTrue(b.observe(row(11)));self.assertEqual(b.result['status'],'batch-complete')
    def test_stage_change_and_missing_fields_are_errors(self):
        b=ScanBatch(row(10),[row(11)]);r=row(11);r['stage']=5
        with self.assertRaises(ValueError):b.observe(r)
        r=row(10);del r['bossLife']
        with self.assertRaises(ValueError):compare(r,row(10))
if __name__=='__main__':unittest.main()
