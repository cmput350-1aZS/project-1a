
#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace CMPUT350 {

class GameObject;

class NotificationManager {
public:
    // registers an object to receive a notification
    void Register(std::weak_ptr<GameObject> object, const std::string& key);

    // removes an object's registration for a notification
    void Unregister(std::weak_ptr<GameObject> object, const std::string& key);

    // immediately sends a notification to all registered listeners
    void Notify(const std::string& message);

private:
    // weak pointers prevent the manager from owning game objects
    std::unordered_map<std::string, std::vector<std::weak_ptr<GameObject>>> mListeners;
};

} // namespace CMPUT350

#endif
