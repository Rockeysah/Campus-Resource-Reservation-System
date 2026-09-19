#ifndef WAITING_LIST_H
#define WAITING_LIST_H

#include "Reservation.h"
#include <queue>

using namespace std;

class WaitingList {
private:
    queue<Reservation> waitingReservations;

public:
    void addToWaitingList(const Reservation& reservation);

    bool removeFromWaitingList(
        Reservation& reservation
    );

    void displayWaitingList() const;

    bool isEmpty() const;

    int getSize() const;
};

#endif
