#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

static const char *CELL_VALUE[] = {" - ", "[X]", "{A}", "(R)"};

typedef struct CellDefinition {
    enum { ROOM, WALL, AGENT, RUBBISH } tag;
    const char *value;
} Cell;

typedef struct LocationDefinition {
    int x_size;
    int y_size;
    Cell **matrix;    
} Location;    

Location* init_location(int x_size, int y_size);

void free_location(Location *location);

void init_matrix(Location *location);

Cell* first_empty_cell(Location *location);

void file_write_matrix(Location *location, char *file_path,
    int walls_count, int rubbish_count, int players_count);

void file_read_matrix(Location *location, char *file_path); 

void randomize_matrix(Location *location, int walls_count);

void randomize_rubbish(Location *location, int rubbish_count);

void init_agent(Location *location, int players_count);

void display_matrix(Location *location);

void start(Location *location, clock_t end_time_millis);



