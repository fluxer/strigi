// generated by makecode.pl
#ifndef DBUSCLIENTINTERFACE_H
#define DBUSCLIENTINTERFACE_H
#include "clientinterface.h"
#define DBUS_API_SUBJECT_TO_CHANGE 1
#include <dbus/dbus.h>
#include <exception>
class DBusObjectInterface;
class DBusClientInterface : public ClientInterface {
private:
    std::string object;
    DBusConnection* const conn;
    DBusObjectInterface* const iface;
public:
    DBusClientInterface(const std::string& objectname, DBusConnection* c, ClientInterface* i);
    ~DBusClientInterface();
    DBusObjectInterface* interface() { return iface; }
};
#endif
