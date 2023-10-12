#include <fstream>
#include <vector>
#include <iostream>

// assuming the grid size is 37*37
const int gridSize = 37;

// Read the grid layout from the .txt file
std::vector<int> readGrid(const std::string& filename) {
    std::ifstream myfile;
    myfile.open(filename);
    char mychar;
    int myint;

    std::vector<int> tiles(gridSize * gridSize);

    // Open grid layout file
    if (myfile.is_open()) {
        int tileCounter = 0;
        while (myfile.good())
        {
            myfile >> std::noskipws >> mychar; // pipe file's content into stream

            if (!isspace(mychar))	// Line-break counts as space
            {
                myint = int(mychar) - 48;	// 48 is keycode for 0, 49 keycode for 1, etc.
                tiles[tileCounter] = myint;
                tileCounter += 1;
                // I break here, because if not it will try to add the last character of the .txt file doubled.
                if (tileCounter == gridSize * gridSize)
                    break;
            }
        }
    }
    else
    {
        std::cout << "Failed to read the .txt file." << std::endl;
        std::cout << "Make sure the path is correct, or just try again." << std::endl;
    }
    myfile.close();

    return tiles;
}

// Write the Visibility precomputation in another .txt file
//void writeVisibility(const std::vector<std::vector<bool>>& visibility, const std::string& filename) {
void writeVisibility(const std::vector<std::vector<bool>>& grid, const std::string& filename) {
    // Check if the file exists
    std::ifstream fileExists(filename);
    if (fileExists) {
        // Delete the existing file
        if (remove(filename.c_str()) != 0) {
            std::cerr << "Error deleting file: " << filename << std::endl;
        }
    }

    std::ofstream myfile(filename);

    for (const auto& row : grid) {
        for (bool tile : row) {
            myfile << (tile ? 1 : 0) << ' ';
        }
        myfile << '\n';
    }

    myfile.close();
}


// Compute Visibility - Simplified Bresenham line algorithm
std::vector<std::vector<bool>> computeVisibilityBresenhamSimplified(const std::vector<std::vector<int>>& grid) {
    std::vector<std::vector<bool>> visibility(gridSize * gridSize, std::vector<bool>(gridSize * gridSize, false));

    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            for (int m = 0; m < gridSize; m++) {
                for (int n = 0; n < gridSize; n++) {
                    // Check if the cell (m,n) is visible from cell (i,j)
                    if (i == m && j == n) {
                        visibility[i * gridSize + j][m * gridSize + n] = true;
                        continue;
                    }

                    // Compute the line from (i,j) to (m,n)
                    int dx = abs(m - i);
                    int dy = abs(n - j);
                    int x = i, y = j;
                    int n1 = 1 + dx + dy;
                    int x_inc = (m > i) ? 1 : -1;
                    int y_inc = (n > j) ? 1 : -1;
                    int error = dx - dy;
                    dx *= 2;
                    dy *= 2;

                    // "Walk" along the line and check for walls
                    bool visible = true;            // Assume the cell (m,n) is initially visible
                    bool wallEncountered = false;   // Set to true when a wall is encountered

                    for (; n1 > 0; --n1) {
                        if (grid[x][y] == 2) {
                            wallEncountered = true;
                            break;
                        }

                        if (error > 0) {
                            x += x_inc;
                            error -= dy;
                        } else {
                            y += y_inc;
                            error += dx;
                        }
                    }

                    // If no wall was encountered on the line between the two cells, or if a wall was the final cell
                    if (!wallEncountered || (x == m && y == n)) {
                        visibility[i * gridSize + j][m * gridSize + n] = visible;
                    }

                }
            }
        }
    }

    return visibility;
}

std::vector<std::vector<bool>> computeVisibilityBresenhamSupercover(const std::vector<std::vector<int>>& grid) {
    std::vector<std::vector<bool>> visibility(gridSize * gridSize, std::vector<bool>(gridSize * gridSize, false));

    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            for (int m = 0; m < gridSize; m++) {
                for (int n = 0; n < gridSize; n++) {
                    if (i == m && j == n) {
                        visibility[i * gridSize + j][m * gridSize + n] = true;
                        continue;
                    }

                    int dx = abs(m - i);
                    int dy = abs(n - j);
                    int x = i, y = j;
                    int x_inc = (m > i) ? 1 : -1;
                    int y_inc = (n > j) ? 1 : -1;
                    int error = dx - dy;
                    dx *= 2;
                    dy *= 2;

                    // "Walk" along the line and check for walls
                    bool visible = true;  // Assume the cell (m,n) is initially visible
                    bool wallEncountered = false;

                    for (; x != m || y != n;) {

                        // Boundary checks
                        if (x < 0 || x >= gridSize || y < 0 || y >= gridSize) {
                            // Line went out of bounds, stop drawing
                            break;
                        }
                        if (grid[x][y] == 2) {
                            wallEncountered = true;
                            break;
                        }

                        if (grid[x][y] == 2) {
                            wallEncountered = true;
                            break;
                        }

                        if (error > 0) {
                            x += x_inc;
                            error -= dy;
                        }
                        else if (error < 0) {
                            y += y_inc;
                            error += dx;
                        }
                        else { // error == 0
                            if(grid[x + x_inc][y] != 2 && grid[x][y + y_inc] != 2) {
                                x += x_inc;
                                y += y_inc;
                                error += dx - dy;
                            }
                            else {
                                wallEncountered = true;
                                break;
                            }
                        }
                    }

                    // If no wall was encountered on the line between the two cells, or if a wall was the final cell
                    if (!wallEncountered || (x == m && y == n)) {
                        visibility[i * gridSize + j][m * gridSize + n] = visible;
                    }
                }
            }
        }
    }

    return visibility;
}


// Main function
int main() {
    std::vector<int> tiles = readGrid("../../extras/grid/grid_layout.txt");

    // Convert 1D tiles to 2D grid
    std::vector<std::vector<int>> grid(gridSize, std::vector<int>(gridSize));
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            grid[i][j] = tiles[i * gridSize + j];
        }
    }

    // Compute visibility matrices
    std::vector<std::vector<bool>> visibilitySimplified = computeVisibilityBresenhamSimplified(grid);
    writeVisibility(visibilitySimplified, "../../extras/grid/visibilitySimplified.txt");

    std::vector<std::vector<bool>> visibilitySupercover = computeVisibilityBresenhamSupercover(grid);
    writeVisibility(visibilitySupercover, "../../extras/grid/visibilitySupercover.txt");

    return 0;
}


