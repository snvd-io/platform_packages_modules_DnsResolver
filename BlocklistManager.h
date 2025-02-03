// BlocklistManager.h

#ifndef BLOCKLIST_MANAGER_H
#define BLOCKLIST_MANAGER_H

#include <string>
#include <unordered_set>
#include <mutex>

namespace android {
namespace net {

class BlocklistManager {
public:
    // Declaration of getInstance()
    static BlocklistManager& getInstance();

    // Other public methods
    bool loadBlocklist(const std::string& filename);
    bool isBlocked(const std::string& domain) const;

private:
    // Private constructor for Singleton pattern
    BlocklistManager() = default;
    // Delete copy constructor and assignment operator
    BlocklistManager(const BlocklistManager&) = delete;
    BlocklistManager& operator=(const BlocklistManager&) = delete;

    // Private members
    mutable std::mutex mMutex;
    std::unordered_set<std::string> blocklist;

    // Helper function to trim whitespace
    static std::string trim(const std::string& str);
};

}  // namespace net
}  // namespace android

#endif // BLOCKLIST_MANAGER_H

