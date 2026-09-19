#ifndef CANCELLATION_HISTORY_H
#define CANCELLATION_HISTORY_H

#include "Reservation.h"
#include <stack>

using namespace std;

class CancellationHistory {
private:
    stack<Reservation> cancelledReservations;

public:
    void addCancellation(
        const Reservation& reservation
    );

    bool restoreLastCancellation(
        Reservation& reservation
    );

    void displayHistory() const;

    bool isEmpty() const;

    int getSize() const;
};

#endif
