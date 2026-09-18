#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>

using namespace std;

class Resource {
public:
    Resource();
    Resource(const string& id, const string& name, const string& type, bool available);

    const string& getId() const;
    const string& getName() const;
    const string& getType() const;
    bool isAvailable() const;
    int getTimesReserved() const;

    void setAvailable(bool available);
    void incrementTimesReserved();

    void print() const;
    static void printHeader();

private:
    string id;
    string name;
    string type;
    bool available;
    int timesReserved;
};

#endif
