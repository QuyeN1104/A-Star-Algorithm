# A* Algorithm Project

This project implements the A* algorithm for finding the optimal path in both 2D and 3D spaces. It includes functionality for generating maps with obstacles, placing drones, and visualizing the path.

## How to Run the Project

1. **Compile the Project**:
   Use a C compiler (e.g., `gcc`) to compile the project. Ensure all source files are in the same directory.
   ```bash
   gcc -o astar File_out.c Var_str.c -lm  
   
   ```

2. **Run the Executable**:
   Execute the compiled program.
   ```bash
   ./astar
   ```

3. **Follow the Prompts**:
   - Choose between 2D or 3D mode.
   - Input the map dimensions, drone size, and number of obstacles.
   - For 2D mode, you can manually input the start and end points or load them from `map.txt`.
   - For 3D mode, the program will generate projections and save them to `map.txt`.

## Folder Structure

```
A Star Algorithm/
├── File_out.c          # Main file for running the project
├── Var_str.h           # Header file for data structures (e.g., Queue)
├── Var_str.c           # Implementation of data structures
├── Alg.c               # Implementation of the A* algorithm
├── gen.c               # Map generation logic for 2D and 3D
├── description.txt     # Detailed description of the project
├── README.md           # Documentation for the project
└── map.txt             # Single file for all outputs (map, projections, results)
```

## Project Flow in `main`

The `main` function handles the flow of the project as follows:

1. **Choose Mode**:
   - The user selects between 2D or 3D mode.

2. **Input Parameters**:
   - For 2D: Input height, width, drone size, and number of obstacles.
   - For 3D: Input height, width, depth, drone size, and number of obstacles.

3. **Generate Map**:
   - A random map is generated with obstacles.
   - The map is displayed on the screen.

4. **Save Projections (3D Mode)**:
   - For 3D mode, projections (XOY, XOZ, YOZ) are saved to `map.txt`.

5. **Place Drones**:
   - The user inputs the start (A) and end (B) positions for the drone.
   - The program validates the positions and places the drones on the map.

6. **Run A* Algorithm**:
   - The A* algorithm is executed to find the optimal path from A to B.
   - The path is marked on the map.

7. **Save Results**:
   - The final map with the path and projections is saved to `map.txt`.

8. **Display Results**:
   - The map and projections are displayed on the screen.

## Example Output

### 2D Mode
```
Choose version to run (2D = 1, 3D = 2): 1
Enter map dimensions (height width): 10 10
Enter drone size (maximum 1): 1
Enter number of obstacles: 20
Map has been saved to file 'map.txt'.
Map:
....................
....................
........#...........
........#..#######..
..#.....#...........
..#..#..............
..#..#...........#..
.....#..######...#..
.....#...........#..
.....#..............
.....#.......#......
.........#...#...#..
..#......#...#...#..
...#.....#.......#..
....#....#.......#..
.....#...#.......#..
.................#..
............###.....
....................
....................
Do you want to input points A and B from console? (y/n): y  
If not, please ensure valid points are added to the file before confirming.
Enter top-left corner coordinates of drone A (x y): 0 0
Enter top-left corner coordinates of drone B (x y): 9 9
Map after placing drones A and B:
Map:
AA..................
AA..................
........#...........
........#..#######..
..#.....#...........
..#..#..............
..#..#...........#..
.....#..######...#..
.....#...........#..
.....#..............
.....#.......#......
.........#...#...#..
..#......#...#...#..
...#.....#.......#..
....#....#....BB.#..
.....#...#....BB.#..
.................#..
............###.....
....................
....................

Path found!
Map after finding the path:
Map:
AA..................
AA+.................
.+++....#...........
..+++++.#..#######..
..#+++++#...........
..#..#+++...........
..#..#+++........#..
.....#++######...#..
.....#+++........#..
.....#.++++.........
.....#..++++.#......
.........#+++#...#..
..#......#.++#...#..
...#.....#.++++..#..
....#....#..++BB.#..
.....#...#....BB.#..
.................#..
............###.....
....................
....................

Result map has been saved to file 'map.txt'.
```

### 3D Mode
```
Choose version to run (2D = 1, 3D = 2): 2
Enter map dimensions (height width depth): 10 10 10
Enter drone size (maximum 1): 1
Enter number of obstacles: 10
Projection XOY:
..........
.####.###.
.......#..
.#..#####.
.##.#.###.
.######.#.
.####.###.
.####.###.
.########.
..........

Projection XOZ:
..........
.####.###.
.##.#####.
..###.###.
.#.######.
..###.#...
..#...###.
.####..##.
..#...#.#.
..........

Projection YOZ:
..........
.#.#..###.
...##.###.
.#...#....
...######.
.#....#...
.#..####..
.##.#####.
...#.####.
..........
Projections have been saved to 'map.txt'.
Enter top-left corner coordinates of drone A (x y z): 0 0 0
Enter top-left corner coordinates of drone B (x y z): 8 8 8
Projections after placing drones A and B:
Projection XOY:
A.........
.####.###.
.......#..
.#..#####.
.##.#.###.
.######.#.
.####.###.
.####.###.
.########.
..........

Projection XOZ:
A.........
.####.###.
.##.#####.
..###.###.
.#.######.
..###.#...
..#...###.
.####..##.
..#...#.#.
..........

Projection YOZ:
A.........
.#.#..###.
...##.###.
.#...#....
...######.
.#....#...
.#..####..
.##.#####.
...#.####.
..........
Path found in 3D!
Projections after finding the path:
Projection XOY:
A.........
+####.###.
.+.....#..
.#+.#####.
.##+#.###.
.###+##.#.
.####+###.
.####.+##.
.########.
..........

Projection XOZ:
A.........
+####.###.
.+#.#####.
..###.###.
.#.+#####.
..###.#...
..#..+###.
.####.+##.
..#...#.#.
..........

Projection YOZ:
A.........
.+.#..###.
..+##.###.
.#.+.#....
...######.
.#...+#...
.#..####..
.##.#####.
...#.####.
..........
Projections have been saved to 'map.txt'.
```

## Notes

- Ensure the `map.txt` file is writable, as the program will overwrite it.
- For 3D mode, the projections are saved to `map.txt` for visualization.
- The A* algorithm uses Euclidean distance for heuristic calculations.