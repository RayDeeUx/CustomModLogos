#include <Geode/ui/GeodeUI.hpp>
#include "Utils.hpp"
#include "Settings.hpp"

using namespace geode::prelude;

// #include <regex>
// const static std::regex modIDLogoPngRegex = std::regex(R"(^([a-z0-9\-_]+\.[a-z0-9\-_]+)\/logo\.png$)");

static std::filesystem::path generateFilePath(const std::string_view modID) {
	return Mod::get()->getConfigDir() / fmt::format("{}.png", modID);
}

$on_mod(Loaded) {
	(void) Mod::get()->registerCustomSettingType("configdir", &MyButtonSettingV3::parse);
	std::filesystem::path pngInConfigDir = Mod::get()->getConfigDir() / fmt::format("{}.png", Mod::get()->getID());
	std::filesystem::path pngInResourcesDir = Mod::get()->getResourcesDir() / fmt::format("{}.png", Mod::get()->getID());

	if (auto foo = std::filesystem::exists(pngInConfigDir)) return;
	if (auto bar = !std::filesystem::exists(pngInResourcesDir)) return;
	std::filesystem::copy(pngInResourcesDir, pngInConfigDir);

	Mod::get()->setLoggingEnabled(Utils::getBool("logging"));
	listenForSettingChanges<bool>("logging", [](bool newLogging) { Mod::get()->setLoggingEnabled(newLogging); });
}
// apparently this NEEDS to be in an execute thread (at least for v4.4.0)????? --raydeeux
$execute {
	// new EventListener<EventFilter<ModLogoUIEvent>>(+[](ModLogoUIEvent* event) {
	auto listener = ModLogoUIEvent().listen([](cocos2d::CCNode* sprite, std::string_view modID, std::optional<geode::Mod*> mod) {
		if (modID.empty() || modID == "geode.loader") return;
		// log::info("a new ModLogoUIEvent posted, LET'S BEGIN.");
		if (!Utils::modEnabled() || !sprite || !mod.has_value()) return;

		std::string formattedSpriteID = fmt::format("{}-custom-logo"_spr, modID);
		// log::info("formattedSpriteID: {}", formattedSpriteID);
		if (sprite->getChildByID(formattedSpriteID)) {
			// log::info("sprite->getChildByID(formattedSpriteID) was true, ABORT MISSION");
			return;
		}

		std::filesystem::path customLogoPath = generateFilePath(modID);
		log::info("customLogoPath: {}", customLogoPath);
		if (!std::filesystem::exists(customLogoPath)) {
			// log::info("!std::filesystem::exists(customLogoPath) was true, ABORT MISSION");
			return;
		}

		CCSprite* iJustWantAPictureOfAGodDangHotDog = CCSprite::create(geode::utils::string::pathToString(customLogoPath).c_str());
		if (!iJustWantAPictureOfAGodDangHotDog) {
			// log::info("!iJustWantAPictureOfAGodDangHotDog was true, ABORT MISSION");
			return;
		}
		iJustWantAPictureOfAGodDangHotDog->setID(formattedSpriteID);

		const CCSize originalSize = sprite->getContentSize();
		const CCSize replacementSize = iJustWantAPictureOfAGodDangHotDog->getContentSize();
		const float yRatio = originalSize.height / replacementSize.height;
		const float xRatio = originalSize.width / replacementSize.width;

		iJustWantAPictureOfAGodDangHotDog->setScale(std::min(xRatio, yRatio));
		if (auto trueSprite = typeinfo_cast<CCSprite*>(sprite)) {
			trueSprite->setCascadeOpacityEnabled(false);
			trueSprite->setOpacity(0);
		}
		sprite->addChildAtPosition(iJustWantAPictureOfAGodDangHotDog, Anchor::Center);
	});
	listener.leak();
}

/*
// old code left here for posterity
// let this be a warning to all: never hook cocos functions
// unless if you have very valid reason to do so
// --raydeeux
class $modify(MyCCSprite, CCSprite) {
	static std::filesystem::path generateFilePath(const std::string& modID) {
		return Mod::get()->getConfigDir() / fmt::format("{}.png", modID);
	}
	static CCSprite* create(const char* fileName) {
		CCSprite* sprite = CCSprite::create(fileName);
		std::string fileNameStr = fileName;
		if (!fileNameStr.ends_with(R"(/logo.png)")) return sprite;
		if (!Utils::modEnabled()) return sprite;
		std::smatch match;
		bool regexMatched = std::regex_match(fileNameStr, match, modIDLogoPngRegex);
		if (!regexMatched) return sprite;
		if (match.empty() || match.size() > 2) return sprite;
		std::filesystem::path customLogo = MyCCSprite::generateFilePath(match[1].str());
		if (auto fooBarFoo = !std::filesystem::exists(customLogo)) return sprite;
		if (CCSprite* iJustWantAPictureOfAGodDangHotDog = CCSprite::create(customLogo.string().c_str()))
			return iJustWantAPictureOfAGodDangHotDog;
		return sprite;
	}
};
*/