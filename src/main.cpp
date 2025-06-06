#include <Geode/Geode.hpp>
#include <Geode/modify/MusicDownloadManager.hpp>
#include "button_setting.hpp"
#include "blacklisted_files.hpp"

using namespace geode::prelude;

class $modify(MusicDownloadManager) {

    gd::string pathForSongFolder(int p0) {
        std::filesystem::path customPath = Mod::get()->getSettingValue<std::filesystem::path>("custom-folder").string() + "\\";
        if (std::filesystem::exists(customPath)) return customPath.string();
        else return MusicDownloadManager::pathForSongFolder(p0);
    }

    gd::string pathForSFXFolder(int p0) {
        std::filesystem::path customPath = Mod::get()->getSettingValue<std::filesystem::path>("custom-folder").string() + "\\";
        if (std::filesystem::exists(customPath)) return customPath.string();
        else return MusicDownloadManager::pathForSFXFolder(p0);
    }
   
    bool customIsSongDownloaded(int id, std::filesystem::path songPath) {
        std::string songID = std::to_string(id) + ".mp3";

        if (!std::filesystem::is_directory(songPath)) return false;

        for (const auto& entry : std::filesystem::directory_iterator(songPath)) {
            if (entry.is_regular_file() && entry.path().filename() == songID && !blacklistedFiles.contains(entry.path().filename().string()))
                return true;
        }

        return false;
    }

    cocos2d::CCArray* getDownloadedSongs() {
        auto songs = MusicDownloadManager::getDownloadedSongs();
        std::filesystem::path songPath = Mod::get()->getSettingValue<std::filesystem::path>("custom-folder").string() + "\\";
        CCArray* newSongs = CCArray::create();

        CCObject* obj;
        CCARRAY_FOREACH(songs, obj) {
            SongInfoObject* info = static_cast<SongInfoObject*>(obj);
            if (info && customIsSongDownloaded(info->m_songID, songPath))
                newSongs->addObject(obj);
        }

        return newSongs;
    }
};