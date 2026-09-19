#ifndef RESERVATION_MANAGER_H
#define RESERVATION_MANAGER_H

#include "Reservation.h"
#include "Resource.h"
#include <vector>
#include <string>

using namespace std;

class ReservationManager {
private:
    struct Node {
        Reservation reservation;
        Node* next;

        Node(const Reservation& reservation);
    };

    Node* head;

    bool reservationIdExists(int reservationId) const;

public:
    ReservationManager();
    ~ReservationManager();

    bool insertReservation(const Reservation& reservation);

    bool removeReservation(int reservationId);

    Reservation* findReservation(int reservationId);

    void displayReservations() const;

    bool validateReservation(
        int reservationId,
        const string& studentId,
        const string& studentName,
        const string& resourceId,
        const string& date,
        const vector<Resource>& resources
    ) const;

    bool createReservation(
        int reservationId,
        const string& studentId,
        const string& studentName,
        const string& resourceId,
        const string& date,
        vector<Resource>& resources
    );

    bool cancelReservation(
        int reservationId,
        vector<Resource>& resources,
        Reservation& cancelledReservation
    );

    bool isEmpty() const;

    int getReservationCount() const;
};

#endif