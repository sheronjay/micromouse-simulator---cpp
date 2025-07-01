#include <iostream>
#include <string>
#include <climits>
#include <queue>
#include <utility>
#include "API.h"

using namespace std;

// Your existing maze representation
const int maze_size = 16;
int flood_values[maze_size][maze_size];
bool walls[maze_size][maze_size][4]; // [row][column][N,E,S,W]

// State of micromouse
int position[2]; // [row, column]
int direction; // 0=N, 1=E, 2=S, 3=W

int direction_deltas[4][2] = {{-1,0},{0,1},{1,0},{0,-1}}; // N, E, S, W
int goal_cells[4][2] = {{7,7},{7,8},{8,7},{8,8}};

// Your existing functions (copy them here)
void initialize(){
    position[0] = 15;
    position[1] = 0;
    direction = 0;

    for(int i = 0; i < maze_size; i++){
        for(int j = 0; j < maze_size; j++){
            flood_values[i][j] = INT_MAX;
            for(int d = 0; d < 4; d++){
                walls[i][j][d] = false;
            }
        }
    }

    // Set boundary walls
    for(int i = 0; i < maze_size; i++){
        walls[0][i][0] = true;
        API::setWall(0, 15 - i, 'n');
        walls[15][i][2] = true;
        API::setWall(15, 15 - i, 's');
        walls[i][0][3] = true;
        API::setWall(i, 15, 'w');
        walls[i][15][1] = true;
        API::setWall(i, 0, 'e');
    }
}

void calculate_flood_values() {
    // Reset all flood values first
    for(int i = 0; i < maze_size; i++){
        for(int j = 0; j < maze_size; j++){
            flood_values[i][j] = INT_MAX;
        }
    }

    // Set initial flood values for goal cells
    for(int i = 0; i < 4; i++){
        flood_values[goal_cells[i][0]][goal_cells[i][1]] = 0;
    }

    // Flood fill algorithm
    queue<pair<int, int>> q;
    for(int i = 0; i < 4; i++){
        q.push(make_pair(goal_cells[i][0], goal_cells[i][1]));
    }

    while(!q.empty()){
        pair<int, int> current = q.front();
        q.pop();
        int row = current.first;
        int col = current.second;

        for(int d = 0; d < 4; d++){
            int new_row = row + direction_deltas[d][0];
            int new_col = col + direction_deltas[d][1];

            if(new_row >= 0 && new_row < maze_size && new_col >= 0 && new_col < maze_size &&
               !walls[row][col][d]) {
                if(flood_values[new_row][new_col] > flood_values[row][col] + 1) {
                    flood_values[new_row][new_col] = flood_values[row][col] + 1;
                    q.push(make_pair(new_row, new_col));
                }
            }
        }
    }
}

void update_walls(bool front_sensor, bool right_sensor, bool left_sensor){
    // Front wall
    walls[position[0]][position[1]][direction] = front_sensor;
    if(front_sensor){
        int front_row = position[0] + direction_deltas[direction][0];
        int front_col = position[1] + direction_deltas[direction][1];
        if(front_row >= 0 && front_row < maze_size && front_col >= 0 && front_col < maze_size){
            walls[front_row][front_col][(direction + 2) % 4] = true;
        }
        // Update simulator display
        char dir_chars[] = {'n', 'e', 's', 'w'};
        API::setWall(position[1], 15 - position[0], dir_chars[direction]);
    }
    
    // Left wall
    int left_direction = (direction + 3) % 4;
    walls[position[0]][position[1]][left_direction] = left_sensor;
    if(left_sensor){
        int left_row = position[0] + direction_deltas[left_direction][0];
        int left_col = position[1] + direction_deltas[left_direction][1];
        if(left_row >= 0 && left_row < maze_size && left_col >= 0 && left_col < maze_size){
            walls[left_row][left_col][(left_direction + 2) % 4] = true;
        }
        char dir_chars[] = {'n', 'e', 's', 'w'};
        API::setWall(position[1], 15 - position[0], dir_chars[left_direction]);
    }
    
    // Right wall
    int right_direction = (direction + 1) % 4;
    walls[position[0]][position[1]][right_direction] = right_sensor;
    if(right_sensor){
        int right_row = position[0] + direction_deltas[right_direction][0];
        int right_col = position[1] + direction_deltas[right_direction][1];
        if(right_row >= 0 && right_row < maze_size && right_col >= 0 && right_col < maze_size){
            walls[right_row][right_col][(right_direction + 2) % 4] = true;
        }
        char dir_chars[] = {'n', 'e', 's', 'w'};
        API::setWall(position[1], 15 - position[0], dir_chars[right_direction]);
    }
}

int get_next_move(){
    int best_direction = -1;
    int lowest_value = INT_MAX;

    for(int d = 0; d < 4; d++){
        int next_row = position[0] + direction_deltas[d][0];
        int next_col = position[1] + direction_deltas[d][1];

        if(next_row >= 0 && next_row < maze_size && next_col >= 0 && next_col < maze_size && 
           !walls[position[0]][position[1]][d] && flood_values[next_row][next_col] < lowest_value){
            lowest_value = flood_values[next_row][next_col];
            best_direction = d;
        }
    }
    return best_direction;
}

void turn_to_direction(int target_direction){
    int turns = (target_direction - direction + 4) % 4;

    switch(turns){
        case 0:
            break;
        case 1:
            API::turnRight();
            break;
        case 2:
            API::turnRight();
            API::turnRight();
            break;
        case 3:
            API::turnLeft();
            break;
    }
    direction = target_direction;
}

void move_forward(){
    position[0] += direction_deltas[direction][0];
    position[1] += direction_deltas[direction][1];
    API::moveForward();
}

bool is_at_goal(){
    for (int i = 0; i < 4; i++) {
        if (position[0] == goal_cells[i][0] && position[1] == goal_cells[i][1]) {
            return true;
        }
    }
    return false;
}

void log(string text) {
    cerr << text << endl;
}

int main(int argc, char* argv[]) {
    log("Running floodfill algorithm...");
    
    initialize();
    
    while (!is_at_goal()) {
        // Read sensor values from simulator
        bool front_wall = API::wallFront();
        bool left_wall = API::wallLeft();
        bool right_wall = API::wallRight();
        
        // Update walls and recalculate flood values
        update_walls(front_wall, right_wall, left_wall);
        calculate_flood_values();
        
        // Display current flood value
        log("Current position: (" + to_string(position[0]) + ", " + to_string(position[1]) + ")");
        log("Flood value: " + to_string(flood_values[position[0]][position[1]]));
        
        // Get next move and execute
        int next_direction = get_next_move();
        if (next_direction != -1) {
            turn_to_direction(next_direction);
            move_forward();
        } else {
            log("No valid move found!");
            break;
        }
    }
    
    if (is_at_goal()) {
        log("Goal reached!");
    }
    
    return 0;
}