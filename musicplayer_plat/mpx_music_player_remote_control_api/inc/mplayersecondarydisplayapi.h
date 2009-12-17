/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Note IDs and other definitions for secondary displays
*
*/


#ifndef MPLAYERSECONDARYDISPLAYAPI_H
#define MPLAYERSECONDARYDISPLAYAPI_H

/**
* Category value used for Music Player notes
*/
const TUid KMPlayerNoteCategory = {0x102072C3};

/**
* Note IDs for notes Music Player publishes for secondary display
* implementations.
*
* \%U parameters are passed as 16-bit descriptors and \%N parameters as 32-bit
* signed integers in the data buffer given to mediator command observers.
*
* @note The \% character is escaped in comments to make sure documentation
* parsers parse this file correctly.
*
* @see CMediatorCommandResponder
* @since S60 3.2.3
*/
enum TMPlayerSecondaryDisplayNote
    {
    /**
     * Not used.
     */
    EMPlayerNoteNone = 0,

    /**
     * Confirmation note after saving a clip or a playlist.
     * - \%U is the title of the saved track or playlist
     *
     * \em qtn_mus_note_saved_to_collection
     * "%U saved to Music Library"
     */
    EMPlayerNoteClipSaved,

    /**
     * Error note when attempting to play an invalid or corrupted clip.
     *
     * \em qtn_mp_note_corrupt_file
     * "File is corrupt. Operation cancelled."
     */
    EMPlayerNoteInvalidClip,

    /**
     * Error note when attempting to play an invalid or corrupted playlist.
     *
     * \em qtn_nmp_note_open_broken_playlist
     * "Playlist file cannot be found. Unable to open."
     */
    EMPlayerNoteInvalidPlaylist,

    /**
     * Error note when attempting to play an empty playlist.
     *
     * \em qtn_nmp_note_empty_playlist
     * "Playlist is empty.\nNothing to play."
     */
    EMPlayerNoteEmptyPlaylist,

    /**
     * Error note when attempting to play a category of tracks and none of the
     * tracks are playable.
     *
     * \em qtn_nmp_note_invalid_list
     * "Unable to play selection. Operation cancelled."
     */
    EMPlayerNoteInvalidGroup,

    /**
     * Wait note shown when Music Library DB is created after starting
     * Music Player for the first time.
     *
     * \em qtn_mus_note_searching
     * "Searching for music."
     */
    EMPlayerNoteCreatingDb,

    /**
     * Information note shown after Music Library DB was was created but
     * no tracks were found.
     *
     * \em qtn_mus_note_search_complete_no_tracks1
     * "Search complete\nNothing found"
     */
    EMPlayerNoteDbCreationCompleteEmpty,

    /**
     * Information note shown after Music Library DB was created and
     * some tracks were found.
     * - \%N is the number of (new) tracks
     *
     * \em qtn_mus_note_search_complete1
     * "Search complete\nSongs: %0N\nPlaylists: %1N"
     */
    EMPlayerNoteDbCreationComplete,

    /**
     * Wait note shown when updating Music Library DB
     * - \%0N is the number of added tracks
     * - \%1N is the number of removed tracks
     *
     * \em qtn_mus_note_updating_db1
     * "Refreshing\nAdded: %0N"
     */
    EMPlayerNoteUpdatingDb,

    /**
     * Information note shown when updating Music Library DB is complete.
     * - \%0N is the number of added tracks
     * - \%1N is the number of removed tracks
     *
     * \em qtn_mus_note_updating_db_complete1
     * "Refresh complete\nAdded: %0N"
     */
    EMPlayerNoteDbUpdateComplete,

    /**
     * Information note shown when updating Music Library DB was stopped
     * by the user.
     * - \%0N is the number of added tracks
     * - \%1N is the number of removed tracks
     *
     * \em qtn_mus_note_updating_db_stopped1
     * "Refresh stopped\nAdded: %0N"
     */
    EMPlayerNoteDbUpdateStopped,

    /**
     * Wait note for updating Music Library DB.
     * No track counts are shown.
     *
     * \em qtn_mus_note_updating_db_short
     * "Updating Music Library."
     */
    EMPlayerNoteUpdatingDbShort,

    /**
     * Information note shown when updating Music Library DB is complete.
     * No track counts are shown.
     *
     * \em qtn_mus_note_update_complete_unmod
     * "Update complete."
     */
    EMPlayerNoteDbUpdateCompleteShort,

    /**
     * Information note shown when updating Music Library DB was stopped
     * by the user. No track counts are shown.
     *
     * \em qtn_mus_note_updating_db_stopped_short
     * "Update stopped."
     */
    EMPlayerNoteDbUpdateStoppedShort,

    /**
     * Error note shown when there is not enough free space on memory card
     * to add all tracks to Music Library DB.
     *
     * \em qtn_mus_note_memory_card_full
     * "Not enough memory to add tracks from memory card to Music Library..."
     */
    EMPlayerNoteDbUpdateDiscFull,

    /**
     * Information note shown when playback had to be paused in progressive
     * download, because all downloaded data has been played.
     *
     * \em qtn_mus_note_played_all_dl_content
     * "All downloaded content has been played."
     */
    EMPlayerNoteAllDlContentPlayed,

    /**
     * Information note shown when attempting to use Music Player during
     * a video call.
     *
     * \em qtn_mus_info_video_call_ongoing
     * "Music Player cannot be used during video call."
     */
    EMPlayerNoteVideoCall,

    /**
     * Note shown when attempting to play a non-protected track when
     * KRequireDRMInPlayback is enabled.
     *
     * \em qtn_mp_info_tone_no_drm
     * "Unprotected file. Cannot be played."
     */
    EMPlayerNoteCannotPlayUnprotected,

    /**
     * Wait note shown when saving a track in embedded mode.
     *
     * \em qtn_gen_note_saving
     * "Saving"
     */
    EMPlayerNoteSaving,

    /**
     * Query shown when attempting to exit Music Player without saving
     * a downloaded/received track/playlist in embedded mode.
     * - \%U is the title of the track or the name of the playlist
     *
     * \em qtn_iv_query_conf_save_downloaded
     * "%U has not been saved. Save now?"
     */
    EMPlayerQueryNotSaved,

    /**
     * Wait note when deleting either a single track or a category.
     * - \%U is the title of the track or the name of the category
     *
     * \em qtn_album_waiting_deleting1 , qtn_mus_query_conf_delete_all1
     * "Deleting '%U'"
     * "Deleting songs belonging to '%U'"
     */
    EMPlayerNoteDeletingSingle,

    /**
     * when deleting several marked tracks.
     *
     * \em qtn_nmp_del_songs_wait_note
     * "Deleting songs"
     */
    EMPlayerNoteDeletingMany,

    /**
     * Confirmation query shown when attempting to delete a category.
     *
     * \em qtn_nmp_query_conf_delete_group
     * "Delete all songs belonging to '%U'?"
     */
    EMPlayerQueryDeleteCategory,

    /**
     * Confirmation query shown when attempting to delete a single track or
     * a playlist.
     *
     * \em qtn_query_common_conf_delete
     * "Delete?\n%U"
     */
    EMPlayerQueryDeleteTrack,

    /**
     * Confirmation query shown when attempting to delete several marked
     * tracks.
     * - \%N is the number of tracks to be deleted
     *
     * \em qtn_nmp_del_songs_query
     * "Delete %N songs?"
     */
    EMPlayerQueryDeleteTracks,

    /**
     * Confirmation query shown when attempting to all tracks in a category
     * to Now Playing List.
     * - \%U is the name of the category
     *
     * \em qtn_mus_query_conf_add_all_to_playing_pl
     * "Add all tracks belonging to %U to Now Playing?"
     */
    EMPlayerQueryAddAllToNowPlaying,

    /**
     * Confirmation query shown when attempting to all tracks in a category
     * to a new playlist.
     * - \%U is the name of the category
     *
     * \em qtn_mus_query_conf_add_all_to_new_pl
     * "Create new playlist of all tracks belonging to %U?"
     */
    EMPlayerQueryAddAllToNewPl,

    /**
     * Confirmation query shown when attempting to all tracks in a category
     * to an existing playlist.
     * - \%U is the name of the category
     *
     * \em qtn_mus_query_conf_add_all_to_pl
     * "Add all tracks belonging to %U to a playlist?"
     */
    EMPlayerQueryAddAllToSavedPl,

    /**
     * Confirmation query shown when attempting to remove several tracks
     * from a playlist.
     * - \%N is the number of tracks to be removed
     *
     * \em qtn_mus_query_conf_remove_many1
     * "Remove %N songs from the playlist?"
     */
    EMPlayerQueryRemoveTracks,

    /**
     * Confirmation query shown when attempting to remove a single track
     * from a playlist.
     * - \%U is the name of the track to be removed
     *
     * \em qtn_mus_query_conf_remove_track
     * "Remove %U from the playlist?"
     */
    EMPlayerQueryRemoveTrack,

    /**
     * Confirmation query shown in embedded mode when setting a track
     * as ringing tone and it has not yet been saved.
     *
     * \em qtn_mp_query_embd_rtone_change_for_all
     * "Tone will be saved to Gallery and changed for all profiles. Continue?"
     */
    EMPlayerQuerySaveAndSetAsRt,

    /**
     * Confirmation query shown when setting a track as ringing tone.
     *
     * \em qtn_mg_query_rtone_change_for_all
     * "Ringing tone will be changed for all profiles. Change?"
     */
    EMPlayerQuerySetAsRt,

    /**
     * Wait note shown when copying tracks/playlists using the organize
     * function.
     *
     * \em qtn_mg_copying_many_files
     * "Copying files"
     */
    EMPlayerNoteCopyingTracks,

    /**
     * Wait note shown when copying a single track or playlist using the
     * organize function.
     * - \%U is the title of the track or name of the playlist
     *
     * \em qtn_mg_waiting_copy
     * "Copying %U"
     */
    EMPlayerNoteCopyingTrack,

    /**
     * Wait note shown when moving tracks/playlists using the organize
     * function.
     *
     * \em qtn_mg_moving_many_files
     * "Moving files"
     */
    EMPlayerNoteMovingTracks,

    /**
     * Wait note shown when moving a single track or playlist using the
     * organize function.
     * - \%U is the title of the track or name of the playlist
     *
     * \em qtn_album_waiting_move
     * "Moving %U"
     */
    EMPlayerNoteMovingTrack,

    /**
     * List query shown when choosing a playlist to add tracks to.
     * List contents are not available for secondary displays.
     *
     * \em qtn_mus_title_select_pl
     * "Select playlist:"
     */
    EMPlayerQuerySelectPlaylist,

    /**
     * List query shown when choosing visualization plug-in.
     *
     * \em qtn_mus_title_select_visualization
     * "Select visualization:"
     */
    EMPlayerQuerySelectVisualization,

    /**
     * Global note shown when running out of disk space when saving, moving,
     * or copying files.
     * - Data buffer contains the drive letter of drive in question
     *
     * \em qtn_memlo_mmc_not_enough_memory or \em qtn_memlo_not_enough_memory
     */
    EMPlayerNoteDiskFull,

    /**
    * Wait Note when MMC card becomes anavailable.
    * \em qtn_nmp_note_mmc_unavailable
    * "Memory card no longer available. Updating music library."
    */
    EMPlayerNoteMMCUnavailableWaitNote,

    /**
    * Wait note when opening album art.
    * \em qtn_nmp_note_opening
    * "Opening..."
    */
    EMPlayerNoteOpeningAlbumArt,

    /**
    * Wait note for opening cache.
    * \em qtn_nmp_note_opening_db
    * "Opening..."
    */
    EMPlayerNoteOpeningDb,

    /**
    * Wait note for scan cancelled opening cache.
    * \em qtn_nmp_note_cancelled_db
    * "Stopping search..."
    */
    EMPlayerNoteCancelledDbWaitNote,

    /**
    * Wait note for completing scan opening cache.
    * \em qtn_nmp_note_completing_db
    * "Completing search..."
    */
    EMPlayerNoteCompletingDb,

    /**
    * Wait note text when renaming a category with one item.
    * \em qtn_nmp_note_updating_name1
    * "Updating 1 song"
    */
    EMPlayerNoteUpdateOneItem,

    /**
    * Wait note text when renaming a category.
    * \em qtn_nmp_note_updating_name
    * "Updating %N songs"
    */
    EMPlayerNoteUpdateManyItems,

    /**
    * Wait note text when a corrupt db has been detected and collection
    *is attempting to rebuild the db
    * \em qtn_nmp_note_rebuilding_db
    * "Repairing corrupt library. Searching for songs."
    */
    EMPlayerNoteRebuildingDb,
     /**
    * Wait note used in wait note while saving an item.
    * \em qtn_nmp_wait_save_one_item
    * "Saving %U"
    */
    EMPlayerNoteSaveOneItem,

    /**
    * Wait Note used when remove one track.
    * \em qtn_mus_note_removing_track1
    * "Removing '%U'"
    */
    EMPlayerNoteRemovingTrack,

    /**
    * Wait note used when remove multiple tracks.
    * \em qtn_nmp_note_removing_many
    * "Removing songs"
    */
    EMPlayerNoteRemovingMany,

    /**
    * Wait note for adding album art.
    * \em qtn_nmp_note_adding_album_art
    * "Changing album art"
    */
    EMPlayerNoteAddingAlbumArt,

    /**
    * Wait note for adding one song.
    * \em qtn_nmp_note_adding_one_song
    * "Adding song"
    */
    EMPlayerNoteAddingSong,

    /**
    * Wait note for adding many songs.
    * \em qtn_mus_note_adding_tracks1
    * "Adding songs"
    */
    EMPlayerNoteAddingManySongs,

    /**
    * Wait note for restoring arlbum art.
    * \em qtn_nmp_note_restoring_album_art
    * "Restoring album art"
    */
    EMPlayerNoteRestoreAlbumArt,

    /**
    * Confirmation query when MMC card is inserted into the phone.
    * \em qtn_nmp_note_mmc_inserted
    * "New memory card detected. Update music library?"
    */
    EMPlayerNoteMMCInserted,

    /**
    * Confirmation query after a MTP sync has occured.
    * \em qtn_nmp_note_library_refresh_sync
    * "Music may need to be refreshed due to recent USB sync. Refresh now?"
    */
    EMPlayerNoteUSBSyncRefresh,

    /**
    * Confirmation query when sending invalid songs.
    * \em qtn_nmp_query_send_valid_songs1
    * "Some songs cannot be sent. Continue?"
    */
    EMPlayerNoteSendInvalidSongs,

    /**
    * Confirmation note text when ringtone is set.
    * \em qtn_nmp_note_ringtone_set
    * "%U set as ringing tone for active profile"
    */
    EMPlayerRingtoneSet,

    /**
    * Confirmation query used when clip isn't supported by collection.
    * \em qtn_nmp_note_saved_to_gallery
    * "Saved to Media Gallery"
    */
    EMPlayerNoteSavedtoGallery,

    /**
    * Confirmation query used in note when save is ok.
    * \em qtn_nmp_note_saved_to_collection
    * "Saved to Music Library"
    */
    EMPlayerNoteSavedtoCollection,
    /**
    * Confirmation query for adding Album Art.
    * \em qtn_nmp_query_add_art_to_album
    * "Album art for all songs in album will be changed. Continue?"
    */
    EMPlayerNoteAddArtToAlbum,

    /**
    * Confirmation query for adding Songs.
    * \em qtn_nmp_query_add_songs
    * "Add songs now?"
    */
    EMPlayerNoteAddSongs,

    /**
    * Confirmation query for canceling download.
    * \em qtn_musicshop_note_cancel
    * "Unable to resume download after cancelling. Cancel anyway?"
    */
    EMPlayerNoteMusicshopCancel,

    /**
    * Confirmation note for original arlbum art restored.
    * \em qtn_nmp_note_art_restored
    * "Original album art restored"
    */
    EMPlayerNoteArtRestored,

    /**
    * Confirmation note for invalid changing album art.
    * \em qtn_nmp_note_change_with_invalid
    * "Album art for some songs could not be changed"
    */
    EMPlayerNoteAlbumArtChangeInvalid,

    /**
    * Confirmation note for invalid updating songs name.
    * \em qtn_nmp_note_rename_with_invalid
    * "Some songs could not be updated with %U name"
    */
    EMPlayerNoteRenameInvalid,

    /**
    * Confirmation qurey for restoring album art.
    * \em qtn_nmp_query_restore_art_to_album
    * "Original art for all songs in album will be restored. Continue?"
    */
    EMPlayerNoteRestoreArtToAlbum,

    /**
    * Confirmation query for drm not alllowed.
    * \em qtn_nmp_note_art_changed
    * "Album art changed"
    */
    EMPlayerNoteAlbumArtChanged,

    /**
    * Confirmation query for adding single song to playlist.
    * \em qtn_mus_note_track_added_to_pl1
    * "Song added to %U"
    */
    EMPlayerNoteAddSongToPlaylist,

    /**
    * Confirmation query for adding many songs to playlist.
    * \em qtn_mus_multiple_tracks_added_to_pl1
    * "%N songs added to %U"
    */
    EMPlayerNoteAddManySongToPlaylist,

    /**
    * Confirmation query for adding many songs to playlist.
    * \em qtn_sm_all_serv_failed
    * "Saving failed "
    */
    EMPlayerNoteSavingFailed,

    /**
    * Warning Note to warn the user that we wer unable to re-create the DB
    * after a DB corrupted event has been detected.
    * \em qtn_nmp_note_out_of_disk_error
    * "Not enough memory to add songs to library. Delete some data and refresh".
    */
    EMPlayerNoteCannotCreateDB,

    /**
    * Warning note for adding many songs to playlist.
    * \em qtn_nmp_exp_warn_n_counts
    * "Rights for %U will expire after %N uses. Renew license on PC."
    */
    EMPlayerNoteDrmExpNUse,

    /**
    * Information note after a MTP sync has occured.
    * \em qtn_nmp_note_library_updated_sync
    * "Music library was updated during Media Player Sync"
    */
    EMPlayerNoteMTPSyncInfo,

    /**
    * Information note when not enough memory to perform specified operation.
    * \em qtn_nmp_memlo_not_enough_memory
    * "Not enough memory to complete operation. Delete some data first"
    */
    EMPlayerNoteNotEnoughMemoryNotification,

    /**
    * Information note when making changes to items that are currently playing.
    * \em qtn_nmp_note_changes_take_effect
    * "Changes will take effect after %U stopped playing"
    *
    */
    EMPlayerNoteMakingChangesInformationNote,

    /**
    * Information note for db scan stopped and songs found.
    * \em qtn_mus_note_search_stopped1
    * "Search stopped\nSongs: %0N\nPlaylists: %1N"
    */
    EMPlayerNoteDbSearchStoppedItemFound,

    /**
    * Information note for db scan stopped and no song found.
    * \em qtn_mus_note_search_stopped_no_tracks1
    * "Search stopped\nNothing found"
    */
    EMPlayerNoteDbSearchStoppedNothingFound,

    /**
    * Information note used to display file deletion failed.
    * \em qtn_nmp_delete_file_open
    * "Unable to delete song. It is currently in use."
    */
    EMPlayerDeleteFileIsOpened,

    /**
    * Information note used to display multiple file deletion failed
    * \em qtn_nmp_delete_mult_file_open
    * "Unable to delete some songs. They are currently in use."
    */
    EMPlayerDeleteMultFileIsOpened,

    /**
    * Information note when cannot play DRM music due to no rights.
    * \em qtn_nmp_note_drm_expired_playback
    * "Rights are expired. Unable to play %U"
    */
    EMPlayerNoteDrmExpiredPlayback,

    /**
    * Information note when all songs to be sent are invalid.
    * \em qtn_nmp_note_nothing_to_send
    * "Songs cannot be found. Nothing to send."
    */
    EMPlayerNoteSendAllInvalidSongs,

    /**
    * Information note when not enough memory to perform a refresh.
    * \em qtn_nmp_note_no_mem_complete_refresh
    * "Not enough memory to add more music to the library.
    * Delete some data first"
    */
    EMPlayerNoteCannotCompleteRefresh,

    /**
    * Information note for unable to play DRM protected music over bluetooth.
    * \em qtn_nmp_note_protected_over_bluetooth
    * "Song is protected. Unable to play over Bluetooth headset".
    */
    EMPlayerNoteNoDRMOverBT,

    /**
    * Information note when attempting to perform an operation on broken file.
    * \em qtn_mp_note_broken_file
    * "File cannot be found. Operation cancelled."
    */
    EMPlayerBrokenfile,

    /**
    * Information note when attempting to play an invalid playlist.
    * \em qtn_nmp_note_play_broken_file
    * "Playlist file cannot be found. Unable to play."
    */
    EMPlayerNoteInvalidFile,

    /**
    * Information note when WMDRM rights activation key has one count remaining.
    * \em qtn_nmp_exp_warn_1_count
    * "Rights for %U will expire in 1 use. Renew license on PC."
    */
    EMPlayerNoteWDrmExpOneUse,

    /**
    * Information note when WMDRM rights are about to expire in one day.
    * \em qtn_nmp_exp_warn_1_day
    * "Rights for %U will expire in 1 day.  Renew license on PC."
    */
    EMPlayerNoteWDrmExpOneDay,

    /**
    * Information note when WMDRM rights are about to expire.
    * \em qtn_nmp_exp_warn_today
    * "Rights for %U will expire today.  Renew license on PC."
    */
    EMPlayerNoteWDrmExpToday,

    /**
    * Information note when WMDRM rights are about to expire in multiple days.
    * \em qtn_nmp_exp_warn_n_days
    * "Rights for %U will expire after %N uses. Renew license on PC."
    */
    EMPlayerNoteWDrmExpNDays,

    /**
    * Information note when WMDRM rights have expired.
    * \em qtn_nmp_license_exp
    * "Unable to play song.  Rights are invalid or expired.  Renew rights on PC."
    */
    EMPlayerNoteWDrmLicenseExp,

    /**
    * Information note for drm not alllowed.
    * \em qtn_mp_drm_not_allowed
    * "Unable to select a copyright protected item."
    */
    EMPlayerNoteDrmNotAllowed

// Cover UI start
    ,
    /**
    * Query for a new playlist: should tracks be added to it or not?
    * \em qtn_nmp_query_add_songs
    * "Add songs now?"
    */
    EMPlayerQueryAddSongsToPlaylist,

    /**
    * Query whether the selected track should be moved to another library
    * \em qtn_nmp_query_move_to_library
    * "Item will be moved to %U library. Some details may be lost. Continue?"
    */
    EMPlayerQueryMoveToLibrary,

    /**
    * Generic confirmation query. Text can vary, but dialog is modal.
    * "%U"
    */
    EMPlayerQueryConfirmationGeneric,

    /**
    * Query for name for a playlist to be created.
    * \em qtn_mp_enter_pl_name
    * "Playlist name:"
    */
    EMPlayerQueryCreatePlaylist,

    /**
    * Query for new name for an item.
    * \em qtn_nmp_prmpt_new_name
    * "New name:"
    */
    EMPlayerQueryPrmptNewName,

    /**
    * Progress dialog for USB connection in progress.
    * \em qtn_nmp_note_usb_connection_in_progress
    * "USB connection in progress"
    */
    EMPlayerUSBConnectionInProgress,


    /**
    * Progress dialog for USB connection in progress.
    * \em qtn_nmp_note_usb_connection_in_progress
    * "USB connection in progress"
    */
    EMPlayerMTPConnectionInProgress,

    /**
    * Progress dialog for formatting wait note.
    * \em qtn_nmp_note_mp_unavailable_mmc
    * "Music Player unable to access memory card. Memory card operation underway."
    */
    EMPlayerFormatting

// Cover UI end
    };
#endif // MPLAYERSECONDARYDISPLAYAPI_H
