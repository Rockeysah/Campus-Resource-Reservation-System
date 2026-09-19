#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Resource.h"
#include "Reservation.h"
#include "ReservationManager.h"
#include "WaitingList.h"
#include "CancellationHistory.h"

using namespace std;

void loadResources(
    const string& filename,
    vector<Resource>& resources) {

    ifstream file(filename);

    if (!file) {
        cout << "Error: Could not open resource file.\n";
        return;
    }

    string line;

    while (getline(file, line)) {

        if (line.empty()) {
            continue;
        }

        stringstream ss(line);

        string id;
        string name;
        string type;
        string status;

        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, type, ',');
        getline(ss, status, ',');

        bool available =
            (status == "Available" ||
             status == "available" ||
             status == "1");

        resources.push_back(
            Resource(id, name, type, available)
        );
    }

    file.close();
}

void displayResources(
    const vector<Resource>& resources) {

    if (resources.empty()) {
        cout << "\nNo resources available.\n";
        return;
    }

    cout << "\nResources\n";
    cout << "=========\n";

    Resource::printHeader();

    for (const Resource& resource : resources) {
        resource.print();
    }

    cout << endl;
}

bool resourceExists(
    const vector<Resource>& resources,
    const string& resourceId) {

    for (const Resource& resource : resources) {

        if (resource.getId() == resourceId) {
            return true;
        }
    }

    return false;
}

bool resourceIsAvailable(
    const vector<Resource>& resources,
    const string& resourceId) {

    for (const Resource& resource : resources) {

        if (resource.getId() == resourceId) {
            return resource.isAvailable();
        }
    }

    return false;
}

void restoreReservation(
    ReservationManager& reservationManager,
    CancellationHistory& cancellationHistory,
    vector<Resource>& resources) {

    Reservation reservation;

    if (!cancellationHistory.restoreLastCancellation(
            reservation)) {

        cout << "\nNo cancelled reservations to restore.\n";
        return;
    }

    if (!resourceExists(
            resources,
            reservation.getResourceId())) {

        cout << "\nResource no longer exists.\n";
        return;
    }

    if (!resourceIsAvailable(
            resources,
            reservation.getResourceId())) {

        cout << "\nThe resource is currently unavailable.\n";
        return;
    }

    if (reservationManager.insertReservation(reservation)) {

        for (Resource& resource : resources) {

            if (resource.getId() ==
                reservation.getResourceId()) {

                resource.setAvailable(false);
                resource.incrementTimesReserved();

                break;
            }
        }

        cout << "\nReservation restored successfully.\n";
        cout << "Reservation ID: "
             << reservation.getReservationId()
             << endl;
    }
}

int main() {

    vector<Resource> resources;

    loadResources(
        "data/resources.txt",
        resources
    );

    ReservationManager reservationManager;
    WaitingList waitingList;
    CancellationHistory cancellationHistory;

    int choice = 0;

    while (choice != 9) {

        cout << "\n===== Campus Resource Reservation System =====\n";
        cout << "1. Display Resources\n";
        cout << "2. Display Resource Availability\n";
        cout << "3. Create Reservation\n";
        cout << "4. Cancel Reservation\n";
        cout << "5. Display Active Reservations\n";
        cout << "6. Display Waiting List\n";
        cout << "7. Display Cancellation History\n";
        cout << "8. Restore Last Cancellation\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";

        

     if (!(cin >> choice)) {
    cin.clear();
    cin.ignore(10000, '\n');
    cout << "Invalid choice. Please enter a number from 1 to 9.\n";
    continue;
}
       if (choice == 1) {
    displayResources(resources);
       }

        else if (choice == 2) {

            displayResources(resources);
        }

        else if (choice == 3) {

            int reservationId;
            string studentId;
            string studentName;
            string resourceId;
            string date;

            cout << "\nEnter reservation ID: ";
            cin >> reservationId;

            cout << "Enter student ID: ";
            cin >> studentId;

            cin.ignore();

            cout << "Enter student name: ";
            getline(cin, studentName);

            cout << "Enter resource ID: ";
            cin >> resourceId;

            cout << "Enter date: ";
            cin >> date;

            if (!resourceExists(
                    resources,
                    resourceId)) {

                cout << "\nError: Resource does not exist.\n";
            }

            else if (!resourceIsAvailable(
                         resources,
                         resourceId)) {

                Reservation waitingReservation(
                    reservationId,
                    studentId,
                    studentName,
                    resourceId,
                    date
                );

                waitingList.addToWaitingList(
                    waitingReservation
                );

                cout << "Resource is unavailable.\n";
                cout << "Reservation added to waiting list.\n";
            }

            else {

                reservationManager.createReservation(
                    reservationId,
                    studentId,
                    studentName,
                    resourceId,
                    date,
                    resources
                );
            }
        }

        else if (choice == 4) {

            int reservationId;

            cout << "\nEnter reservation ID to cancel: ";
            cin >> reservationId;

            Reservation cancelledReservation;

            if (reservationManager.cancelReservation(
                    reservationId,
                    resources,
                    cancelledReservation)) {

                cancellationHistory.addCancellation(
                    cancelledReservation
                );

                Reservation nextWaitingReservation;

                if (waitingList.removeFromWaitingList(
                        nextWaitingReservation)) {

                    cout << "\nProcessing waiting list.\n";

                    if (reservationManager.createReservation(
                            nextWaitingReservation.getReservationId(),
                            nextWaitingReservation.getStudentId(),
                            nextWaitingReservation.getStudentName(),
                            nextWaitingReservation.getResourceId(),
                            nextWaitingReservation.getDate(),
                            resources)) {

                        cout << "Waiting-list reservation processed.\n";
                    }
                    else {
                        waitingList.addToWaitingList(
                            nextWaitingReservation
                        );
                    }
                }
            }
        }

        else if (choice == 5) {

            reservationManager.displayReservations();
        }

        else if (choice == 6) {

            waitingList.displayWaitingList();
        }

        else if (choice == 7) {

            cancellationHistory.displayHistory();
        }

        else if (choice == 8) {

            restoreReservation(
                reservationManager,
                cancellationHistory,
                resources
            );
        }

        else if (choice == 9) {

            cout << "\nExiting program.\n";
        }

        else {

            cout << "\nInvalid choice. Please try again.\n";
        }
    }

    return 0;
}