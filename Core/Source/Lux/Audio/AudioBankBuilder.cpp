#include "lpch.h"
#include "AudioBankBuilder.h"

#include <cstdlib>

namespace Lux {

	namespace {

		// Checked before anything else so a machine with Studio installed somewhere unusual, or a
		// CI runner, can point at it without touching the project.
		constexpr const char* kStudioCommandLineEnvVar = "LUX_FMOD_STUDIO_CL";
		constexpr const char* kStudioApplicationEnvVar = "LUX_FMOD_STUDIO";

		// FMOD's installers do not put either binary on PATH, so the usual locations are tried
		// explicitly. Ordered newest-first within each platform.
		const char* kStudioCommandLineCandidates[] = {
#ifdef LUX_PLATFORM_WINDOWS
			"C:/Program Files/FMOD SoundSystem/FMOD Studio 2.03.14/fmodstudiocl.exe",
			"C:/Program Files/FMOD SoundSystem/FMOD Studio/fmodstudiocl.exe",
#else
			"/opt/fmodstudio/fmodstudiocl",
			"/usr/local/fmodstudio/fmodstudiocl",
#endif
		};

		const char* kStudioApplicationCandidates[] = {
#ifdef LUX_PLATFORM_WINDOWS
			"C:/Program Files/FMOD SoundSystem/FMOD Studio 2.03.14/FMOD Studio.exe",
			"C:/Program Files/FMOD SoundSystem/FMOD Studio/FMOD Studio.exe",
#else
			"/opt/fmodstudio/fmodstudio",
			"/usr/local/fmodstudio/fmodstudio",
#endif
		};

		std::filesystem::path ResolveTool(const char* envVar, const char* const* candidates, size_t candidateCount, const char* pathName)
		{
			if (const char* fromEnv = std::getenv(envVar))
			{
				std::filesystem::path envPath(fromEnv);
				if (std::filesystem::exists(envPath))
					return envPath;

				LUX_CORE_WARN_TAG("Audio", "{0} points at '{1}', which does not exist - falling back to the default locations", envVar, fromEnv);
			}

			for (size_t i = 0; i < candidateCount; i++)
			{
				std::filesystem::path candidate(candidates[i]);
				std::error_code ec;
				if (std::filesystem::exists(candidate, ec))
					return candidate;
			}

			// Last resort: trust PATH. Returning the bare name lets the shell resolve it, and the
			// caller finds out it was wrong from a non-zero exit code rather than from us.
			return pathName;
		}

		// Newest write time anywhere under a directory. Used on the .fspro's folder, which holds
		// both the XML metadata and the source audio, so any authoring change moves it.
		std::filesystem::file_time_type NewestWriteTimeUnder(const std::filesystem::path& directory, const std::filesystem::path& skipDirectory)
		{
			std::filesystem::file_time_type newest = std::filesystem::file_time_type::min();

			std::error_code ec;
			for (auto it = std::filesystem::recursive_directory_iterator(directory, ec);
				it != std::filesystem::recursive_directory_iterator(); it.increment(ec))
			{
				if (ec)
					break;

				// The build output lives under the project directory, so comparing it against
				// itself would make every build look current the moment it finished.
				if (it->is_directory(ec) && !skipDirectory.empty() && it->path() == skipDirectory)
				{
					it.disable_recursion_pending();
					continue;
				}

				if (!it->is_regular_file(ec))
					continue;

				const auto writeTime = it->last_write_time(ec);
				if (!ec && writeTime > newest)
					newest = writeTime;
			}

			return newest;
		}

	}

	std::filesystem::path AudioBankBuilder::FindStudioCommandLineTool()
	{
		static const std::filesystem::path s_Tool = ResolveTool(kStudioCommandLineEnvVar,
			kStudioCommandLineCandidates, std::size(kStudioCommandLineCandidates), "fmodstudiocl");
		return s_Tool;
	}

	std::filesystem::path AudioBankBuilder::FindStudioApplication()
	{
		static const std::filesystem::path s_App = ResolveTool(kStudioApplicationEnvVar,
			kStudioApplicationCandidates, std::size(kStudioApplicationCandidates), "fmodstudio");
		return s_App;
	}

