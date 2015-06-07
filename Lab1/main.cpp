#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

#define MILLION 1000000

class Simulation {
  private:
    struct Packet {
        int arriveTime;
        int waitTime;
        int remainingServiceTime;
    };
    int n, lambda, L, C, K;
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
    Simulation(int n, int lambda, int L, int C, int K = -1);
    void startSimulation ();
    void computePerformances();
    double getEN();
    double getET();
    double getPidle();
    double getPloss();
};

Simulation::Simulation(int n_, int lambda_, int L_, int C_, int K_)
  : n(n_), lambda(lambda_), L(L_), C(C_), K(K_)
{
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
    if (buffer.size() == K) {
        droppedPacks++;
        return;
    }
    buffer.push(nextPacket);
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
    long sum = 0;
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
    if (K > -1) {
        cout << "P_loss = " << getPloss() << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5 && argc != 6) {
        cerr << "Usage: " << argv[0] << " n lambda L C [ K ]" << endl;
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    int lambda = atoi(argv[2]);
    int L = atoi(argv[3]);
    int C = atoi(argv[4]);
    int K = -1;

    if (argc == 6) {
        K = atoi(argv[5]);
    }

    Simulation simulation(n, lambda, L, C, K);
    simulation.startSimulation();
    simulation.computePerformances();

    return 0;
}
