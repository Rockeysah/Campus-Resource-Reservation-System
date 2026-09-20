#include "ReservationManager.h"
#include <iostream>
#include <limits>

using namespace std;

static void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    ReservationManager m;

    m.loadResources("data/resources.txt");
    m.loadReservations("data/reservations.txt");

    int c = -1;

    do {
        cout << "\n===== Campus Resource Reservation System =====\n";
        cout << "1. Display resources\n";
        cout << "2. Display availability\n";
        cout << "3. Display active reservations\n";
        cout << "4. Create reservation\n";
        cout << "5. Cancel reservation\n";
        cout << "6. Display waiting list\n";
        cout << "7. Process next waiting request\n";
        cout << "8. Undo most recent cancellation\n";
        cout << "9. Display cancellation history\n";
        cout << "0. Exit\n";
        cout << "Choice: ";

        if (!(cin >> c)) {
            cout << "Invalid menu selection.\n";
            clearInput();
            continue;
        }

        if (c == 1) {
            m.displayResources();
        }
        else if (c == 2) {
            m.displayAvailability();
        }
        else if (c == 3) {
            m.displayReservations();
        }
        else if (c == 4) {
            int id;
            string sid, name, rid, date;

            cout << "Reservation ID: ";
            if (!(cin >> id)) {
                clearInput();
                continue;
            }

            cout << "Student ID: ";
            cin >> sid;

            cout << "Student Name: ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, name);

            cout << "Resource ID: ";
            cin >> rid;

            cout << "Reservation Date: ";
            cin >> date;

            m.createReservation(id, sid, name, rid, date);
        }
        else if (c == 5) {
            int id;

            cout << "Reservation ID to cancel: ";

            if (cin >> id) {
                m.cancelReservation(id);
            }
            else {
                clearInput();
            }
        }
        else if (c == 6) {
            m.displayWaitingList();
        }
        else if (c == 7) {
            if (!m.processNextWaitingRequest()) {
                cout << "No waiting request can be processed right now.\n";
            }
        }
        else if (c == 8) {
            m.undoLastCancellation();
        }
        else if (c == 9) {
            m.displayCancellationHistory();
        }
        else if (c == 0) {
            cout << "Goodbye!\n";
        }
        
        else {
            cout << "Invalid choice.\n";
        }

    } while (c != 0);

    return 0;
}