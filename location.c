#include "location.h"
#include <sys\stat.h>

#define CELL_VALUE_LENGTH 4
#define DEFAULT_RUBBISH_WEIGHT 1
#define DEFAULT_AGENT_ENERGY 2000
#define RESULT_FILE_NAME "result.mtr"

Cell* first_empty_cell(Location *location);

void define_agent_coordinates(Location *location);

void init_agent_for_file(Location *location, int players_count);

int* rubbish_info(Location *location);

void randomize_rubbish_for_file(Location *location, int rubbish_count);

char* get_output(Location *location);

void display_agent_stat(Location *location);

void move_top(Location *location);

void move_bottom(Location *location);

void move_left(Location *location);

void move_right(Location *location);

void clean(Location *location);

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

void file_write_info(char *file_path, char *information) {
    FILE *file = fopen(file_path, "w");
    if(file != NULL) {      
        fputs(information, file);            
    }   
    fclose(file);
}

void file_write_matrix(Location *location, char *file_path) {
    FILE *file = fopen(file_path, "w");
    if(file != NULL) {      
        for(int i = 0; i < location->x_size; i++) {
            for(int j = 0; j < location->y_size; j++) {                 
                fputs((char *) location->matrix[i][j].value, file);                                
            }   
            fputs("\n", file);               
        }
    }   
    fclose(file);
}

void init_matrix_in_file(Location *location, char *file_path, int walls_count,
     int rubbish_count, int players_count) {
    init_matrix(location);
    randomize_matrix(location, walls_count);
    randomize_rubbish_for_file(location, rubbish_count);
    init_agent_for_file(location, players_count);
    file_write_matrix(location, file_path);
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
                    Rubbish rubbish;
                    rubbish.weight = DEFAULT_RUBBISH_WEIGHT;
                    cell->tag = RUBBISH;
                    cell->value = CELL_VALUE[RUBBISH]; 
                    cell->rubbish = rubbish;
                } else if(strcmp(buffer, CELL_VALUE[AGENT]) == 0) {      
                    Agent agent;
                    agent.energy_count = DEFAULT_AGENT_ENERGY;            
                    cell->tag = AGENT;
                    cell->value = CELL_VALUE[AGENT];        
                    cell->agent = agent;
                } else {                  
                    cell->tag = ROOM;
                    cell->value = CELL_VALUE[ROOM]; 
                }
            }                                               
        }     
        free(buffer);     
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

void randomize_rubbish_for_file(Location *location, int rubbish_count) {
    srand(time(NULL));
    for(int i = 0; i < rubbish_count; i++) {
        int row_index = (int) rand() % (location->x_size - 1);
        int col_index = (int) rand() % (location->y_size - 1);
        if(location->matrix[row_index][col_index].tag != WALL) {          
            location->matrix[row_index][col_index].value = CELL_VALUE[RUBBISH];
        }        
    }  
}

void randomize_rubbish(Location *location, int rubbish_count, int weight) {
    srand(time(NULL));
    for(int i = 0; i < rubbish_count; i++) {
        int row_index = (int) rand() % (location->x_size - 1);
        int col_index = (int) rand() % (location->y_size - 1);
        if(location->matrix[row_index][col_index].tag == ROOM) {
            Rubbish rubbish;
            rubbish.weight = weight;
            location->matrix[row_index][col_index].rubbish = rubbish;
            location->matrix[row_index][col_index].tag = RUBBISH;
            location->matrix[row_index][col_index].value = CELL_VALUE[RUBBISH];
        } else if(location->matrix[row_index][col_index].tag == RUBBISH) {
            location->matrix[row_index][col_index].rubbish.weight++;
        }                 
    }  
}

void init_agent_for_file(Location *location, int players_count) {
    srand(time(NULL));
    for(int i = 0; i < players_count; i++) {
        int row_index = (int) rand() % (location->x_size - 1);
        int col_index = (int) rand() % (location->y_size - 1);
        if(location->matrix[row_index][col_index].tag == ROOM) {               
            location->matrix[row_index][col_index].value = CELL_VALUE[AGENT];
        } else {
            players_count++;
        }       
    }  
}


