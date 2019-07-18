#ifndef __NOXIMSELECTION_FILE_H__
#define __NOXIMSELECTION_FILE_H__

#include "SelectionStrategy.h"
#include "SelectionStrategies.h"
#include "../Router.h"

using namespace std;


class Selection_FILE : SelectionStrategy {
	public:
        int apply(Router * router, const vector < int >&directions, const RouteData & route_data);
        void perCycleUpdate(Router * router);

		static Selection_FILE * getInstance();

	private:
		Selection_FILE(){};
		~Selection_FILE(){};

		static Selection_FILE * selection_FILE;
		static SelectionStrategiesRegister selectionStrategiesRegister;
};

#endif
