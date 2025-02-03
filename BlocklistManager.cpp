// BlocklistManager.cpp

#define LOG_TAG "BlocklistManager"
#include "BlocklistManager.h"

#include <fstream>
#include <algorithm>
#include <cctype>
#include "log/log.h"

namespace android {
namespace net {

// Helper function to trim leading/trailing whitespace
std::string BlocklistManager::trim(const std::string& str) {
    auto start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Definition of getInstance()
BlocklistManager& BlocklistManager::getInstance() {
    static BlocklistManager instance;
    return instance;
}

bool BlocklistManager::loadBlocklist(const std::string& filename) {
    ALOGI("%s: Attempting to open blocklist file: %s", __func__, filename.c_str());

    std::ifstream file(filename);
    if (!file.is_open()) {
        ALOGE("%s: Failed to open blocklist file: %s Error: %s", __func__, filename.c_str(), strerror(errno));
        return false;
    }

    std::unique_lock<std::mutex> lock(mMutex);

    std::string line;
    while (std::getline(file, line)) {
        std::string domain = trim(line);
        if (!domain.empty()) {
            // Convert to lowercase to ensure case-insensitive matching
            std::transform(domain.begin(), domain.end(), domain.begin(),
                           [](unsigned char c){ return std::tolower(c); });
            blocklist.insert(domain);
            // Taieb : commented for log performance - huge list - logs taking 10-20 seconds
	    // ALOGI("%s: Added domain to blocklist: %s", __func__, domain.c_str());
        }
    }

    ALOGI("%s: Successfully loaded %zu domains into blocklist", __func__, blocklist.size());
    return true;
}

bool BlocklistManager::isBlocked(const std::string& domain) const {
    ALOGI("%s: Checking domain => %s", __func__, domain.c_str());

    // Make a lowercase copy of the domain
    std::string currentDomain = domain;
    std::transform(currentDomain.begin(), currentDomain.end(), currentDomain.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    std::unique_lock<std::mutex> lock(mMutex);

    // Check exact match
    if (blocklist.find(currentDomain) != blocklist.end()) {
        ALOGI("%s: Exact match found in blocklist => %s", __func__, currentDomain.c_str());
        return true;
    }

    // Check parent domains
    while (true) {
        size_t dotPos = currentDomain.find('.');
        if (dotPos == std::string::npos) {
            break;
        }
        currentDomain = currentDomain.substr(dotPos + 1);
        if (blocklist.find(currentDomain) != blocklist.end()) {
            ALOGI("%s: Parent domain match => %s", __func__, currentDomain.c_str());
            return true;
        }
    }

    ALOGI("%s: Domain not blocked => %s", __func__, domain.c_str());
    return false;
}

}  // namespace net
}  // namespace android

