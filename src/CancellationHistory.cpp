#include "CancellationHistory.h"
#include <iostream>

using namespace std;

void CancellationHistory::addCancellation(
    const Reservation& reservation) {

    cancelledReservations.push(reservation);

    cout << "Reservation added to cancellation history.\n";
}

bool CancellationHistory::restoreLastCancellation(
    Reservation& reservation) {

    if (cancelledReservations.empty()) {
        return false;
    }

    reservation = cancelledReservations.top();

    cancelledReservations.pop();

    return true;
}

void CancellationHistory::displayHistory() const {

    if (cancelledReservations.empty()) {
        cout << "\nCancellation history is empty.\n";
        return;
    }

    cout << "\nCancellation History\n";
    cout << "====================\n";

    Reservation::printHeader();

    stack<Reservation> temp =
        cancelledReservations;

    while (!temp.empty()) {

        temp.top().print();

        temp.pop();
    }

    cout << endl;
}

bool CancellationHistory::isEmpty() const {
    return cancelledReservations.empty();
}

int CancellationHistory::getSize() const {
    return static_cast<int>(
        cancelledReservations.size()
    );
}
