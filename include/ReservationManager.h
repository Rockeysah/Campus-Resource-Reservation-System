#ifndef RESERVATION_MANAGER_H
#define RESERVATION_MANAGER_H

#include "Resource.h"
#include "Reservation.h"
#include <string>
#include <vector>
#include <queue>
#include <stack>

using namespace std;

class ReservationManager {
private:
    vector<Resource> resources;

    struct ReservationNode {
        Reservation reservation;
        ReservationNode* next;

        ReservationNode(const Reservation& r)
            : reservation(r), next(nullptr) {}
    };

    ReservationNode* reservationHead;

    struct WaitingRequest {
        string studentId;
        string studentName;
        string resourceId;
        string date;
    };

    queue<WaitingRequest> waitingQueue;
    stack<Reservation> cancellationHistory;

        Resource* findResource(const string& id);
    ReservationNode* findReservationNode(int id) const;

    bool studentHasReservation(const string& studentId,
                               const string& resourceId) const;

    bool reservationIdInCancellationHistory(int id) const;
    int generateNextReservationId() const;

    void deleteAllNodes();

public:
    ReservationManager();
    ~ReservationManager();

    bool loadResources(const string& filename);
    bool loadReservations(const string& filename);
    bool saveReservations(const string& filename) const;
    void displayResources() const;
    void displayAvailability() const;
    void displayReservations() const;

    bool createReservation(int reservationId,
                           const string& studentId,
                           const string& studentName,
                           const string& resourceId,
                           const string& date);

    bool cancelReservation(int reservationId);

    void addToWaitingList(const string& studentId,
                          const string& studentName,
                          const string& resourceId,
                          const string& date);

    bool processNextWaitingRequest();
    bool undoLastCancellation();

    void displayWaitingList() const;
    void displayCancellationHistory() const;

    bool reservationExists(int id) const;
    int activeReservationCount() const;

    // Added search, sorting, and reporting features.
    void searchResourceById() const;
    void searchReservationOrStudent() const;
    void sortResourcesByName() const;
    void sortResourcesByPopularity() const;
    void sortReservationsByDate() const;
    void reportActiveReservations() const;
    void reportResourceUtilization() const;
    void reportMostRequestedResource() const;
    void reportWaitingListStatistics() const;
    void reportFullSummary() const;
};

#endif