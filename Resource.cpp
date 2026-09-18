#include "Resource.h"
#include <iostream>

using namespace std;

// Adds spaces to the end of text until it is width characters long,
// so the columns of the table lines up and saves headaches. Same thing,
// as Reservation.cpp
static string pad(string text, size_t width) {
    while (text.size() < width) text += ' ';
    return text;
}

Resource::Resource() : available(false), timesReserved(0) {}

Resource::Resource(const string& id, const string& name, const string& type, bool available)
: id(id), name(name), type(type), available(available), timesReserved(0) {}

const string& Resource::getId() const { return id; }
const string& Resource::getName() const { return name; }
const string& Resource::getType() const { return type; }
bool Resource::isAvailable() const { return available; }
int Resource::getTimesReserved() const { return timesReserved; }

void Resource::setAvailable(bool a) { available = a; }
void Resource::incrementTimesReserved() { ++timesReserved; }

void Resource::printHeader() {
    cout << pad("ID", 8) << pad("Name", 22) << pad("Type", 24) << "Status\n"
              << string(66, '-') << '\n';
}

void Resource::print() const {
    cout << pad(id, 8) << pad(name, 22) << pad(type, 24) << (available ? "Available" : "Unavailable") << '\n';
}