	bool AudioBankBuilder::NeedsRebuild(const std::filesystem::path& studioProjectPath, const std::filesystem::path& bankDirectory)
	{
		std::error_code ec;
		if (studioProjectPath.empty() || !std::filesystem::exists(studioProjectPath, ec))
			return false;

		if (!std::filesystem::exists(bankDirectory, ec))
			return true;

		// Oldest bank, not newest: a project that gained a bank since the last full build has one
		// current file and one stale one, and the stale one is what matters.
		std::filesystem::file_time_type oldestBank = std::filesystem::file_time_type::max();
		bool foundBank = false;
		for (const auto& entry : std::filesystem::directory_iterator(bankDirectory, ec))
		{
			if (ec)
				break;

			if (!entry.is_regular_file(ec) || entry.path().extension() != ".bank")
				continue;

			foundBank = true;
			const auto writeTime = entry.last_write_time(ec);
			if (!ec && writeTime < oldestBank)
				oldestBank = writeTime;
		}

		if (!foundBank)
			return true;

		const std::filesystem::path projectDirectory = studioProjectPath.parent_path();
		return NewestWriteTimeUnder(projectDirectory, bankDirectory) > oldestBank;
	}

	bool AudioBankBuilder::Build(const std::filesystem::path& studioProjectPath)
	{
		std::error_code ec;
		if (studioProjectPath.empty() || !std::filesystem::exists(studioProjectPath, ec))
		{
			LUX_CORE_ERROR_TAG("Audio", "Cannot build banks: FMOD Studio project not found at '{0}'", studioProjectPath.string());
			return false;
		}

		const std::filesystem::path tool = FindStudioCommandLineTool();

		// -export-guids writes Build/GUIDs.txt: the {guid} -> event:/Path table, for tooling and for
		// diffing what changed between builds. The engine itself reads event GUIDs from the loaded
		// banks (AudioEngine::GetEvents) so the two can never disagree, but the exported file is what
		// makes a rename in Studio traceable - scenes reference events by GUID precisely because a
		// rename would otherwise leave them silently mute.
		const std::string command =
			"\"" + tool.string() + "\" -build -export-guids \"" + studioProjectPath.string() + "\"";

		LUX_CORE_INFO_TAG("Audio", "Building FMOD banks from '{0}'...", studioProjectPath.filename().string());

		const int result = std::system(command.c_str());
		if (result != 0)
		{
			LUX_CORE_ERROR_TAG("Audio", "Bank build failed with exit code {0}. Command: {1}", result, command);
			if (tool == std::filesystem::path("fmodstudiocl"))
				LUX_CORE_ERROR_TAG("Audio", "fmodstudiocl was not found in the usual locations; set {0} to its full path", kStudioCommandLineEnvVar);

			return false;
		}

		LUX_CORE_INFO_TAG("Audio", "Bank build completed.");
		return true;
	}

	bool AudioBankBuilder::OpenInStudio(const std::filesystem::path& path)
	{
		std::error_code ec;
		if (path.empty() || !std::filesystem::exists(path, ec))
		{
			LUX_CORE_ERROR_TAG("Audio", "Cannot open '{0}' in FMOD Studio: it does not exist", path.string());
			return false;
		}

		const std::filesystem::path application = FindStudioApplication();

		// Checked up front because the launch below cannot report this. Backgrounding with "&" makes
		// the shell return 0 immediately whether or not the binary exists, so std::system's exit code
		// says nothing about whether FMOD Studio actually started.
		std::error_code appEc;
		if (!std::filesystem::exists(application, appEc))
		{
			LUX_CORE_ERROR_TAG("Audio", "FMOD Studio not found at '{0}'. Set {1} to its full path.",
				application.string(), kStudioApplicationEnvVar);
			return false;
		}

		// Detached, because FMOD Studio is a long-lived GUI application - std::system would block
		// the editor until the designer closed it.
#ifdef LUX_PLATFORM_WINDOWS
		const std::string command = "start \"\" \"" + application.string() + "\" \"" + path.string() + "\"";
#else
		const std::string command = "\"" + application.string() + "\" \"" + path.string() + "\" >/dev/null 2>&1 &";
#endif

		LUX_CORE_INFO_TAG("Audio", "Opening '{0}' in FMOD Studio...", path.filename().string());
		if (std::system(command.c_str()) != 0)
		{
			LUX_CORE_ERROR_TAG("Audio", "Failed to launch FMOD Studio ('{0}')", application.string());
			return false;
		}

		return true;
	}

}
