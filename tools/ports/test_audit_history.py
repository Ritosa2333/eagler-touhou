"""Exercise patch correspondence across cherry-picks, splits and reversions."""
import importlib.util
from pathlib import Path
import subprocess
import tempfile
import unittest


spec = importlib.util.spec_from_file_location("audit_history", Path(__file__).with_name("audit-history.py"))
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


class HistoryAuditTests(unittest.TestCase):
    def run_git(self, *args):
        return subprocess.check_output(["git", "-C", str(self.repo), *args], stderr=subprocess.PIPE).decode().strip()

    def commit(self, message, **files):
        for name, value in files.items():
            (self.repo / name).write_text(value, encoding="utf-8")
        self.run_git("add", ".")
        self.run_git("-c", "commit.gpgsign=false", "commit", "-m", message)
        return self.run_git("rev-parse", "HEAD")

    def test_cherry_pick_split_and_revert_keep_evidence_separate_from_acceptance(self):
        with tempfile.TemporaryDirectory(prefix="port-history-audit-") as directory:
            self.repo = Path(directory)
            self.run_git("init", "-b", "main")
            self.run_git("config", "user.name", "历史测试")
            self.run_git("config", "user.email", "test@example.invalid")
            self.run_git("config", "color.ui", "always")
            combined = "".join("line " + str(index) + "\n" for index in range(30))
            self.commit("共同基线", first="base\n", second="base\n", audio="base\n", mode="base\n", combined=combined, historical="base\n")
            inherited = self.commit("分叉前普通版修改", historical="shared implementation\n")
            self.run_git("branch", "port-history/th10mp")
            self.run_git("switch", "-c", "port-history/th10")
            split = self.commit("组合修改", first="updated\n", second="updated\n")
            hunk_source = self.commit("同时修改两块代码", combined=combined.replace("line 1\n", "updated first\n").replace("line 25\n", "updated last\n"))
            exact = self.commit("降低音效延迟", audio="low latency\n")
            adapted = self.commit("适配模式", mode="single player\n")
            self.run_git("switch", "port-history/th10mp")
            self.commit("暂时回退旧修改", historical="base\n")
            copied_ancestor = self.commit("重新迁入旧普通版实现", historical="shared implementation\n")
            part1 = self.commit("迁入第一项", first="updated\n")
            part2 = self.commit("迁入第二项", second="updated\n")
            hunk_target = self.commit("合入一部分并加入多人修改", combined=combined.replace("line 1\n", "updated first\n"), mode="mp extra\n")
            copied = self.commit("同步普通版音效", audio="low latency\n")
            self.commit("撤销音效调整", audio="base\n")
            mp_adapted = self.commit("适配模式", mode="multiplayer\n")
            before = self.run_git("show-ref")
            result = module.audit(self.repo, "th10")
            self.assertEqual(self.run_git("show-ref"), before)
            rows = {row["commit"]: row for row in result["normal"]}
            self.assertEqual(rows[exact]["evidence"], "patch-equivalent")
            self.assertEqual(rows[exact]["equivalentCommits"], [copied])
            self.assertEqual(rows[exact]["currentState"]["counts"], {"text-differs": 1})
            mp_rows = {row["commit"]: row for row in result["mp"]}
            self.assertEqual(mp_rows[copied_ancestor]["equivalentCommits"], [inherited])
            self.assertEqual(rows[split]["evidence"], "file-patches-covered")
            self.assertEqual(rows[split]["fileMatches"], {"first": [part1], "second": [part2]})
            self.assertEqual(rows[hunk_source]["evidence"], "partial-hunk-patches")
            self.assertEqual(rows[hunk_source]["matchedHunks"], 1)
            self.assertEqual(rows[hunk_source]["totalHunks"], 2)
            self.assertEqual(rows[hunk_source]["hunkMatches"]["combined"][0]["commits"], [hunk_target])
            self.assertEqual(rows[adapted]["evidence"], "related-candidate")
            self.assertEqual(rows[adapted]["sameSubject"], [mp_adapted])
            self.assertEqual(self.run_git("show", "port-history/th10mp:audio"), "base")
            # A historical match deliberately does not claim current acceptance.
            self.assertNotIn("accepted", rows[exact])
            report = {"games": [result]}
            self.assertIn("后续撤销", module.markdown(report))
            decisions = {"games": [{"game": "th10", "reviewedHeads": dict(result["heads"]), "records": [
                {"normalCommit": split, "status": "received", "summary": "拆分迁入", "pendingScopes": [],
                 "correspondingMpCommits": [part1, part2]}]}]}
            module.attach_decisions(self.repo, report, decisions)
            self.assertTrue(rows[split]["review"]["snapshotMatches"])
            decisions["games"][0]["reviewedHeads"]["normal"] = inherited
            module.attach_decisions(self.repo, report, decisions)
            self.assertFalse(rows[split]["review"]["snapshotMatches"])
            decisions["games"][0]["records"][0]["status"] = "partial"
            with self.assertRaisesRegex(RuntimeError, "remaining scopes"):
                module.attach_decisions(self.repo, report, decisions)
            decisions["games"][0]["records"][0]["status"] = "received"
            self.run_git("switch", "port-history/th10")
            new_source = self.commit("下一轮普通版修改", newly_added="next round\n")
            next_report = {"games": [module.audit(self.repo, "th10")]}
            module.attach_decisions(self.repo, next_report, decisions)
            new_rows = {row["commit"]: row for row in next_report["games"][0]["normal"]}
            self.assertNotIn("review", new_rows[new_source])
            self.assertFalse(new_rows[split]["review"]["snapshotMatches"])
            self.assertEqual(next_report["games"][0]["reviewSummary"]["reviewed"], 1)
            pending = {"normalCommit": new_source, "status": "pending", "summary": "新增文件尚未迁入", "pendingScopes": ["迁入新文件"],
                       "correspondingMpCommits": [], "pendingEvidence": [{"sourcePath": "newly_added", "addedContains": ["next round"],
                                                                         "targetPath": "newly_added", "targetPathAbsent": True}]}
            decisions["games"][0]["records"].append(pending)
            mp_review = {"mpCommit": copied_ancestor, "classification": "normal-equivalent", "summary": "旧普通版补丁对应",
                         "normalCounterparts": [inherited], "scopeFiles": ["historical"], "changedFileCount": 1}
            decisions["games"][0]["mpRecords"] = [mp_review]
            module.attach_decisions(self.repo, next_report, decisions)
            self.assertEqual(next_report["games"][0]["mpReviewSummary"]["reviewed"], 1)
            pending["pendingEvidence"][0]["addedContains"] = ["not in this source delta"]
            with self.assertRaisesRegex(RuntimeError, "not in the source delta"):
                module.attach_decisions(self.repo, next_report, decisions)
            pending["pendingEvidence"][0]["addedContains"] = ["next round"]
            mp_review["normalCounterparts"] = [new_source]
            with self.assertRaisesRegex(RuntimeError, "lacks patch evidence"):
                module.attach_decisions(self.repo, next_report, decisions)
            mp_review["normalCounterparts"] = [inherited]
            mp_head = next_report["games"][0]["heads"]["mp"]
            merged = self.run_git("-c", "commit.gpgsign=false", "commit-tree", new_source + "^{tree}", "-p", new_source, "-p", mp_head, "-m", "保留测试合并关系")
            self.run_git("update-ref", "refs/heads/port-history/th10", merged)
            merge_report = {"games": [module.audit(self.repo, "th10")]}
            self.assertEqual(len(merge_report["games"][0]["merges"]), 1)
            merge = merge_report["games"][0]["merges"][0]
            decisions["games"][0]["mergeRecords"] = [merge | {"summary": "已核对父链和冲突解决范围"}]
            module.attach_decisions(self.repo, merge_report, decisions)
            self.assertEqual(merge_report["games"][0]["mergeReviewSummary"]["reviewed"], 1)


if __name__ == "__main__":
    unittest.main()
