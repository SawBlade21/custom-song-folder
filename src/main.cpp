#include <Geode/Geode.hpp>
#include <Geode/modify/MusicDownloadManager.hpp>
#include "button_setting.hpp"
#include "blacklisted_files.hpp"

using namespace geode::prelude;

std::filesystem::path customPath = Mod::get()->getSettingValue<std::filesystem::path>("custom-folder").string() + slash;

$execute {
    geode::listenForSettingChanges("custom-folder", +[](std::filesystem::path  value) {
        customPath = value.string() + slash;
    });
};

class $modify(MusicDownloadManager) {

    gd::string pathForSongFolder(int p0) {
        if (std::filesystem::exists(customPath)) 
            return customPath.string();
        else
            return MusicDownloadManager::pathForSongFolder(p0);
    }

    gd::string pathForSFXFolder(int p0) {
        if (std::filesystem::exists(customPath)) 
            return customPath.string();
        else 
            return MusicDownloadManager::pathForSFXFolder(p0);
    }
   
    bool customIsSongDownloaded(std::string songID) {
        return (std::filesystem::exists(customPath / (songID + ".mp3")) || std::filesystem::exists(customPath / (songID + ".ogg")));
    }

    bool isSongDownloaded(int id) {
        return customIsSongDownloaded(std::to_string(id));
    }

    cocos2d::CCArray* getDownloadedSongs() {
        CCArray* newSongs = CCArray::create();

        for (SongInfoObject* info : CCArrayExt<SongInfoObject*>(MusicDownloadManager::getDownloadedSongs()))
            if (customIsSongDownloaded(std::to_string(info->m_songID)))
                newSongs->addObject(info);

        return newSongs;
    }
};