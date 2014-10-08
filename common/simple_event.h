#include <cstdlib>

class SimpleEvent {
public:
	int Isub;
	int LumiBlock; 
	int RunNumber; 
	int EveNumber; 

	double MissPx;
	double MissPy;
	double MissPt;
	double Weight;

	Particle p1;
	Particle p2;
	std::vector<Particle> pv1;
	std::vector<Particle> pv2;

	SimpleEvent (void);

	void calculations (void);
	virtual void vcalculations (void);
};

SimpleEvent::SimpleEvent (void) {
	Isub = rand();
	LumiBlock = rand(); 
	RunNumber = rand(); 
	EveNumber = rand(); 

	MissPx = rand();
	MissPy = rand();
	MissPt = rand();
	Weight = rand();

	p1.fill();
	p2.fill();

	unsigned length = rand() % 10 + 1;

	for (unsigned i = 0; i < length; ++i) {
		pv1.push_back(p1);
		pv2.push_back(p2);
	}
}

void SimpleEvent::calculations (void) {
	int sum = 0;

	for (unsigned i = 0; i < 1000; ++i) {
		sum += MissPx*MissPy*MissPt + LumiBlock*RunNumber + Weight;

		for (auto p : pv1)
			sum += p.mass();

		for (auto p : pv2)
			sum += p.mass();

		sum += p1.mass() + p2.mass();
	}

	Isub = sum;
}

void SimpleEvent::vcalculations (void) {

}