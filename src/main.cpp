#include <Geode/Geode.hpp>
#include <Geode/modify/MusicDownloadManager.hpp>
#include "button_setting.hpp"
#include "blacklisted_files.hpp"

using namespace geode::prelude;

class $modify(MusicDownloadManager) {

    gd::string pathForSongFolder(int p0) {
        log::debug("pathForSongFolder()");
        std::filesystem::path customPath = Mod::get()->getSettingValue<std::filesystem::path>("custom-folder").string() + "\\";
        if (std::filesystem::exists(customPath)) return customPath.string();
        else return MusicDownloadManager::pathForSongFolder(p0);
    }

    gd::string pathForSFXFolder(int p0) {
        log::debug("pathForSFXFolder()");
        std::filesystem::path customPath = Mod::get()->getSettingValue<std::filesystem::path>("custom-folder").string() + "\\";
        if (std::filesystem::exists(customPath)) return customPath.string();
        else return MusicDownloadManager::pathForSFXFolder(p0);
    }

    gd::string pathForSong(int p0) {
        log::debug("pathForSong()");
        return MusicDownloadManager::pathForSong(p0);
    }

    gd::string pathForSFX(int p0) {
        log::debug("pathForSFX()");
        return MusicDownloadManager::pathForSFX(p0);
    }
   
    bool customIsSongDownloaded(int id, std::filesystem::path songPath) {
        
        if (!std::filesystem::is_directory(songPath)) return false;

        std::string songID = std::to_string(id);

        log::debug("customIsSongDownloaded()");
        
        for (const auto& entry : std::filesystem::directory_iterator(songPath)) {

            std::string name = entry.path().filename().string();
            std::string ext = entry.path().extension().string();

            if (entry.is_regular_file() && (ext == ".mp3" || ext == ".ogg") && name == (songID + ext) && !blacklistedFiles.contains(name))
                return true;
        }

        return false;
    }

    cocos2d::CCArray* getDownloadedSongs() {
        log::debug("getDownloadedSongs()");
        auto songs = MusicDownloadManager::getDownloadedSongs();
        std::filesystem::path songPath = Mod::get()->getSettingValue<std::filesystem::path>("custom-folder").string() + "\\";
        CCArray* newSongs = CCArray::create();

        CCObject* obj;
        CCARRAY_FOREACH(songs, obj) {
            SongInfoObject* info = static_cast<SongInfoObject*>(obj);
            log::debug("songID: {}", info->m_songID);
            if (info && customIsSongDownloaded(info->m_songID, songPath)) {
                log::debug("exists: {}", info->m_songID);
                newSongs->addObject(obj);
            }
        }

        return newSongs;
    }
};