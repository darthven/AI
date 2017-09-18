#include "location.h"
#include <sys\stat.h>

#define CELL_VALUE_LENGTH 4

Location* init_location(int x_size, int y_size) {
    Location *location = malloc(sizeof(Location));
    location->x_size = x_size;
    location->y_size = y_size;
    location->agent_coordinates = (int * ) malloc(2 * sizeof(int));
    Cell **matrix = malloc(x_size * sizeof(Cell*));
    for(int i = 0; i < x_size; i++) {
        matrix[i] = malloc(y_size * sizeof(Cell));
        for(int j = 0; j < y_size; j++) {
            matrix[i][j].value = NULL;
        }
    }
    location->matrix = matrix;
}

void free_location(Location *location) {
    for(int i = 0; i < location->x_size; i++) {
        free(location->matrix[i]);
    }
    free(location->agent_coordinates);
    free(location->matrix);
    free(location);
}

void init_matrix(Location *location) {    
    for(int i = 0; i < location->x_size; i++) {
        for(int j = 0; j < location->y_size; j++) {
            if(i == 0 || i == location->x_size - 1 ||
                j == 0 || j == location->y_size - 1) {
                location->matrix[i][j].tag = WALL;
                location->matrix[i][j].value = CELL_VALUE[WALL];
            } else {
                location->matrix[i][j].tag = ROOM;
                location->matrix[i][j].value = CELL_VALUE[ROOM];          
            }  
        }
    }
}

Cell* first_empty_cell(Location *location) {
    for(int i = 0; i < location->x_size; i++) {
        for(int j = 0; j < location->y_size; j++) {
            if(location->matrix[i][j].value == NULL) {                       
                return &(location->matrix[i][j]);        
            }
        }       
    }
    return NULL;
}

void randomize_matrix(Location *location, int walls_count) {
    srand(time(NULL));
    for(int i = 0; i < walls_count; i++) {
        int row_index = (int) rand() % location->x_size;
        int col_index = (int) rand() % location->y_size;
        location->matrix[row_index][col_index].tag = WALL;
        location->matrix[row_index][col_index].value = CELL_VALUE[WALL];
    }  
}

void file_write_matrix(Location *location, char *file_path,
     int walls_count, int rubbish_count, int players_count) {
    FILE *file = fopen(file_path, "w");
    if(file != NULL) {
        init_matrix(location);
        randomize_matrix(location, walls_count);
        randomize_rubbish(location, rubbish_count);
        init_agent(location, players_count);
        for(int i = 0; i < location->x_size; i++) {
            for(int j = 0; j < location->y_size; j++) {                 
                fputs((char *) location->matrix[i][j].value, file);                                
            }   
            fputs("\n", file);               
        }
    }   
    fclose(file);
}

void file_read_matrix(Location *location, char *file_path) {
    FILE *file = fopen(file_path, "r");    
    if(file != NULL) {    
        char *buffer = (char *) malloc(CELL_VALUE_LENGTH);  
        Cell *cell;        
        while(!feof(file)) {
            fgets(buffer, CELL_VALUE_LENGTH, file);   
            cell = first_empty_cell(location);     
            if(cell != NULL && strcmp(buffer, "\n") != 0) {
                if(strcmp(buffer, CELL_VALUE[WALL]) == 0) {                           
                    cell->tag = WALL;
                    cell->value = CELL_VALUE[WALL];               
                } else if(strcmp(buffer, CELL_VALUE[RUBBISH]) == 0) {                  
                    cell->tag = RUBBISH;
                    cell->value = CELL_VALUE[RUBBISH]; 
                } else if(strcmp(buffer, CELL_VALUE[AGENT]) == 0) {                  
                    cell->tag = AGENT;
                    cell->value = CELL_VALUE[AGENT];        
                } else {                  
                    cell->tag = ROOM;
                    cell->value = CELL_VALUE[ROOM]; 
                }
            }                                               
        }          
    }
    fclose(file);   
}


