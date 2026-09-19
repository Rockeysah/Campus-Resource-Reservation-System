#include "WaitingList.h"
#include <iostream>

using namespace std;

void WaitingList::addToWaitingList(
    const Reservation& reservation) {

    waitingReservations.push(reservation);

    cout << "Student added to waiting list.\n";
}

bool WaitingList::removeFromWaitingList(
    Reservation& reservation) {

    if (waitingReservations.empty()) {
        return false;
    }

    reservation = waitingReservations.front();

    waitingReservations.pop();

    return true;
}

void WaitingList::displayWaitingList() const {

    if (waitingReservations.empty()) {
        cout << "\nWaiting list is empty.\n";
        return;
    }

    cout << "\nWaiting List\n";
    cout << "============\n";

    Reservation::printHeader();

    queue<Reservation> temp =
        waitingReservations;

    while (!temp.empty()) {

        temp.front().print();

        temp.pop();
    }

    cout << endl;
}

bool WaitingList::isEmpty() const {
    return waitingReservations.empty();
}

int WaitingList::getSize() const {
    return static_cast<int>(
        waitingReservations.size()
    );
}

