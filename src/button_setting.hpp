#include <Geode/loader/SettingV3.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/loader/Dirs.hpp>

using namespace geode::prelude;

class MyButtonSettingV3 : public SettingV3 {
public:
    static Result<std::shared_ptr<SettingV3>> parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
        auto res = std::make_shared<MyButtonSettingV3>();
        auto root = checkJson(json, "MyButtonSettingV3");

        res->init(key, modID, root);
        res->parseNameAndDescription(root);
        res->parseEnableIf(root);
        
        root.checkUnknownKeys();
        return root.ok(std::static_pointer_cast<SettingV3>(res));
    }

    bool load(matjson::Value const& json) override {
        return true;
    }
    bool save(matjson::Value& json) const override {
        return true;
    }

    bool isDefaultValue() const override {
        return true;
    }
    void reset() override {}

    SettingNodeV3* createNode(float width) override;
};

class MyButtonSettingNodeV3 : public SettingNodeV3 {
protected:
    ButtonSprite* m_buttonSprite;
    CCMenuItemSpriteExtra* m_button;

    bool init(std::shared_ptr<MyButtonSettingV3> setting, float width) {
        if (!SettingNodeV3::init(setting, width))
            return false;
        
        m_buttonSprite = ButtonSprite::create("Automigrate", "goldFont.fnt", "GJ_button_01.png", .8f);
        m_buttonSprite->setScale(.5f);
        m_button = CCMenuItemSpriteExtra::create(
            m_buttonSprite, this, menu_selector(MyButtonSettingNodeV3::onButton)
        );

        this->getButtonMenu()->addChildAtPosition(m_button, Anchor::Center);
        this->getButtonMenu()->updateLayout();

        this->updateState(nullptr);
        
        return true;
    }
    
    void updateState(CCNode* invoker) override {
        SettingNodeV3::updateState(invoker);
    }

    void onButton(CCObject*) {

        std::filesystem::path oldDir = Mod::get()->getSettingValue<std::filesystem::path>("migrate-folder");
        std::filesystem::path newDir = Mod::get()->getSettingValue<std::filesystem::path>("custom-folder");
        std::string status = "No songs or SFX were found.";
        int moved = 0;
        
        if (oldDir == newDir) 
            status = "No files were transferred because the source and destination directories are the same.";
        else {
            bool dirsExist[2] = {std::filesystem::exists(oldDir), std::filesystem::exists(newDir)};
            if (dirsExist[0] && dirsExist[1]) {
                try {
                        for (const auto& entry : std::filesystem::directory_iterator(oldDir)) {
                            std::string ext = entry.path().extension().string();
                            
                            if (std::filesystem::is_regular_file(entry.path()) && (ext == ".mp3" || ext == ".ogg")) {
                                std::filesystem::rename(entry.path(), newDir / entry.path().filename());
                                moved++;
                            }
                        }  

                } catch (const std::filesystem::filesystem_error& err) {
                    status = "An error occured while migrating files.";
                    
                    if (moved > 0)
                        status += fmt::format(" {} files were transferred successfully.", moved);
                    else 
                        status += " No files were transferred.";

                    moved = 0;
                    log::error("An error occured while migrating files: {}", err);
                }

            } else {
                status = "Error: the";

                if (!dirsExist[0] && !dirsExist[1]) 
                    status += fmt::format(" folders \"{}\" and \"{}\" do not exist.", oldDir.string(), newDir.string());
                else {
                    status += " folder \"";

                    if (!dirsExist[0]) 
                        status += oldDir.string();
                    else 
                        status += newDir.string();

                    status += "\" does not exist.";
                }
            }
        }

        FLAlertLayer::create(
            "Migration Status",
            (moved > 0) ? fmt::format("Successfully transferred {} files.", std::to_string(moved)) : status,
            "OK"
        )->show();
    }

    void onCommit() override {}
    void onResetToDefault() override {}

public:
    static MyButtonSettingNodeV3* create(std::shared_ptr<MyButtonSettingV3> setting, float width) {
        auto ret = new MyButtonSettingNodeV3();
        if (ret && ret->init(setting, width)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool hasUncommittedChanges() const override {
        return false;
    }
    bool hasNonDefaultValue() const override {
        return false;
    }

    std::shared_ptr<MyButtonSettingV3> getSetting() const {
        return std::static_pointer_cast<MyButtonSettingV3>(SettingNodeV3::getSetting());
    }
};

SettingNodeV3* MyButtonSettingV3::createNode(float width) {
    return MyButtonSettingNodeV3::create(
        std::static_pointer_cast<MyButtonSettingV3>(shared_from_this()),
        width
    );
}

$execute {
    (void)Mod::get()->registerCustomSettingType("button", &MyButtonSettingV3::parse);
}