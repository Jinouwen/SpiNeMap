#include "Selection_FILE.h"
#include "../GlobalSelectionTable.h"

SelectionStrategiesRegister Selection_FILE::selectionStrategiesRegister("FILE", getInstance());

Selection_FILE * Selection_FILE::selection_FILE = 0;

Selection_FILE * Selection_FILE::getInstance() {
	if ( selection_FILE == 0 )
		selection_FILE = new Selection_FILE();
    
	return selection_FILE;
}

int Selection_FILE::apply(Router * router, const vector < int >&directions, const RouteData & route_data){
    assert(directions.size()!=0);

//    return router->selection_table->getSelectionPort(route_data);
    return 0;

}

void Selection_FILE::perCycleUpdate(Router * router){ }
