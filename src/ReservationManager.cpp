#include "ReservationManager.h"
#include <iostream>

using namespace std;

ReservationManager::Node::Node(const Reservation& reservation)
    : reservation(reservation), next(nullptr) {
}

ReservationManager::ReservationManager()
    : head(nullptr) {
}

ReservationManager::~ReservationManager() {
    Node* current = head;

    while (current != nullptr) {
        Node* nextNode = current->next;
        delete current;
        current = nextNode;
    }

    head = nullptr;
}

bool ReservationManager::reservationIdExists(int reservationId) const {
    Node* current = head;

    while (current != nullptr) {
        if (current->reservation.getReservationId() == reservationId) {
            return true;
        }

        current = current->next;
    }

    return false;
}

bool ReservationManager::insertReservation(
    const Reservation& reservation) {

    if (reservationIdExists(
            reservation.getReservationId())) {

        cout << "Error: Reservation ID "
             << reservation.getReservationId()
             << " already exists.\n";

        return false;
    }

    Node* newNode = new Node(reservation);

    newNode->next = head;
    head = newNode;

    return true;
}

Reservation* ReservationManager::findReservation(
    int reservationId) {

    Node* current = head;

    while (current != nullptr) {
        if (current->reservation.getReservationId()
            == reservationId) {

            return &(current->reservation);
        }

        current = current->next;
    }

    return nullptr;
}

bool ReservationManager::removeReservation(
    int reservationId) {

    if (head == nullptr) {
        return false;
    }

    if (head->reservation.getReservationId()
        == reservationId) {

        Node* temp = head;
        head = head->next;

        delete temp;

        return true;
    }

    Node* current = head;

    while (current->next != nullptr) {

        if (current->next->reservation.getReservationId()
            == reservationId) {

            Node* temp = current->next;
            current->next = temp->next;

            delete temp;

            return true;
        }

        current = current->next;
    }

    return false;
}

void ReservationManager::displayReservations() const {

    if (head == nullptr) {
        cout << "\nNo active reservations.\n";
        return;
    }

    cout << "\nActive Reservations\n";
    cout << "===================\n";

    Reservation::printHeader();

    Node* current = head;

    while (current != nullptr) {
        current->reservation.print();
        current = current->next;
    }

    cout << endl;
}

bool ReservationManager::validateReservation(
    int reservationId,
    const string& studentId,
    const string& studentName,
    const string& resourceId,
    const string& date,
    const vector<Resource>& resources
) const {

    if (reservationId <= 0) {
        cout << "Error: Reservation ID must be positive.\n";
        return false;
    }

    if (reservationIdExists(reservationId)) {
        cout << "Error: Reservation ID already exists.\n";
        return false;
    }

    if (studentId.empty()) {
        cout << "Error: Student ID cannot be empty.\n";
        return false;
    }

    if (studentName.empty()) {
        cout << "Error: Student name cannot be empty.\n";
        return false;
    }

    if (resourceId.empty()) {
        cout << "Error: Resource ID cannot be empty.\n";
        return false;
    }

    if (date.empty()) {
        cout << "Error: Reservation date cannot be empty.\n";
        return false;
    }

    bool resourceFound = false;

    for (const Resource& resource : resources) {

        if (resource.getId() == resourceId) {

            resourceFound = true;

            if (!resource.isAvailable()) {
                cout << "Error: Resource "
                     << resourceId
                     << " is currently unavailable.\n";

                return false;
            }

            break;
        }
    }

    if (!resourceFound) {
        cout << "Error: Resource "
             << resourceId
             << " does not exist.\n";

        return false;
    }

    return true;
}

bool ReservationManager::createReservation(
    int reservationId,
    const string& studentId,
    const string& studentName,
    const string& resourceId,
    const string& date,
    vector<Resource>& resources
) {

    if (!validateReservation(
            reservationId,
            studentId,
            studentName,
            resourceId,
            date,
            resources)) {

        return false;
    }

    Reservation reservation(
        reservationId,
        studentId,
        studentName,
        resourceId,
        date
    );

    if (!insertReservation(reservation)) {
        return false;
    }

    for (Resource& resource : resources) {

        if (resource.getId() == resourceId) {

            resource.setAvailable(false);
            resource.incrementTimesReserved();

            break;
        }
    }

    cout << "\nReservation Created Successfully.\n";
    cout << "Reservation ID: " << reservationId << endl;
    cout << "Student: " << studentName << endl;
    cout << "Resource: " << resourceId << endl;
    cout << "Date: " << date << endl;

    return true;
}

bool ReservationManager::cancelReservation(
    int reservationId,
    vector<Resource>& resources,
    Reservation& cancelledReservation
) {

    Reservation* reservation =
        findReservation(reservationId);

    if (reservation == nullptr) {
        cout << "Error: Reservation "
             << reservationId
             << " was not found.\n";

        return false;
    }

    cancelledReservation = *reservation;

    string resourceId =
        reservation->getResourceId();

    if (!removeReservation(reservationId)) {
        return false;
    }

    for (Resource& resource : resources) {

        if (resource.getId() == resourceId) {

            resource.setAvailable(true);

            break;
        }
    }

    cout << "\nReservation "
         << reservationId
         << " Cancelled Successfully.\n";

    return true;
}

bool ReservationManager::isEmpty() const {
    return head == nullptr;
}

int ReservationManager::getReservationCount() const {

    int count = 0;

    Node* current = head;

    while (current != nullptr) {
        count++;
        current = current->next;
    }

    return count;
}