void define_agent_coordinates(Location *location) {   
    for(int i = 0; i < location->x_size; i++) {       
        for(int j = 0; j < location->y_size; j++) {
            if(strcmp(location->matrix[i][j].value, CELL_VALUE[AGENT]) == 0) {
               location->agent_coordinates[0] = i;
               location->agent_coordinates[1] = j;
               return;           
            }       
        }      
    }     
}

void randomize_rubbish(Location *location, int rubbish_count) {
    srand(time(NULL));
    for(int i = 0; i < rubbish_count; i++) {
        int row_index = (int) rand() % (location->x_size - 1);
        int col_index = (int) rand() % (location->y_size - 1);
        if(location->matrix[row_index][col_index].tag != WALL) {
            location->matrix[row_index][col_index].tag = RUBBISH;
            location->matrix[row_index][col_index].value = CELL_VALUE[RUBBISH];
        }        
    }  
}

void init_agent(Location *location, int players_count) {
    srand(time(NULL));
    for(int i = 0; i < players_count; i++) {
        int row_index = (int) rand() % (location->x_size - 1);
        int col_index = (int) rand() % (location->y_size - 1);
        if(location->matrix[row_index][col_index].tag == ROOM) {
            location->agent_coordinates[0] = row_index;
            location->agent_coordinates[1] = col_index;
            location->matrix[row_index][col_index].tag = AGENT;
            location->matrix[row_index][col_index].value = CELL_VALUE[AGENT];
        } else {
            players_count++;
        }       
    }  
}

void display_matrix(Location *location) {
    for(int i = 0; i < location->x_size; i++) {
        for(int j = 0; j < location->y_size; j++) {
            printf("%s", location->matrix[i][j].value);
        }
        printf("\n");
    }
}

void move_right(Location *location) {  
    int agent_x = location->agent_coordinates[0];
    int agent_y = location->agent_coordinates[1];
    if(location->matrix[agent_x][agent_y + 1].tag != WALL) {
        location->matrix[agent_x][agent_y].value = CELL_VALUE[ROOM];
        location->matrix[agent_x][agent_y + 1].value = CELL_VALUE[AGENT]; 
        location->agent_coordinates[1] = agent_y + 1; 
    } else {
        //move_left(location);
    }        
}

void move_left(Location *location) {   
    int agent_x = location->agent_coordinates[0];
    int agent_y = location->agent_coordinates[1];
    if(location->matrix[agent_x][agent_y - 1].tag != WALL) {
        location->matrix[agent_x][agent_y].value = CELL_VALUE[ROOM];
        location->matrix[agent_x][agent_y - 1].value = CELL_VALUE[AGENT];    
        location->agent_coordinates[1] = agent_y - 1;   
    } else {
        //move_right(location);
    }     
}

void move_top(Location *location) { 
    int agent_x = location->agent_coordinates[0];
    int agent_y = location->agent_coordinates[1];
    if(location->matrix[agent_x - 1][agent_y].tag != WALL) {     
        location->matrix[agent_x][agent_y].value = CELL_VALUE[ROOM];
        location->matrix[agent_x - 1][agent_y].value = CELL_VALUE[AGENT]; 
        location->agent_coordinates[0] = agent_x - 1; 
    } else {
        //move_bottom(location);
    }        
}

void move_bottom(Location *location) {    
    int agent_x = location->agent_coordinates[0];
    int agent_y = location->agent_coordinates[1];
    if(location->matrix[agent_x + 1][agent_y].tag != WALL) {     
        location->matrix[agent_x][agent_y].value = CELL_VALUE[ROOM];
        location->matrix[agent_x + 1][agent_y].value = CELL_VALUE[AGENT]; 
        location->agent_coordinates[0] = agent_x + 1;
    } else {
        //move_top(location);
    }  
}

void clean(Location *location) {  
    // move_left(location);
    // move_bottom(location); 
    move_top(location); 
    move_right(location);   
    //TODO provide logic for cleaning rooms for AI-agent
}

void start(Location *location, clock_t end_time_millis) { 
    define_agent_coordinates(location);      
    do {         
        clean(location);
        display_matrix(location);
        sleep(1);
    } while(clock() <= end_time_millis);
}