import 'game_file_archive.dart';
import 'l10n/strings.dart';

extension GameFileFeedback on GameFileError {
  String message(LauncherStrings strings) => switch (issue) {
    GameFileIssue.tooManyLocalSaves => strings.fileIssueTooManyLocalSaves,
    GameFileIssue.cannotReadLocal => strings.fileIssueCannotReadLocal,
    GameFileIssue.noLocalSaveFolder => strings.fileIssueNoLocalSaveFolder,
    GameFileIssue.noRouteHint => strings.fileIssueNoRouteHint,
    GameFileIssue.cloudSaveTooLarge => strings.fileIssueCloudSaveTooLarge,
    GameFileIssue.cloudSaveEmptyOrLarge =>
      strings.fileIssueCloudSaveEmptyOrLarge,
    GameFileIssue.incompleteSave => strings.fileIssueIncompleteSave,
    GameFileIssue.replaySlotsFull => strings.fileIssueReplaySlotsFull,
    GameFileIssue.replayUnsupported => strings.fileIssueReplayUnsupported,
    GameFileIssue.rollbackFailed => strings.fileIssueRollbackFailed,
    GameFileIssue.replayMissing => strings.fileIssueReplayMissing,
    GameFileIssue.nameTaken => strings.fileIssueNameTaken,
    GameFileIssue.keepReplayExtension => strings.fileIssueKeepReplayExtension,
    GameFileIssue.replayNameRequired => strings.fileIssueReplayNameRequired,
    GameFileIssue.saveNamesInvalid => strings.fileIssueSaveNamesInvalid,
    GameFileIssue.tooManySelectedSaves => strings.fileIssueTooManySelectedSaves,
    GameFileIssue.exportTooLarge => strings.fileIssueExportTooLarge,
    GameFileIssue.importTooLarge => strings.fileIssueImportTooLarge,
    GameFileIssue.wrongGameFormat => strings.fileIssueWrongGameFormat,
    GameFileIssue.invalidPath => strings.fileIssueInvalidPath,
    GameFileIssue.importNameConflict => strings.fileIssueImportNameConflict,
    GameFileIssue.wrongReplayName => strings.fileIssueWrongReplayName,
    GameFileIssue.importEmptyOrLarge => strings.fileIssueImportEmptyOrLarge,
    GameFileIssue.cloudFileEmptyOrLarge =>
      strings.fileIssueCloudFileEmptyOrLarge,
    GameFileIssue.incompleteFile => strings.fileIssueIncompleteFile,
    GameFileIssue.cannotReadReplayName => strings.fileIssueCannotReadReplayName,
    GameFileIssue.cannotReadArchive => strings.fileIssueCannotReadArchive,
    GameFileIssue.cannotReadCloudArchive =>
      strings.fileIssueCannotReadCloudArchive,
    GameFileIssue.archiveSpecialFile => strings.fileIssueArchiveSpecialFile,
    GameFileIssue.archiveInvalidPath => strings.fileIssueArchiveInvalidPath,
    GameFileIssue.archiveDuplicateFile => strings.fileIssueArchiveDuplicateFile,
    GameFileIssue.archiveTooManyFiles => strings.fileIssueArchiveTooManyFiles,
    GameFileIssue.archiveSizeInvalid => strings.fileIssueArchiveSizeInvalid,
    GameFileIssue.archiveMetadataMismatch =>
      strings.fileIssueArchiveMetadataMismatch,
    GameFileIssue.archiveNoUsableFiles => strings.fileIssueArchiveNoUsableFiles,
    GameFileIssue.restoreCancelled => strings.fileIssueRestoreCancelled,
    GameFileIssue.cloudSaveWrongFileFormat =>
      strings.fileIssueCloudSaveWrongFileFormat,
    GameFileIssue.cloudMainSaveMissing => strings.fileIssueCloudMainSaveMissing,
    GameFileIssue.cloudManifestMissing => strings.fileIssueCloudManifestMissing,
    GameFileIssue.cloudChecksumFailed => strings.fileIssueCloudChecksumFailed,
    GameFileIssue.cloudArchiveIncomplete =>
      strings.fileIssueCloudArchiveIncomplete,
    GameFileIssue.cloudManifestMismatch =>
      strings.fileIssueCloudManifestMismatch,
    GameFileIssue.cloudWrongGameOrLimit =>
      strings.fileIssueCloudWrongGameOrLimit,
    GameFileIssue.gameNotInCatalog => strings.fileIssueGameNotInCatalog,
  };
}
