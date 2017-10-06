#include "location.h"

int main() {
    Location *location = init_location(20, 20);
    init_matrix_in_file(location, "location.mtr", 50, 10, 1);
    free_location(location);
    return 0;
}