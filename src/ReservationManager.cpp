#include "ReservationManager.h"
#include <fstream>
#include <iostream>
#include <sstream>
 
using namespace std;
ReservationManager::ReservationManager()
    : reservationHead(nullptr) {
}

ReservationManager::~ReservationManager() {
    deleteAllNodes();
}

void ReservationManager::deleteAllNodes() {
    while (reservationHead) {
        auto* t = reservationHead;
        reservationHead = reservationHead->next;
        delete t;
    }
}

Resource* ReservationManager::findResource(const string& id) {
    for (auto& r : resources) {
        if (r.getId() == id) {
            return &r;
        }
    }

    return nullptr;
}

ReservationManager::ReservationNode*
ReservationManager::findReservationNode(int id) const {
    auto* c = reservationHead;

    while (c) {
        if (c->reservation.getReservationId() == id) {
            return c;
        }

        c = c->next;
    }

    return nullptr;
}

bool ReservationManager::studentHasReservation(
    const string& s,
    const string& r
) const {
    auto* c = reservationHead;

    while (c) {
        if (c->reservation.getStudentId() == s &&
            c->reservation.getResourceId() == r) {
            return true;
        }

        c = c->next;
    }

    return false;
}

bool ReservationManager::loadResources(const string& f) {
    ifstream file(f);

    if (!file) {
        return false;
    }

    resources.clear();

    string line;

    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        stringstream ss(line);

        string id, n, t, status;

        getline(ss, id, ',');
        getline(ss, n, ',');
        getline(ss, t, ',');
        getline(ss, status);

        if (!id.empty()) {
            resources.emplace_back(
                id,
                n,
                t,
                status == "Available" ||
                status == "available" ||
                status == "1"
            );
        }
    }

    return true;
}

bool ReservationManager::loadReservations(const string& f) {
    ifstream file(f);

    if (!file) {
        return false;
    }

    string line;

    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        stringstream ss(line);

        string i, s, n, r, d;

        getline(ss, i, ',');
        getline(ss, s, ',');
        getline(ss, n, ',');
        getline(ss, r, ',');
        getline(ss, d);

        try {
            int id = stoi(i);

            if (!reservationExists(id) && findResource(r)) {
                auto* node =
                    new ReservationNode(
                        Reservation(id, s, n, r, d)
                    );

                if (!reservationHead) {
                    reservationHead = node;
                }
                else {
                    auto* c = reservationHead;

                    while (c->next) {
                        c = c->next;
                    }

                    c->next = node;
                }

                auto* res = findResource(r);

                res->setAvailable(false);
                res->incrementTimesReserved();
            }
        }
        catch (...) {
        }
    }

    return true;
}

void ReservationManager::displayResources() const {
    if (resources.empty()) {
        cout << "No resources loaded.\n";
        return;
    }

    Resource::printHeader();

    for (const auto& r : resources) {
        r.print();
    }
}

void ReservationManager::displayAvailability() const {
    displayResources();
}

void ReservationManager::displayReservations() const {
    if (!reservationHead) {
        cout << "No active reservations.\n";
        return;
    }

    Reservation::printHeader();

    for (auto* c = reservationHead; c; c = c->next) {
        c->reservation.print();
    }
}

bool ReservationManager::reservationExists(int id) const {
    return findReservationNode(id) != nullptr;
}

int ReservationManager::activeReservationCount() const {
    int n = 0;

    for (auto* c = reservationHead; c; c = c->next) {
        ++n;
    }

    return n;
}

