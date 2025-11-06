/* =================================================================================== *
		   * =================== TechnoSoftware =================== *
	     * ================= Discord Rich Presence ================== *
 * =================================================================================== */

#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <atomic>
#include <thread>
#include <chrono>
#include <ctime>
#include <filesystem>

#include <shellapi.h>
#include <fileapi.h>
#include <tlhelp32.h>

#include <discord.h>

class DiscordIntegration {
private:
	static constexpr int64_t APP_ID = 1331410585665142886;

	std::unique_ptr<discord::Core> core_;
	std::atomic<bool> isRunning_{ false };
	std::thread callbackThread_;
	std::time_t startTime_;

	void OnCurrentUserUpdate() {
		discord::User currentUser{};
		core_->UserManager().GetCurrentUser(&currentUser);

		std::cout << "Discord: Connected as "
			<< currentUser.GetUsername()
			<< "#" << currentUser.GetDiscriminator() << std::endl;
	}

	void OnActivityJoin(const char* secret) {
		std::cout << "Discord: Join game with secret: " << secret << std::endl;
	}

	void OnActivitySpectate(const char* secret) {
		std::cout << "Discord: Spectate game with secret: " << secret << std::endl;
	}

	void OnActivityJoinRequest(const discord::User& user) {
		std::cout << "Discord: Join request from "
			<< user.GetUsername() << std::endl;

		core_->ActivityManager().SendRequestReply(
			user.GetId(),
			discord::ActivityJoinRequestReply::Yes,
			[](discord::Result result) {
				if (result == discord::Result::Ok) {
					std::cout << "Discord: Join request accepted" << std::endl;
				}
			});
	}

public:
	DiscordIntegration() : startTime_(std::time(nullptr)) {}

	~DiscordIntegration() {
		Shutdown();
	}

	bool Initialize() {
		std::cout << "=== Modern Discord SDK ===" << std::endl;
		std::cout << "App ID: " << APP_ID << std::endl;

		discord::Core* core = nullptr;
		auto result = discord::Core::Create(APP_ID, DiscordCreateFlags_Default, &core);
		core_.reset(core);

		if (!core_) {
			std::cout << "Discord: Failed to create core (Error: "
				<< static_cast<int>(result) << ")" << std::endl;
			return false;
		}

		core_->SetLogHook(
			discord::LogLevel::Info,
			[](discord::LogLevel level, const char* message) {
				std::cout << "Discord Log: " << message << std::endl;
			}
		);

		core_->ActivityManager().RegisterCommand("Counter-Strike Techno:Zombies.exe");

		core_->UserManager().OnCurrentUserUpdate.Connect([this]() {
			OnCurrentUserUpdate();
			});

		core_->ActivityManager().OnActivityJoin.Connect([this](const char* secret) {
			OnActivityJoin(secret);
			});

		core_->ActivityManager().OnActivitySpectate.Connect([this](const char* secret) {
			OnActivitySpectate(secret);
			});

		core_->ActivityManager().OnActivityJoinRequest.Connect([this](const discord::User& user) {
			OnActivityJoinRequest(user);
			});

		isRunning_ = true;
		callbackThread_ = std::thread([this]() {
			while (isRunning_) {
				core_->RunCallbacks();
				std::this_thread::sleep_for(std::chrono::milliseconds(16));
			}
			});

		UpdatePresence("Starting game...", "TechnoSoftware");

		std::cout << "Discord: Modern SDK initialized successfully!" << std::endl;
		return true;
	}

	void UpdatePresence(const std::string& state, const std::string& details = "",
		const std::string& largeImage = "logo",
		const std::string& largeText = "TechnoSoftware",
		const std::string& smallImage = "",
		const std::string& smallText = "") {
		if (!core_) return;

		discord::Activity activity{};

		activity.SetState(state.c_str());
		activity.SetDetails(details.c_str());

		activity.GetTimestamps().SetStart(startTime_);

		activity.GetAssets().SetLargeImage(largeImage.c_str());
		activity.GetAssets().SetLargeText(largeText.c_str());

		if (!smallImage.empty()) {
			activity.GetAssets().SetSmallImage(smallImage.c_str());
			activity.GetAssets().SetSmallText(smallText.c_str());
		}

		activity.SetType(discord::ActivityType::Playing);

		// activity.GetParty().SetId("cstz_paty");
		// activity.GetSecrets().SetJoin("join_secret_here");   
		// activity.GetParty().GetSize().SetCurrentSize(1);
		// activity.GetParty().GetSize().SetMaxSize(16);

		core_->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
			if (result != discord::Result::Ok) {
				std::cout << "Discord: Failed to update activity (Error: "
					<< static_cast<int>(result) << ")" << std::endl;
			}
			});
	}

	void UpdateInMenu() {
		UpdatePresence("В главном меню", "Выбор сервера");
	}

	void UpdateInGame(const std::string& map, const std::string& mode, int players, int maxPlayers) {
		std::string details = "Map: " + map;
		std::string state = mode + " | " + std::to_string(players) + "/" + std::to_string(maxPlayers);

		UpdatePresence(state, details, map, "TechnoSoftware");
	}

	void UpdateLoading(const std::string& map) {
		UpdatePresence("Загрузка...", "Карта: " + map);
	}

	void UpdateCustom(const std::string& state, const std::string& details,
		const std::string& imageKey = "logo") {
		UpdatePresence(state, details, imageKey);
	}

	void UpdatePresenceWithPlayers(const std::string& state, const std::string& details,
		int currentPlayers, int maxPlayers,
		const std::string& mapName = "") {
		if (!core_) return;

		discord::Activity activity{};

		activity.SetState(state.c_str());
		activity.SetDetails(details.c_str());

		activity.GetParty().SetId("cs_party");
		activity.GetParty().GetSize().SetCurrentSize(currentPlayers);
		activity.GetParty().GetSize().SetMaxSize(maxPlayers);

		activity.GetTimestamps().SetStart(startTime_);

		activity.GetAssets().SetLargeImage("logo");
		activity.GetAssets().SetLargeText("Counter-Strike T:Z");

		if (!mapName.empty()) {
			activity.GetAssets().SetSmallImage(mapName.c_str());
			activity.GetAssets().SetSmallText(("Map: " + mapName).c_str());
		}

		activity.SetType(discord::ActivityType::Playing);

		core_->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
			if (result != discord::Result::Ok) {
				std::cout << "Failed to update activity with players" << std::endl;
			}
			});
	}

	void ClearPresence() {
		if (!core_) return;

		core_->ActivityManager().ClearActivity([](discord::Result result) {
			if (result == discord::Result::Ok) {
				std::cout << "Discord: Presence cleared" << std::endl;
			}
			});
	}

	void Shutdown() {
		isRunning_ = false;

		if (callbackThread_.joinable()) {
			callbackThread_.join();
		}

		if (core_) {
			ClearPresence();
			core_.reset();
		}

		std::cout << "Discord: Shutdown complete" << std::endl;
	}

	bool IsInitialized() const {
		return core_ != nullptr;
	}
};