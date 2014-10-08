#include <cmath>
#include <cstdlib>

class Particle {
public:
	double px;
	double py;
	double pz;
	double e;

	double mass (void);
	void fill (void);
	virtual double vmass (void);
};

void Particle::fill (void) {
	px = rand();
	py = rand();
	pz = rand();
	e  = rand();
}

double Particle::mass (void) {
	return sqrt(px*px + py*py + pz*pz + e*e);
}

double Particle::vmass (void) {
	
}