bool ReservationManager::createReservation(
    int id,
    const string& s,
    const string& n,
    const string& r,
    const string& d
) {
    if (id <= 0 || s.empty() || n.empty() ||
        r.empty() || d.empty()) {
        cout << "Invalid reservation information.\n";
        return false;
    }

    if (reservationExists(id)) {
        cout << "Reservation ID already exists.\n";
        return false;
    }

    auto* res = findResource(r);

    if (!res) {
        cout << "Resource ID not found.\n";
        return false;
    }

    if (studentHasReservation(s, r)) {
        cout << "Student already has a reservation for this resource.\n";
        return false;
    }

    if (!res->isAvailable()) {
        cout << "Resource is unavailable. Adding student to waiting list.\n";
        addToWaitingList(s, n, r, d);
        return false;
    }

    auto* node =
        new ReservationNode(
            Reservation(id, s, n, r, d)
        );

    if (!reservationHead) {
        reservationHead = node;
    }
    else {
        auto* c = reservationHead;

        while (c->next) {
            c = c->next;
        }

        c->next = node;
    }

    res->setAvailable(false);
    res->incrementTimesReserved();

    cout << "Reservation created successfully.\n";

    return true;
}

bool ReservationManager::cancelReservation(int id) {
    ReservationNode* c = reservationHead;
    ReservationNode* p = nullptr;

    while (c &&
           c->reservation.getReservationId() != id) {
        p = c;
        c = c->next;
    }

    if (!c) {
        cout << "Reservation ID not found.\n";
        return false;
    }

    Reservation x = c->reservation;

    if (!p) {
        reservationHead = c->next;
    }
    else {
        p->next = c->next;
    }

    delete c;

    cancellationHistory.push(x);

    if (auto* res = findResource(x.getResourceId())) {
        res->setAvailable(true);
    }

    cout << "Reservation cancelled and saved in cancellation history.\n";

    processNextWaitingRequest();

    return true;
}

void ReservationManager::addToWaitingList(
    const string& s,
    const string& n,
    const string& r,
    const string& d
) {
    waitingQueue.push({s, n, r, d});

    cout << "Added to waiting queue.\n";
}

bool ReservationManager::processNextWaitingRequest() {
    if (waitingQueue.empty()) {
        return false;
    }

    auto q = waitingQueue.front();

    auto* res = findResource(q.resourceId);

    if (!res || !res->isAvailable()) {
        return false;
    }

    int id = 1;

    while (reservationExists(id)) {
        ++id;
    }

    waitingQueue.pop();

    return createReservation(
        id,
        q.studentId,
        q.studentName,
        q.resourceId,
        q.date
    );
}

bool ReservationManager::undoLastCancellation() {
    if (cancellationHistory.empty()) {
        cout << "No cancellation to undo.\n";
        return false;
    }

    Reservation x = cancellationHistory.top();

    auto* res = findResource(x.getResourceId());

    if (!res || !res->isAvailable()) {
        cout << "Cannot restore: resource is unavailable.\n";
        return false;
    }

    cancellationHistory.pop();

    auto* node = new ReservationNode(x);

    if (!reservationHead) {
        reservationHead = node;
    }
    else {
        auto* c = reservationHead;

        while (c->next) {
            c = c->next;
        }

        c->next = node;
    }

    res->setAvailable(false);

    cout << "Most recent cancellation restored.\n";

    return true;
}

void ReservationManager::displayWaitingList() const {
    if (waitingQueue.empty()) {
        cout << "Waiting list is empty.\n";
        return;
    }

    auto q = waitingQueue;

    cout << "\nWaiting List (FIFO)\n";

    while (!q.empty()) {
        auto& r = q.front();

        cout << "Student ID: " << r.studentId
             << " | Name: " << r.studentName
             << " | Resource: " << r.resourceId
             << " | Date: " << r.date << '\n';

        q.pop();
    }
}

void ReservationManager::displayCancellationHistory() const {
    if (cancellationHistory.empty()) {
        cout << "Cancellation history is empty.\n";
        return;
    }

    auto s = cancellationHistory;

    cout << "\nCancellation History (most recent first)\n";

    while (!s.empty()) {
        cout << "Reservation ID: "
             << s.top().getReservationId()
             << " | Student: "
             << s.top().getStudentName()
             << " | Resource: "
             << s.top().getResourceId()
             << '\n';

        s.pop();
    }
}