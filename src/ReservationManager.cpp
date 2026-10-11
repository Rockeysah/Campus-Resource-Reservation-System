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

bool ReservationManager::reservationExists(int id) const {
    return findReservationNode(id) != nullptr;
}

bool ReservationManager::reservationIdInCancellationHistory(int id) const {
    auto history = cancellationHistory;

    while (!history.empty()) {
        if (history.top().getReservationId() == id) {
            return true;
        }

        history.pop();
    }

    return false;
}

int ReservationManager::generateNextReservationId() const {
    int highestId = 0;

    // Check active reservations
    for (auto* c = reservationHead; c; c = c->next) {
        if (c->reservation.getReservationId() > highestId) {
            highestId = c->reservation.getReservationId();
        }
    }

    // Check cancelled reservations
    auto history = cancellationHistory;

    while (!history.empty()) {
        if (history.top().getReservationId() > highestId) {
            highestId = history.top().getReservationId();
        }

        history.pop();
    }

    return highestId + 1;
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

    // Prevent duplicate IDs in active reservations
    // and in cancellation history.
    if (reservationExists(id) ||
        reservationIdInCancellationHistory(id)) {
        cout << "Reservation ID is already in use.\n";
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

    // Store cancelled reservation in the stack.
    cancellationHistory.push(x);

    // Make the resource available.
    if (auto* res = findResource(x.getResourceId())) {
        res->setAvailable(true);
    }

    cout << "Reservation cancelled and saved in cancellation history.\n";

    // Try to process an available waiting request.
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

    queue<WaitingRequest> remainingQueue;

    WaitingRequest selectedRequest;
    bool foundAvailableRequest = false;

    size_t numberOfRequests = waitingQueue.size();

    // Check all waiting requests so that an unavailable
    // resource at the front does not block requests
    // for other available resources.
    for (size_t i = 0; i < numberOfRequests; ++i) {
        WaitingRequest request = waitingQueue.front();
        waitingQueue.pop();

        Resource* resource = findResource(request.resourceId);

        if (!foundAvailableRequest &&
            resource != nullptr &&
            resource->isAvailable()) {

            selectedRequest = request;
            foundAvailableRequest = true;
        }
        else {
            remainingQueue.push(request);
        }
    }

    waitingQueue = remainingQueue;

    if (!foundAvailableRequest) {
        return false;
    }

    // Generate an ID that is not already active
    // and is not in cancellation history.
    int id = generateNextReservationId();

    if (createReservation(
            id,
            selectedRequest.studentId,
            selectedRequest.studentName,
            selectedRequest.resourceId,
            selectedRequest.date)) {

        return true;
    }

    // If creation fails, put the request back
    // into the waiting queue.
    waitingQueue.push(selectedRequest);

    return false;
}

bool ReservationManager::undoLastCancellation() {
    if (cancellationHistory.empty()) {
        cout << "No cancellation to undo.\n";
        return false;
    }

    Reservation x = cancellationHistory.top();

    // Prevent duplicate reservation IDs during undo.
    if (reservationExists(x.getReservationId())) {
        cout << "Cannot undo: reservation ID already exists.\n";
        return false;
    }

    auto* res = findResource(x.getResourceId());

    if (!res) {
        cout << "Cannot undo: resource not found.\n";
        return false;
    }

    if (!res->isAvailable()) {
        cout << "Cannot restore: resource is unavailable.\n";
        return false;
    }

    // Remove the reservation from cancellation history
    // only after all validation succeeds.
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
// The following features are added without changing the existing
// reservation, cancellation, waiting-list, or undo implementations.

void ReservationManager::searchResourceById() const {
    string id;
    cout << "Enter Resource ID: ";
    cin >> id;

    // Linear search through the resources.
    for (const auto& resource : resources) {
        if (resource.getId() == id) {
            cout << "\nResource found:\n";
            Resource::printHeader();
            resource.print();
            return;
        }
    }
    cout << "Resource ID not found.\n";
}

void ReservationManager::searchReservationOrStudent() const {
    int choice;
    cout << "1. Search by Reservation ID\n";
    cout << "2. Search by Student ID\n";
    cout << "Choice: ";
    if (!(cin >> choice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid choice.\n";
        return;
    }

    if (choice == 1) {
        int id;
        cout << "Enter Reservation ID: ";
        if (!(cin >> id)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid reservation ID.\n";
            return;
        }
        for (auto* node = reservationHead; node; node = node->next) {
            if (node->reservation.getReservationId() == id) {
                cout << "\nReservation found:\n";
                Reservation::printHeader();
                node->reservation.print();
                return;
            }
        }
        cout << "Reservation ID not found.\n";
    } else if (choice == 2) {
        string studentId;
        cout << "Enter Student ID: ";
        cin >> studentId;
        bool found = false;
        for (auto* node = reservationHead; node; node = node->next) {
            if (node->reservation.getStudentId() == studentId) {
                if (!found) Reservation::printHeader();
                node->reservation.print();
                found = true;
            }
        }
        if (!found) cout << "No active reservations found for that student.\n";
    } else {
        cout << "Invalid choice.\n";
    }
}

void ReservationManager::sortResourcesByName() const {
    vector<Resource> sorted = resources;
    if (sorted.empty()) {
        cout << "No resources loaded.\n";
        return;
    }

    // Quick Sort: partition resources by name, then sort each partition.
    auto quickSort = [&](auto&& self, int low, int high) -> void {
        if (low >= high) return;
        int i = low;
        int j = high;
        string pivot = sorted[low + (high - low) / 2].getName();
        while (i <= j) {
            while (sorted[i].getName() < pivot) ++i;
            while (sorted[j].getName() > pivot) --j;
            if (i <= j) {
                Resource temp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = temp;
                ++i;
                --j;
            }
        }
        if (low < j) self(self, low, j);
        if (i < high) self(self, i, high);
    };
    quickSort(quickSort, 0, static_cast<int>(sorted.size()) - 1);

    cout << "\nResources sorted by name (Quick Sort):\n";
    Resource::printHeader();
    for (const auto& resource : sorted) resource.print();
}

void ReservationManager::sortResourcesByPopularity() const {
    vector<Resource> sorted = resources;
    if (sorted.empty()) {
        cout << "No resources loaded.\n";
        return;
    }

    // Quick Sort by recorded reservation count, highest count first.
    auto quickSort = [&](auto&& self, int low, int high) -> void {
        if (low >= high) return;
        int i = low;
        int j = high;
        int pivot = sorted[low + (high - low) / 2].getTimesReserved();
        while (i <= j) {
            while (sorted[i].getTimesReserved() > pivot) ++i;
            while (sorted[j].getTimesReserved() < pivot) --j;
            if (i <= j) {
                Resource temp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = temp;
                ++i;
                --j;
            }
        }
        if (low < j) self(self, low, j);
        if (i < high) self(self, i, high);
    };
    quickSort(quickSort, 0, static_cast<int>(sorted.size()) - 1);

    cout << "\nResources sorted by reservation count (Quick Sort):\n";
    Resource::printHeader();
    for (const auto& resource : sorted) {
        resource.print();
        cout << "  Times reserved: " << resource.getTimesReserved() << "\n";
    }
}

void ReservationManager::sortReservationsByDate() const {
    vector<Reservation> sorted;
    for (auto* node = reservationHead; node; node = node->next) {
        sorted.push_back(node->reservation);
    }
    if (sorted.empty()) {
        cout << "No active reservations.\n";
        return;
    }

    // Quick Sort by date, ascending. Original linked list remains unchanged.
    auto quickSort = [&](auto&& self, int low, int high) -> void {
        if (low >= high) return;
        int i = low;
        int j = high;
        string pivot = sorted[low + (high - low) / 2].getDate();
        while (i <= j) {
            while (sorted[i].getDate() < pivot) ++i;
            while (sorted[j].getDate() > pivot) --j;
            if (i <= j) {
                Reservation temp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = temp;
                ++i;
                --j;
            }
        }
        if (low < j) self(self, low, j);
        if (i < high) self(self, i, high);
    };
    quickSort(quickSort, 0, static_cast<int>(sorted.size()) - 1);

    cout << "\nReservations sorted by date (Quick Sort):\n";
    Reservation::printHeader();
    for (const auto& reservation : sorted) reservation.print();
}

void ReservationManager::reportActiveReservations() const {
    cout << "\nActive Reservations: " << activeReservationCount() << "\n";
    displayReservations();
}

void ReservationManager::reportResourceUtilization() const {
    cout << "\nResource Utilization / Reservation Counts\n";
    if (resources.empty()) { cout << "No resources loaded.\n"; return; }
    for (const auto& resource : resources) {
        cout << resource.getId() << " - " << resource.getName()
             << ": " << resource.getTimesReserved() << " reservation(s) recorded; "
             << (resource.isAvailable() ? "Available" : "Unavailable") << "\n";
    }
}

void ReservationManager::reportMostRequestedResource() const {
    if (resources.empty()) { cout << "No resources loaded.\n"; return; }
    const Resource* mostRequested = &resources[0];
    for (const auto& resource : resources) {
        if (resource.getTimesReserved() > mostRequested->getTimesReserved())
            mostRequested = &resource;
    }
    cout << "\nMost requested resource (based on recorded reservation count):\n"
         << mostRequested->getId() << " - " << mostRequested->getName()
         << " (" << mostRequested->getTimesReserved() << " reservation(s))\n";
}

void ReservationManager::reportWaitingListStatistics() const {
    auto copy = waitingQueue;
    cout << "\nWaiting-list requests: " << copy.size() << "\n";
    if (copy.empty()) { cout << "No students are currently waiting.\n"; return; }
    while (!copy.empty()) {
        const auto request = copy.front();
        cout << "Resource " << request.resourceId << ": Student "
             << request.studentId << " (" << request.studentName << ")\n";
        copy.pop();
    }
}

void ReservationManager::reportFullSummary() const {
    reportActiveReservations();
    reportResourceUtilization();
    reportMostRequestedResource();
    reportWaitingListStatistics();
}
