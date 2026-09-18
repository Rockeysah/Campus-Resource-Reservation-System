#include "Reservation.h"
#include <iostream>

using namespace std;

// Adds spaces to the end of text until it is width characters long,
// so the columns of the table lines up and saves headaches. Same thing,
// as Resource.cpp
static string pad(string text, size_t width) {
    while (text.size() < width) text += ' ';
    return text;
}

Reservation::Reservation() : reservationId(0) {}

Reservation::Reservation(int reservationId, const string& studentId, const string& studentName, const string& resourceId, const string& date)
: reservationId(reservationId), studentId(studentId), studentName(studentName), resourceId(resourceId), date(date) {}

int Reservation::getReservationId() const { return reservationId; }
const string& Reservation::getStudentId() const { return studentId; }
const string& Reservation::getStudentName() const { return studentName; }
const string& Reservation::getResourceId() const { return resourceId; }
const string& Reservation::getDate() const { return date; }

void Reservation::printHeader() {
    cout << pad("Res ID", 8) << pad("Student ID", 12) << pad("Student Name", 24)
         << pad("Resource", 10) << "Date\n"
              << string(66, '-') << '\n';
}

void Reservation::print() const {
    cout << pad(to_string(reservationId), 8) << pad(studentId, 12) << pad(studentName, 24)
         << pad(resourceId, 10) << date << '\n';
}