#include "UpdateLogic/UpdateClientAppEntry.hpp"
#include "async_simple/executors/SimpleExecutor.h"
#include "cxxopts.hpp"

inline int handleArgument(int argc, char *argv[]) {
	try {
		cxxopts::Options options("UniversalUpdateFramework Client",
								 "A tool that help you update your app.");

		options.add_options()(
			"m,mode",
			"Update Mode. (-m "
			"FullPackageUpdate/DifferencePackageUpdate/"
			"MultiVersionDifferencePackageUpdate/DifferenceUpdate)",
			cxxopts::value<std::string>())("c,config", "Config file path.",
										   cxxopts::value<std::string>())(
			"v,newversion",
			"New version to update. If not given by default will update to the "
			"newest version.",
			cxxopts::value<std::string>())("h,help", "Print usage.");
		auto result = options.parse(argc, argv);

		if (result.count("help")) {
			std::cout << options.help() << std::endl;
			return 0;
		}

		if (result.count("mode")) {
			std::string modeString = result["mode"].as<std::string>();
			UpdateMode mode;
			mode = magic_enum::enum_cast<UpdateMode>(modeString)
					   .value_or(UpdateMode::Unknown);

			if (result.count("config")) {
				std::string configFilePath = result["config"].as<std::string>();
				UpdateConfigIo config(configFilePath);
				config.readFromFile();

				std::string updateToNewVersion;
				if (result.count("newversion")) {
					updateToNewVersion = result["newversion"].as<std::string>();
				}
				ApiRequest api(
					config.getConfig().host, config.getConfig().appName,
					config.getConfig().channel, config.getConfig().platform);
				auto [result1, appVersionContent] =
					async_simple::coro::syncAwait(
						(updateToNewVersion.empty()
							 ? api.GetCurrentAppVersion()
							 : api.GetAppVersion(updateToNewVersion)));
				if (!result1.getStatus()) {
					std::cout << "Cannot connect to serevr!\n";
					return -1;
				}

				try {
					AppVersionInfo appVersion =
						nlohmann::json::parse(appVersionContent);
					updateToNewVersion = appVersion.AppVersion;
				} catch (std::exception &e) {
					std::cout
						<< "Decode version info failed! Please check if the "
						   "version given exist in the server!\n";
					return -1;
				}

				auto updateResult =
					handleUpdateMode(mode, config, updateToNewVersion,
									 [](UpdateStatusInfo updateStatusInfo) {});
				if (updateResult.getStatus()) {
					std::cout << "Update successfully!\n";
				} else {
					std::cout << "Update failed! "
							  << updateResult.getErrorMessage() << "\n";
				}
			} else {
				std::cout << "No config file!\n";
			}
		} else {
			std::cout << "No update mode!\n";
		}

		return 0;
	} catch (std::exception &e) {
		std::cerr << "Exception:" << e.what() << "\n";
	}
}

// int main(int argc, char* argv[]) {
//     return handleArgument(argc, argv);
// }

int main(int argc, char *argv[]) {
	// ApiRequest api("http://localhost:5000", "TestApp", "TestChannel",
	// "Windows");
	// Seele::Log::Init();
	// size_t bytes_received = 0;
	// size_t bytes_total = 0;
	// auto a = apiRequestDownload(
	// 	"https://vip.123pan.cn:443/1838918272/"
	// 	"8469473?auth_key=1729198020-1061263672-"
	// 	"1838918272-eddcc68396d5c9adb5a3161ff973bf58",
	// 	"a.exe", bytes_received, bytes_total,
	// 	[](UpdateStatusInfo updateStatusInfo) {
	// 		std::cout << "Downloading " << updateStatusInfo.CurrentFileName
	// 				  << " " << updateStatusInfo.CurrentProgress << "\n";
	// 	});

	// try {
	// 	async_simple::executors::SimpleExecutor ex(2);
	// 	auto res = async_simple::coro::syncAwait(a, &ex);

	// 	std::cout << res.first.getStatus() << " " << res.first.getErrorMessage()
	// 			  << " " << res.second << "\n";

	// 	Seele::Log::Shutdown();
	// } catch (std::exception &e) {
	// 	std::cerr << "Exception:" << e.what() << "\n";
	// }
	ApiRequest api("https://vip.123pan.cn", "TestApp", "TestChannel",
				   "Windows");

	ApiRequest api2("http://localhost:5275", "TestApp", "TestChannel",
					"Windows");
	auto signedLinkTask =
		api2.GetSignedDirectLink("https://vip.123pan.cn/1838918272/8494695");

	auto signedLink = async_simple::coro::syncAwait(signedLinkTask).second;

	nlohmann::json j = nlohmann::json::parse(signedLink);

	signedLink = j["signedUrl"];
	signedLink = signedLink.substr(25);

	auto task = api.DownloadFile(
		signedLink, "a.exe", [](UpdateStatusInfo updateStatusInfo) {
			std::cout << "Downloading " << updateStatusInfo.CurrentFileName
					  << " " << updateStatusInfo.CurrentProgress << "\n";
		});
	auto res = async_simple::coro::syncAwait(task);
	std::cout << res.first.getStatus() << " " << res.first.getErrorMessage()
			  << "\n";
	std::cout << res.second << "\n";
}
