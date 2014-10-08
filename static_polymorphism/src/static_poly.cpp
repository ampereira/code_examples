#include <cstdlib>
#include <iostream>
#include <sys/time.h>
#include <string>
#include "spa.h"
#include "particle.h"
#include "simple_event.h"

using namespace std;

vector<SimpleEvent> events;

void cuts (void) {
	for (auto event : events) {
		// cut 1
		if (!event.pv1.size() > 2)
			continue;

		if (!event.LumiBlock > 2456)
			continue;

		event.calculations();
	}
}

int main (int argc, char *argv[]) {
	Measure m;
	m.addStatistic(Statistic::Average);
	m.addStatistic(Statistic::Minimum);
	m.addStatistic(Statistic::StdDev);
	m.addStatistic(Statistic::Maximum);

	if (argc < 3) {
		cerr << "Number of runs or events is missing..." << endl;
		return -1;
	}

	unsigned nevents = stoul(argv[1]);
	unsigned runs = stoul(argv[2]);

	for (unsigned i = 0; i < nevents; ++i) {
		SimpleEvent ev;

		events.push_back(ev);
	}

	m.start();

	for (unsigned i = 0; i < runs; ++i)
		cuts();

	m.stop();

	m.report(Report::Verbose);

	return 1;
}