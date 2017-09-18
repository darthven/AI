#include "location.h"

int main() {
    Location *location = init_location(20, 20);
    file_write_matrix(location, "location.mtr", 50, 10, 1);
    free_location(location);
    return 0;
}