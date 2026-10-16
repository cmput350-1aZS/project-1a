
#include "NotificationManager.h"
#include "GameObject.h"

#include <algorithm>

namespace CMPUT350 {

/**
 * Registers an object to receive notifications with the given key.
 * Duplicate registrations for the same object and key are ignored.
 */
void NotificationManager::Register(
    std::weak_ptr<GameObject> object,
    const std::string& key)
{
    // ignore objects that dont exist anymore
    if (object.expired()) {
        return;
    }
    auto& listeners = mListeners[key];

    // check if this obj alr registered
    for (const auto& listener : listeners) {
        if (!listener.owner_before(object) && !object.owner_before(listener)) {
            return;
        }
    }

    listeners.push_back(object);
}

/**
 * Unregisters an object from receiving notifications with the given key.
 * Expired references are also removed.
 */
void NotificationManager::Unregister(
    std::weak_ptr<GameObject> object,
    const std::string& key)
{
    auto it = mListeners.find(key);

    if (it == mListeners.end()) {
        return;
    }

    auto& listeners = it->second;

    listeners.erase(std::remove_if(listeners.begin(), listeners.end(), [&object](const std::weak_ptr<GameObject>& listener) {
    // remove expired obj + this specific obj
        return listener.expired() || (!listener.owner_before(object) && !object.owner_before(listener));
    }),
    listeners.end());
}

/**
 * Immediately delivers a notification to all registered listeners.
 * Expired object references are removed after delivery.
 */
void NotificationManager::Notify(const std::string& message)
{
    auto it = mListeners.find(message);

    if (it == mListeners.end()) {
        return;
    }

    // make separate copy of listener list so if obj unregisters itself, loop can continue thru copied list
    auto listeners = it->second;

    for (const auto& listener : listeners) {
        if (auto object = listener.lock()) {
            object->ReceiveNotification(message);
        }
    }

    // remove expired references from all notif list
    for (auto& entry : mListeners) {
        auto& registeredObjects = entry.second;

        registeredObjects.erase(std::remove_if(registeredObjects.begin(), registeredObjects.end(), [](const std::weak_ptr<GameObject>& object) {
            return object.expired();
        }),
        registeredObjects.end());
    }
}

} // namespace CMPUT350
