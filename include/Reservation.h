#ifndef RESERVATION_H
#define RESERVATION_H

#include <string>
using namespace std;

class Reservation {
public:
    Reservation();
    Reservation(int reservationId, const string& studentId, const string& studentName, const string& resourceId, const string& date);

    int getReservationId() const;
    const string& getStudentId() const;
    const string& getStudentName() const;
    const string& getResourceId() const;
    const string& getDate() const;

    void print() const;
    static void printHeader();

private:
    int reservationId;
    string studentId;
    string studentName;
    string resourceId;
    string date;
};

#endif