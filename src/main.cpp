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
        cout << "\n========================================\n";
        cout << "      CAMPUS RESOURCE RESERVATION\n";
        cout << "========================================\n\n";
        cout << "  [1]  Display Resources\n";
        cout << "  [2]  Create Reservation\n";
        cout << "  [3]  Cancel Reservation\n\n";
        cout << "  ----------- MANAGEMENT ---------------\n\n";
        cout << "  [4]  Waiting List\n";
        cout << "  [5]  Undo Cancellation\n\n";
        cout << "  ----------- TOOLS -------------------\n\n";
        cout << "  [6]  Search\n";
        cout << "  [7]  Sort\n";
        cout << "  [8]  Generate Report\n\n";
        cout << "  [9]  Exit\n\n";
        cout << "========================================\n";
        cout << "Enter your choice: ";

        if (!(cin >> c)) {
            cout << "Invalid menu selection.\n";
            clearInput();
            continue;
        }

        if (c == 1) {
            m.displayAvailability();
        } else if (c == 2) {
            int id;
            string sid, name, rid, date;
            cout << "Reservation ID: ";
            if (!(cin >> id)) { clearInput(); continue; }
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
        } else if (c == 3) {
            int id;
            cout << "Reservation ID to cancel: ";
            if (cin >> id) m.cancelReservation(id);
            else clearInput();
        } else if (c == 4) {
            int option;
            do {
                cout << "\n----- Waiting List Menu -----\n";
                cout << "1. Display Waiting List\n";
                cout << "2. Process Next Waiting Request\n";
                cout << "0. Back to Main Menu\n";
                cout << "Choice: ";
                if (!(cin >> option)) { clearInput(); option = -1; }
                if (option == 1) m.displayWaitingList();
                else if (option == 2) {
                    if (!m.processNextWaitingRequest())
                        cout << "No waiting request can be processed right now.\n";
                } else if (option != 0) cout << "Invalid choice.\n";
            } while (option != 0);
        } else if (c == 5) {
            m.undoLastCancellation();
        } else if (c == 6) {
            int option;
            do {
                cout << "\n----- Search Menu -----\n";
                cout << "1. Search Resource by ID\n";
                cout << "2. Search Reservation or Student ID\n";
                cout << "0. Back to Main Menu\n";
                cout << "Choice: ";
                if (!(cin >> option)) { clearInput(); option = -1; }
                if (option == 1) m.searchResourceById();
                else if (option == 2) m.searchReservationOrStudent();
                else if (option != 0) cout << "Invalid choice.\n";
            } while (option != 0);
        } else if (c == 7) {
            int option;
            do {
                cout << "\n----- Sort Menu -----\n";
                cout << "1. Sort Resources by Name\n";
                cout << "2. Sort Resources by Popularity\n";
                cout << "3. Sort Reservations by Date\n";
                cout << "0. Back to Main Menu\n";
                cout << "Choice: ";
                if (!(cin >> option)) { clearInput(); option = -1; }
                if (option == 1) m.sortResourcesByName();
                else if (option == 2) m.sortResourcesByPopularity();
                else if (option == 3) m.sortReservationsByDate();
                else if (option != 0) cout << "Invalid choice.\n";
            } while (option != 0);
        } else if (c == 8) {
            int option;
            do {
                cout << "\n----- Reports Menu -----\n";
                cout << "1. Active Reservations\n";
                cout << "2. Resource Utilization\n";
                cout << "3. Most Requested Resource\n";
                cout << "4. Waiting-List Statistics\n";
                cout << "5. Cancellation History\n";
                cout << "6. Full Report\n";
                cout << "0. Back to Main Menu\n";
                cout << "Choice: ";
                if (!(cin >> option)) { clearInput(); option = -1; }
                if (option == 1) m.displayReservations();
                else if (option == 2) m.reportResourceUtilization();
                else if (option == 3) m.reportMostRequestedResource();
                else if (option == 4) m.reportWaitingListStatistics();
                else if (option == 5) m.displayCancellationHistory();
                else if (option == 6) m.reportFullSummary();
                else if (option != 0) cout << "Invalid choice.\n";
            } while (option != 0);
        } else if (c == 9) {
    if (!m.saveReservations("data/reservations.txt")) {
        cout << "Error: Could not save reservations.\n";
    }
    cout << "Goodbye!\n";
} else {
            cout << "Invalid choice.\n";
        }
    } while (c != 9);

    return 0;
}