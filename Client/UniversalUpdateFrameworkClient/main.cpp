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

async_simple::coro::Lazy<std::pair<ReturnWrapper, std::string>>
apiRequestDownload(std::string uri, std::string localSaveFilePath,
				   size_t &bytes_received, size_t &bytes_total,
				   std::function<void(UpdateStatusInfo)> updateStatusCallback) {
	cinatra::coro_http_client client1{};
	cinatra::coro_http_client client{};
	std::error_code ec{};
	std::filesystem::remove(localSaveFilePath, ec);

	// Send a HEAD request
	auto head_result = co_await client1.async_head(uri);
	if (head_result.net_err || head_result.status != 200) {
		SEELE_INFO_TAG(__func__, "{}",
					   "Http Response Error!" + head_result.net_err.message());
		std::pair<ReturnWrapper, std::string> ret{
			{false, ErrorCode::HttpResponseError,
			 "Http Response Error!" + head_result.net_err.message()},
			""};
		co_return ret;
	}

	auto iter = std::find_if(head_result.resp_headers.begin(),
							 head_result.resp_headers.end(),
							 [](const cinatra::http_header &header) {
								 return header.name == "Content-Length";
							 });
	if (iter == head_result.resp_headers.end()) {
		SEELE_INFO_TAG(__func__, "{}",
					   "Http Response Error! Content-Length not found!");

		std::pair<ReturnWrapper, std::string> ret{
			{false, ErrorCode::HttpResponseError,
			 "Http Response Error! Content-Length not found!"},
			""};
		co_return ret;
	}
	bytes_total = std::stoul(std::string(iter->value));

	updateStatusCallback(
		UpdateStatusInfo{.status = UpdateStatus::DownloadingFile,
						 .CurrentFileName = localSaveFilePath,
						 .CurrentProgress = 0,
						 .DownloadTotalSize = static_cast<int>(bytes_total),
						 .DownloadCurrentSize = 0});

	std::ofstream ofstream(localSaveFilePath, std::ios::binary);
	if (!ofstream.is_open()) {
		SEELE_INFO_TAG(__func__, "Open File {} Failed!", localSaveFilePath);
		std::pair<ReturnWrapper, std::string> ret{
			{false, ErrorCode::HttpResponseError,
			 "Open File " + localSaveFilePath + " Failed!"},
			""};
		co_return ret;
	}

	// Download the file in chunks of 10MB
	const size_t chunk_size = 10 * 1024 * 1024;
	for (size_t start = 0; start < bytes_total; start += chunk_size) {
		size_t end = std::min(start + chunk_size - 1, bytes_total - 1);
		SEELE_INFO_TAG(__func__, "{}",
					   "Download Range: " + std::to_string(start) + "-" +
						   std::to_string(end) + "/" +
						   std::to_string(bytes_total));
		std::unordered_map<std::string, std::string> headers = {
			{"Range",
			 "bytes=" + std::to_string(start) + "-" + std::to_string(end)}};
		client.add_header("Range", "bytes=" + std::to_string(start) + "-" +
									   std::to_string(end));
		auto result = co_await client.async_get(uri);
		if (!result.net_err && result.status == 206) {
			bytes_received += end - start + 1;

			ofstream << result.resp_body;
		} else {
			SEELE_INFO_TAG(__func__, "{}",
						   "Http Response Error!" + result.net_err.message());
			std::pair<ReturnWrapper, std::string> ret{
				{false, ErrorCode::HttpResponseError,
				 "Http Response Error!" + result.net_err.message()},
				""};
			co_return ret;
		}
		updateStatusCallback(UpdateStatusInfo{
			.status = UpdateStatus::DownloadingFile,
			.CurrentFileName = localSaveFilePath,
			.CurrentProgress = static_cast<float>(start) / bytes_total,
			.DownloadTotalSize = static_cast<int>(bytes_total),
			.DownloadCurrentSize = static_cast<int>(bytes_received)});
	}

	ofstream.close();

	std::pair<ReturnWrapper, std::string> ret{{true}, localSaveFilePath};
	SEELE_INFO_TAG(__func__, "Download file success! File: {}",
				   localSaveFilePath);
	co_return ret;
}

int main(int argc, char *argv[]) {
	// ApiRequest api("http://localhost:5000", "TestApp", "TestChannel",
	// "Windows");
	Seele::Log::Init();
	size_t bytes_received = 0;
	size_t bytes_total = 0;
	auto a = apiRequestDownload(
		"https://vip.123pan.cn:443/1838918272/"
		"8469473?auth_key=1729198020-1061263672-"
		"1838918272-eddcc68396d5c9adb5a3161ff973bf58",
		"a.exe", bytes_received, bytes_total,
		[](UpdateStatusInfo updateStatusInfo) {
			std::cout << "Downloading " << updateStatusInfo.CurrentFileName
					  << " " << updateStatusInfo.CurrentProgress << "\n";
		});

	try {
		async_simple::executors::SimpleExecutor ex(2);
		auto res = async_simple::coro::syncAwait(a, &ex);

		std::cout << res.first.getStatus() << " " << res.first.getErrorMessage()
				  << " " << res.second << "\n";

		Seele::Log::Shutdown();
	} catch (std::exception &e) {
		std::cerr << "Exception:" << e.what() << "\n";
	}
}
