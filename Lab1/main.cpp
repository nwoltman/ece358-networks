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
    int currTick;
    int droppedPackets, totalPackets;
    queue<Packet> buffer;
    Packet nextPacket;
    vector <int> numberOfPackets;
    vector <int> sojournTimes;
    int idleTicks;
    void arrival();
    void departure();
    void createPacket();
    void addPacket();
  public:
    Simulation(int n, int lambda, int L, int C, int K = -1);
    void startSimulation();
    void computePerformances();
    double getEN();
    double getET();
    double getPIdle();
    double getPLoss();
};

Simulation::Simulation(int n_, int lambda_, int L_, int C_, int K_)
  : n(n_), lambda(lambda_), L(L_), C(C_), K(K_)
{
    serviceTime = (L * MILLION) / C;
    currTick = 0;
    srand(time(0));
    createPacket();
}

void Simulation::createPacket() {
    double u = (double) rand() / RAND_MAX;
    double x = (-log(1 - u)) / lambda;
    x *= MILLION;
    nextPacket.waitTime = x;
    totalPackets++;
}

void Simulation::addPacket() {
    nextPacket.arriveTime = currTick;
    nextPacket.remainingServiceTime = serviceTime;
    if (buffer.size() == K) {
        droppedPackets++;
        return;
    }
    buffer.push(nextPacket);
}

void Simulation::startSimulation() {
    for (currTick = 1; currTick <= n; currTick++) {
        arrival();
        departure();
    }
}

void Simulation::arrival() {
    nextPacket.waitTime--;
    if (nextPacket.waitTime == 0) {
        addPacket();
        createPacket();
    }
}

void Simulation::departure() {
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

double Simulation::getPIdle() {
    double avg = idleTicks;
    avg /= n;
    return avg;
}

double Simulation::getPLoss() {
    double avg = droppedPackets;
    avg /= totalPackets;
    return avg;
}

void Simulation::computePerformances() {
    cout << "E[N] = " << getEN() << endl;
    cout << "E[T] = " << getET() << endl;
    cout << "P_idle = " << getPIdle() << endl;
    if (K > -1) {
        cout << "P_loss = " << getPLoss() << endl;
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
