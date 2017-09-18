#include "location.h"

int main() {
    Location *location = init_location(20, 20);
    file_read_matrix(location, "location.mtr");
    // display_matrix(location);
    // init_matrix(location);
    // randomize_matrix(location, 100);
    // randomize_rubbish(location, 10);
    // init_agent(location, 1);    
    start(location, 100000);
    free_location(location);
    return 0;
}