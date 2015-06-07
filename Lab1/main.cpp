#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

#define TICKS_PER_SECOND 1000000

class Simulation {
  private:
    struct Packet {
        int arriveTime;
        int remainingServiceTime;
        Packet(int arrive, int serviceTime) : arriveTime(arrive), remainingServiceTime(serviceTime) { }
    };
    int n, lambda, L, C, K;
    int serviceTime;
    int currTick;
    int droppedPackets, totalPackets;
    queue<Packet> buffer;
    vector <int> numberOfPackets;
    vector <int> sojournTimes;
    int idleTicks;
    int markovianTime();
    void arrival();
    void departure();
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
    serviceTime = (L * TICKS_PER_SECOND) / C;
    currTick = 0;
    srand(time(0));
}

int Simulation::markovianTime() {
    double u = (double) rand() / RAND_MAX;
    double x = (-log(1 - u)) / lambda;
    x *= TICKS_PER_SECOND;
    return x;
}

void Simulation::arrival() {
    totalPackets++;
    if (buffer.size() == (size_t) K) {
        droppedPackets++;
        return;
    }
    Packet packet(currTick, serviceTime);
    buffer.push(packet);
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

void Simulation::startSimulation() {
    int timeUntilNextPacket = markovianTime();
    for (currTick = 1; currTick <= n; currTick++) {
        timeUntilNextPacket--;
        if (timeUntilNextPacket <= 0) {
            arrival();
            timeUntilNextPacket = markovianTime();
        }
        departure();
    }
}

double Simulation::getEN() {
    long sum = 0;
    for (size_t i = 0; i < numberOfPackets.size(); i++) {
        sum += numberOfPackets.at(i);
    }
    return (double) sum / numberOfPackets.size();
}

double Simulation::getET() {
    long sum = 0;
    for (size_t i = 0; i < sojournTimes.size(); i++) {
        sum += sojournTimes.at(i);
    }
    return (double) sum / sojournTimes.size();
}

double Simulation::getPIdle() {
    return (double) idleTicks / n;
}

double Simulation::getPLoss() {
    return (double) droppedPackets / totalPackets;
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
