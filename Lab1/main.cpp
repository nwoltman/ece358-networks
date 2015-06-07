#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

#define TYPE_INFINITE 1
#define TYPE_K 2
#define MILLION 1000000

class Simulation {
  private:
    struct Packet {
        int arriveTime;
        int waitTime;
        int remainingServiceTime;
    };
    int n, lambda, L, C, K, queueType;
    int serviceTime;
    int currTick, droppedPacks, totalPacks;
    queue<Packet> buffer;
    Packet nextPacket;
    vector <int> numberOfPackets;
    vector <int> sojournTimes;
    int idleTicks;
    void Arrival ();
    void Departure ();
    void CreatePacket();
    void AddPacket();
  public:
    Simulation();
    void startSimulation ();
    void computePerformances();
    double getEN();
    double getET();
    double getPidle();
    double getPloss();
};

Simulation::Simulation() {
        do {
            cout << "Enter "<<TYPE_INFINITE<<" for M/D/1 queue, "<<TYPE_K<<" for M/D/1/K queue\n";
            cin >> queueType;
            if ((queueType != TYPE_INFINITE) && (queueType != TYPE_K))
                cout << "Invalid input\n";
        } while ((queueType != TYPE_INFINITE) && (queueType != TYPE_K));
        cout << "Enter simulation length, n\n";
        cin >> n;
        cout << "Enter average number of packets, lambda\n";
        cin >> lambda;
        cout << "Enter packet length, L\n";
        cin >> L;
        cout << "Enter transmission rate, C\n";
        cin >> C;
        if (queueType == TYPE_K) {
            cout << "Enter buffer size, K\n";
            cin >> K;
        }
        serviceTime = (L * MILLION) / C;
        currTick = 0;
        srand(time(0));
        CreatePacket();
}

void Simulation::CreatePacket() {
    double x = ((double) rand() / (RAND_MAX));
    x = (-log(x)) / (double) lambda;
    x *= MILLION;
    nextPacket.waitTime = x;
    totalPacks++;
}

void Simulation::AddPacket() {
    nextPacket.arriveTime = currTick;
    nextPacket.remainingServiceTime = serviceTime;
    if ((queueType == TYPE_INFINITE) || (buffer.size() < K)) {
        buffer.push(nextPacket);
        return;
    }
    droppedPacks++;
}

void Simulation::startSimulation() {
    for (currTick = 1; currTick <= n; currTick++) {
        Arrival();
        Departure();
    }
}

void Simulation::Arrival() {
    nextPacket.waitTime--;
    if (nextPacket.waitTime == 0) {
        AddPacket();
        CreatePacket();
    }
}

void Simulation::Departure() {
    numberOfPackets.push_back(buffer.size());
    if (buffer.empty()) {
        idleTicks++;
        return;
    }
    buffer.front().remainingServiceTime--;
    if (buffer.front().remainingServiceTime == 0) {
        sojournTimes.push_back(currTick - buffer.front().arriveTime);
        buffer.pop();
    }
}


double Simulation::getEN() {
    long  sum = 0;
    for (int i = 0; i < numberOfPackets.size(); i++) {
        sum += numberOfPackets.at(i);
    }
    double avg = sum;
    avg /= numberOfPackets.size();
    return avg;
}

double Simulation::getET() {
    long sum = 0;
    for (int i = 0; i < sojournTimes.size(); i++) {
        sum += sojournTimes.at(i);
    }
    double avg = sum;
    avg /= sojournTimes.size();
    return avg;
}

double Simulation::getPidle() {
    double avg = idleTicks;
    avg /= n;
    return avg;
}

double Simulation::getPloss() {
    double avg = droppedPacks;
    avg /= totalPacks;
    return avg;
}

void Simulation::computePerformances() {
    cout << "E[N] = " << getEN() << endl;
    cout << "E[T] = " << getET() << endl;
    cout << "P_idle = " << getPidle() << endl;
    if (queueType == TYPE_K) {
        cout << "P_loss = " << getPloss() << endl;
    }
}

int main() {
    Simulation x;
    x.startSimulation();
    x.computePerformances();
    return 0;
}
