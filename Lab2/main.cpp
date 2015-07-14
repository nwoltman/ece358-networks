#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
using namespace std;

#define TICKS_PER_SECOND 100000
#define NUMBER_OF_ATTEMPTS 10

int busyCounter = 0;

struct Packet {
    int attempts;
    int remainingServiceTime;
    double delayTime;
    Packet(int serviceTime) : remainingServiceTime(serviceTime), attempts(0), delayTime(0) { }
};

class Station {
public:
        void UpdateSimulation();
        virtual void busyChannel() = 0;
        virtual void emptyChannel() = 0;
        int getTransmittedPackets();
        int getTotalDelay();
        Station(int A, int W, int L) : arrivalRate(A), lanSpeed(W) {
            //packetTime, in ticks, assuming W is in Mbps
            packetTime = (8 * L) / lanSpeed;
            jammingTime = 0;
            waitingTime = 0;
            totalDelay = 0;
			state = IDLE;
            transmittedPackets = 0;
            sensingTimeReset();
            nextPacket = getTimeInterval();
        }
protected:
    vector<Packet> buffer;
    int packetTime;
    int jammingTime;
    int waitingTime;
    int lanSpeed;
    int arrivalRate;
    int nextPacket;
    int sensingTime;
    int transmittedPackets;
    int totalDelay;
    void sensingTimeReset();
    void jammingTimeReset();
    void waitingTimeReset(int r);
    void resetState();
    void backoff();
    int getTimeInterval();
    enum States {IDLE, SENSING, TRANSMITTING, JAMMING, WAITING, DEFERRED};
    States state;
};

int Station::getTimeInterval() {
    double u = (double) rand() / (double) RAND_MAX;
    double x = -1 / ((double) arrivalRate) * log(1 - u);
    x *= TICKS_PER_SECOND;
    return round(x);
}

void Station::sensingTimeReset() {
    //If W is in Mbps and TICKS_PER_SECOND = 10^6, sensingTime = 96 / W ticks
    sensingTime = 96 / lanSpeed;
}

void Station::jammingTimeReset() {
    //If W is in Mbps and TICKS_PER_SECOND = 10^6, jammingTime = 48 / W ticks
    jammingTime = 48 / lanSpeed;
}

void Station::waitingTimeReset(int r) {
    //If W is in Mbps and TICKS_PER_SECOND = 10^6, backingTime = (512 * r) / W ticks
    waitingTime = (512 * r) / lanSpeed;
}

int Station::getTransmittedPackets() {
    return transmittedPackets;
}

int Station::getTotalDelay() {
    return totalDelay;
}

void Station::resetState() {
    if(buffer.empty())
        state = IDLE;
    else
        state = SENSING;
}

void Station::backoff() {
    buffer.front().attempts++;
    if(buffer.front().attempts > NUMBER_OF_ATTEMPTS) {
        buffer.erase(buffer.begin());
        resetState();
    } else {
        int r = rand() % (int) (pow(2, NUMBER_OF_ATTEMPTS) - 1);
        waitingTimeReset(r);
        state = WAITING;
    }
}

void Station::UpdateSimulation() {
    nextPacket--;
    if(nextPacket == 0) {
        Packet p(packetTime);
        buffer.push_back(p);
        nextPacket = getTimeInterval();
    }
    for (int i = 0; i < buffer.size(); i++)
        buffer.at(i).delayTime++;
    switch(state) {
        case IDLE: {
            if(!buffer.empty())
                state = SENSING;
        }
        break;
        case SENSING: {
            if(busyCounter == 0)
                sensingTime--;
            else {
                busyChannel();
                sensingTimeReset();
            }
            if(sensingTime == 0) {
                emptyChannel();
            }
        }
        break;
        case TRANSMITTING: {
            buffer.front().remainingServiceTime--;
            if (buffer.front().remainingServiceTime == 0) {
                busyCounter--;
                resetState();
                transmittedPackets++;
                totalDelay += buffer.front().delayTime;
                buffer.erase(buffer.begin());

            }
            if(busyCounter > 1) {
                state = JAMMING;
                buffer.front().remainingServiceTime = packetTime;
            }
        }
        break;
        case JAMMING: {
            jammingTime--;
            if(jammingTime == 0) {
                busyCounter--;
                jammingTimeReset();
                backoff();
            }
        }
        break;
        case WAITING: {
            waitingTime--;
            if(waitingTime == 0)
                resetState();
        }
        break;
        //P-Persistent Only
        case DEFERRED: {
            if(busyCounter == 0)
                emptyChannel();
            else
                backoff();
        }
    }
}

