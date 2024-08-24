#include <Geode/modify/CCSprite.hpp>
#include <regex>
#include "Utils.hpp"
#include "Settings.hpp"

using namespace geode::prelude;

const static std::regex modIDLogoPngRegex = std::regex(R"(^([a-z0-9\-_]+\.[a-z0-9\-_]+)\/logo\.png$)");

$on_mod(Loaded) {
	Mod::get()->addCustomSetting<MySettingValue>("configdir", "none");
	std::filesystem::path pngInConfigDir = Mod::get()->getConfigDir() / fmt::format("{}.png", Mod::get()->getID());
	std::filesystem::path pngInResourcesDir = Mod::get()->getResourcesDir() / fmt::format("{}.png", Mod::get()->getID());
	if (auto foo = std::filesystem::exists(pngInConfigDir)) return;
	if (auto bar = !std::filesystem::exists(pngInResourcesDir)) return;
	std::filesystem::copy(pngInResourcesDir, pngInConfigDir);
}

class $modify(MyCCSprite, CCSprite) {
	static std::filesystem::path generateFilePath(const std::string& modID) {
		return Mod::get()->getConfigDir() / fmt::format("{}.png", modID);
	}
	static CCSprite* create(const char* fileName) {
		CCSprite* sprite = CCSprite::create(fileName);
		std::string fileNameStr = fileName;
		if (!fileNameStr.ends_with(R"(/logo.png)")) { return sprite; }
		if (!Utils::modEnabled()) { return sprite; }
		std::smatch match;
		bool regexMatched = std::regex_match(fileNameStr, match, modIDLogoPngRegex);
		if (!regexMatched) { return sprite; }
		if (match.empty() || match.size() > 2) { return sprite; }
		std::filesystem::path customLogo = MyCCSprite::generateFilePath(match[1].str());
		if (auto fooBarFoo = !std::filesystem::exists(customLogo)) { return sprite; }
		if (CCSprite* iJustWantAPictureOfAGodDangHotDog = CCSprite::create(customLogo.string().c_str()))
			return iJustWantAPictureOfAGodDangHotDog;
		return sprite;
	}
};