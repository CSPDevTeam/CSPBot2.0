#pragma once
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <Windows.h>

using std::unordered_map;
using std::string;
using std::vector;
using std::map;

#ifdef CSPBot_EXPORTS
#define CSPAPI __declspec(dllexport)
#else
#define CSPAPI __declspec(dllimport)
#endif

typedef void (*CommandCallback)(std::vector<std::string> args);
typedef void (*EventCallback)(std::unordered_map<std::string, std::string> data);

// Type
struct Version {
	enum Status {
		Dev,
		Beta,
		Release
	};

	int major;
	int minor;
	int revision;
	Status status;

	CSPAPI explicit Version(int major = 0, int minor = 0, int revision = 0, Status status = Status::Release);

	CSPAPI bool operator<(Version b);
	CSPAPI bool operator==(Version b);

	CSPAPI std::string toString(bool needStatus = false);
	CSPAPI static Version parse(const std::string& str);
};

struct Plugin {
	std::string name;
	std::string desc; // `introduction` before
	Version version;
	std::map<std::string, std::string> others; // `otherInformation` before

	std::string filePath;
	HMODULE handle;
	// Call a Function by Symbol String
	template <typename ReturnType = void, typename... Args>
	inline ReturnType callFunction(const char* functionName, Args... args) {
		void* address = GetProcAddress(handle, functionName);
		if (!address)
			return ReturnType();
		return reinterpret_cast<ReturnType (*)(Args...)>(address)(std::forward<Args>(args)...);
	}
};