#include <Arduino.h>

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

const int8_t neiCells[4][2] = {{ -1, 0}, {0, 1}, {1, 0}, {0, -1}};
const int8_t neiWalls[4][2] = {{0, 0}, {0, 1}, {1, 0}, {0, 0}};

#define ROWS 10
#define COLUMNS 10

class Cells
{

  private:
    // vertical walls array
    inline static boolean verticalWalls[ROWS][COLUMNS + 1];

    // horizontal walls array
    inline static boolean horizontalWalls[ROWS + 1][COLUMNS];

  public:
    // value array
    inline static byte values[ROWS][COLUMNS];

    byte mouseRow;
    byte mouseColumn;
    byte mouseHeading;

    inline static byte targetRow = 7;
    inline static byte targetColumn = 7;

    // Constructor method (called when the maze is created)
    static void ells()
    {
      // initialize verticalWalls (add exterior walls)
      for (byte i = 0; i < ROWS; i++)
      {
        for (byte j = 0; j < COLUMNS + 1; j++)
        {
          if (j == 0 || j == COLUMNS)
          {
            verticalWalls[i][j] = true;
          }
        }
      }

      // initialize horizontalWalls (add exterior walls)
      for (byte i = 0; i < ROWS + 1; i++)
      {
        for (byte j = 0; j < COLUMNS; j++)
        {
          if (i == 0 || i == ROWS)
          {
            horizontalWalls[i][j] = true;
          }
        }
      }
    }

    static void solve()
    {

      for (int8_t i = 0; i < ROWS; i++)
      {
        for (int8_t j = 0; j < COLUMNS; j++)
        {
          values[i][j] = 255;
        }
      }
      values[targetRow][targetColumn] = 0;
      /* values[targetRow + 1][targetColumn] = 0;
        values[targetRow][targetColumn + 1] = 0;
        values[targetRow + 1][targetColumn + 1] = 0;


        horizontalWalls[7][7] = true;
        horizontalWalls[7][8] = true;
        horizontalWalls[9][7] = true;
        horizontalWalls[9][8] = true;

        verticalWalls[5][5] = true;
        verticalWalls[10][4] = true;
        verticalWalls[8][7] = true;
        verticalWalls[7][9] = true;
        verticalWalls[8][9] = true;

      */
      bool cont = true;
      while (cont)
      {
        cont = false;
        for (int8_t i = 0; i < ROWS; i++)
        {
          for (int8_t j = 0; j < COLUMNS; j++)
          {
            if (values[i][j] < 255)
              for (int8_t k = 0; k < 4; k++)
              {

                int8_t neiCellRow = i + neiCells[k][0];
                int8_t neiCellColomn = j + neiCells[k][1];

                int8_t neiWallRow = i + neiWalls[k][0];
                int8_t neiWallColomn = j + neiWalls[k][1];

                bool wall = false;

                if (k == 0 || k == 2)
                  wall = horizontalWalls[neiWallRow][neiWallColomn];
                else
                  wall = verticalWalls[neiWallRow][neiWallColomn];
                if (values[neiCellRow][neiCellColomn] == 255 && !wall)
                {
                  values[neiCellRow][neiCellColomn] = values[i][j] + 1;
                  cont = true;
                }
              }
          }
        }
      }
    }

    int8_t findBastawese()
    {
      byte bestValue = 255;
      int8_t bestDir = 0;

      for (int8_t k = 0; k < 4; k++)
      {
        int8_t neiCellRow = mouseRow + neiCells[k][0];
        int8_t neiCellColomn = mouseColumn + neiCells[k][1];

        int8_t neiWallRow = mouseRow + neiWalls[k][0];
        int8_t neiWallColomn = mouseColumn + neiWalls[k][1];

        bool wall = false;

        if (k == 0 || k == 2)
          wall = horizontalWalls[neiWallRow][neiWallColomn];
        else
          wall = verticalWalls[neiWallRow][neiWallColomn];
        if (values[neiCellRow][neiCellColomn] < bestValue && !wall)
        {
          bestValue = values[neiCellRow][neiCellColomn];
          bestDir = k;
        }
        else if (values[neiCellRow][neiCellColomn] == bestValue && !wall && mouseHeading == k)
          bestDir = k;
      }
      return bestDir;
    }

    void addWall(int8_t wallDirc)
    {
      switch (wallDirc)
      {
        case 0:
          horizontalWalls[mouseRow][mouseColumn] = true;
          break;
        case 1:
          verticalWalls[mouseRow][mouseColumn + 1] = true;
          break;
        case 2:
          horizontalWalls[mouseRow + 1][mouseColumn] = true;
          break;
        case 3:
          verticalWalls[mouseRow][mouseColumn] = true;
          break;
      }
    }
    /*
          Cell Print Function Version 2
          This version of the print function has been modified to print
          any size maze (the previous version could not print large
          mazes) and to work with the btMonitor Android App I wrote,
          which is available through my free online course at
          http://www.akrobotnerd.com
    */

    void print()
    {
      for (byte i = 0; i < 2 * ROWS + 1; i++)
      {
        for (byte j = 0; j < 2 * COLUMNS + 1; j++)
        {
          // Add Horizontal Walls
          if (i % 2 == 0 && j % 2 == 1)
          {
            if (horizontalWalls[i / 2][j / 2] == true)
            {
              com.print(" ---");
            }
            else
            {
              com.print("    ");
            }
          }

          // Add Vertical Walls
          if (i % 2 == 1 && j % 2 == 0)
          {
            if (verticalWalls[i / 2][j / 2] == true)
            {
              com.print("|");
            }
            else
            {
              com.print(" ");
            }
          }

          // Add Flood Fill Values
          if (i % 2 == 1 && j % 2 == 1)
          {
            if ((i - 1) / 2 == mouseRow && (j - 1) / 2 == mouseColumn)
            {
              if (mouseHeading == NORTH)
              {
                com.print(" ^ ");
              }
              else if (mouseHeading == EAST)
              {
                com.print(" > ");
              }
              else if (mouseHeading == SOUTH)
              {
                com.print(" v ");
              }
              else if (mouseHeading == WEST)
              {
                com.print(" < ");
              }
            }
            else
            {
              byte value = values[(i - 1) / 2][(j - 1) / 2];
              if (value >= 100)
              {
                com.print(value);
              }
              else
              {
                com.print(" ");
                com.print(value);
              }
              if (value < 10)
              {
                com.print(" ");
              }
            }
          }
        }
        com.print("\n");
      }
      com.print("\n");
    }
};