void init_agent(Location *location, int players_count, int energy_count) {
    srand(time(NULL));
    for(int i = 0; i < players_count; i++) {
        int row_index = (int) rand() % (location->x_size - 1);
        int col_index = (int) rand() % (location->y_size - 1);
        if(location->matrix[row_index][col_index].tag == ROOM) {
            Agent agent;
            agent.energy_count = energy_count;
            location->agent_coordinates[0] = row_index;
            location->agent_coordinates[1] = col_index;
            location->matrix[row_index][col_index].agent = agent;
            location->matrix[row_index][col_index].tag = AGENT;            
            location->matrix[row_index][col_index].value = CELL_VALUE[AGENT];
        } else {
            players_count++;
        }       
    }  
}

int* rubbish_info(Location *location) {
    int *result = (int *) malloc(2 * sizeof(int));
    int total_weight = 0;
    int count = 0;
    for(int i = 0; i < location->x_size; i++) {       
        for(int j = 0; j < location->y_size; j++) {            
           if(location->matrix[i][j].tag == RUBBISH) {
               count++;
               total_weight += location->matrix[i][j].rubbish.weight;
           }
        }             
    }
    result[0] = count;
    result[1] = total_weight;
    return result;
}

char* get_agent_stat(Location *location) {
    int agent_x = location->agent_coordinates[0];
    int agent_y = location->agent_coordinates[1];
    char *buffer = (char *) malloc(100 * sizeof(char));
    sprintf(buffer, "Agent X: %d; Agent Y: %d", location->agent_coordinates[0],
    location->agent_coordinates[1]);  
    return buffer;
}

char* get_rubbish_stat(Location *location) {
    int *rubbish = rubbish_info(location);
    char *buffer = (char *) malloc(100 * sizeof(char));
    sprintf(buffer, "Rubbish count: %d; Rubbish total weight: %d", rubbish[0], rubbish[1]);  
    free(rubbish);
    return buffer;
}

char* get_stat(Location *location) {
    char *agent_stat = get_agent_stat(location);
    char *rubbish_stat = get_rubbish_stat(location);
    char *buffer = (char *) malloc(strlen(agent_stat) + strlen(rubbish_stat));   
    sprintf(buffer, "%s\n%s\n", agent_stat, rubbish_stat);
    free(agent_stat);
    free(rubbish_stat);
    return buffer;
}

char* get_output(Location *location) {
    char *buffer = (char *) malloc(location->x_size * location->y_size * CELL_VALUE_LENGTH);
    buffer = strcpy(buffer, "");     
    char *stat = get_stat(location);
    strcat(buffer, stat);
    free(stat); 
    for(int i = 0; i < location->x_size; i++) {       
        for(int j = 0; j < location->y_size; j++) {
            strcat(buffer, location->matrix[i][j].value);           
        }
        strcat(buffer, "\n");
    }    
    return buffer;
}


void move_right(Location *location) {  
    int agent_x = location->agent_coordinates[0];
    int agent_y = location->agent_coordinates[1];
    if(location->matrix[agent_x][agent_y + 1].tag != WALL) {
        location->matrix[agent_x][agent_y].value = CELL_VALUE[ROOM];
        Agent agent = location->matrix[agent_x][agent_y + 1].agent;
        location->matrix[agent_x][agent_y + 1].value = CELL_VALUE[AGENT]; 
        location->matrix[agent_x][agent_y + 1].agent.energy_count--;
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
    move_left(location);
    // move_bottom(location); 
    move_top(location); 
    //move_right(location);   
    //TODO provide logic for cleaning rooms for AI-agent
}

void start(Location *location, clock_t end_time_millis) { 
    srand(time(NULL));
    define_agent_coordinates(location);
    char *output;
    do {         
        clean(location);      
        int rubbish_drop_probability = (int) rand() % (location->x_size * location->y_size);
        if(rubbish_drop_probability > (location->x_size * location->y_size) / 2) {           
            randomize_rubbish(location, rand() % 3, 1);
        }       
        output = get_output(location);
        printf("%s", output);         
        sleep(1);       
        system("cls");
    } while(clock() <= end_time_millis);
    file_write_info(RESULT_FILE_NAME, output);
    free(output);     
}