class OnePersistentStation : public Station {
public:
    OnePersistentStation(int A, int W, int L) : Station(A,W,L) {}
    void busyChannel();
    void emptyChannel();
};

void OnePersistentStation::busyChannel() {
    return;
}

void OnePersistentStation::emptyChannel() {
    state = TRANSMITTING;
    busyCounter++;
    sensingTimeReset();
}

class NonPersistentStation : public Station {
public:
    NonPersistentStation(int A, int W, int L) : Station(A,W,L) {}
    void busyChannel();
    void emptyChannel();
};

void NonPersistentStation::busyChannel() {
    int r = rand() % (int) (pow(2, NUMBER_OF_ATTEMPTS) - 1);
    waitingTimeReset(r);
    state = WAITING;
}

void NonPersistentStation::emptyChannel() {
    state = TRANSMITTING;
    busyCounter++;
    sensingTimeReset();
}

class PPersistentStation : public Station {
private:
    float p;
public:
    PPersistentStation(int A, int W, int L, float P) : Station(A,W,L), p(P) {}
    void busyChannel();
    void emptyChannel();
};

void PPersistentStation::busyChannel() {
    int r = rand() % (int) (pow(2, NUMBER_OF_ATTEMPTS) - 1);
    waitingTimeReset(r);
    state = WAITING;
}

void PPersistentStation::emptyChannel() {
    double u = (double) rand() / (double) RAND_MAX;
    if (u < p) {
        state = TRANSMITTING;
        busyCounter++;
        sensingTimeReset();
    } else {
        state = DEFERRED;
    }
}

class Simulation {
  private:
    int T, N;
    vector<Station *> stations;
  public:
    Simulation(int T_, int N_, int A_, int W_, int L_, float P_);
    void startSimulation();
    void computePerformances();
};

Simulation::Simulation(int T_, int N_, int A_, int W_, int L_, float P_)
  : T(T_), N(N_)
{
    Station* s;
    if (P_ == 1) {
        for (int i = 0; i < N_; i++) {
            s = new OnePersistentStation(A_, W_, L_);
            stations.push_back(s);
        }
    } else if (P_ == -1) {
        for (int i = 0; i < N; i++) {
            s = new NonPersistentStation(A_, W_, L_);
            stations.push_back(s);
        }
    } else {
        for (int i = 0; i < N; i++) {
            s = new PPersistentStation(A_, W_, L_, P_);
            stations.push_back(s);
        }
    }
}

void Simulation::startSimulation() {
    for (int currTick = 1; currTick <= T * TICKS_PER_SECOND; currTick++) {
        for (int i = 0; i < N; i++)
            stations.at(i)->UpdateSimulation();
    }
}

void Simulation::computePerformances() {
	double packs = 0, delay = 0;
	for (int i = 0; i < N; i++) {
		packs += stations.at(i)->getTransmittedPackets();
		delay += stations.at(i)->getTotalDelay();
	}
	delay /= TICKS_PER_SECOND;
	delay /= packs;
	packs /= T;

	cout << "Throughput = " << packs << " packets per second" << endl;
	cout << "Avg delay = " << delay << " seconds per packet" << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 7) {
        cerr << "Usage: " << argv[0] << " T N A W L P" << endl << "Enter P = 1 for 1-persistent,  P = -1 for non-persistent, P = p for P-persistent";
        exit(EXIT_FAILURE);
    }

    int T = atoi(argv[1]);
    int N = atoi(argv[2]);
    int A = atoi(argv[3]);
    int W = atoi(argv[4]);
    int L = atoi(argv[5]);
    float P = atof(argv[6]);

	  Simulation simulation(T, N, A, W, L, P);
    simulation.startSimulation();
    simulation.computePerformances();

    return 0;
}
