#ifndef __HEATMAP_H
#define __HEATMAP_H

const int CELL = 10; // Cell size

class HeatMap
{
public:
    HeatMap(int width, int height)
    {
        max = 1;
        w = width;
        h = height;
        fr = 0;

        // Calculate matrix dimensions
        c_w = w / CELL;
        c_h = h / CELL;

        m = new int *[c_w]; // Initialize matrix
        for (int i = 0; i < c_w; i++)
        {
            m[i] = new int[c_h];
            for (int j = 0; j < c_h; j++)
            {
                m[i][j] = 0;
            }
        }
    }

    // Delete pointers
    ~HeatMap()
    {
        for (int i = 0; i < c_h; i++)
        {
            delete[] m[i];
        }
        delete[] m;
    }

    int **getMatr()
    {
        return m;
    }

    int getTotal()
    {
        return max;
    }

    int getFrames()
    {
        return fr;
    }

    /**
     * Increment cell by the position, then save the max value
     *
     * @param {pos_x} X position
     * @param {pos_y} Y position
     *
    **/
    void addCell(int pos_x, int pos_y)
    {
        m[pos_x / CELL][pos_y / CELL]++;
        if (m[pos_x / CELL][pos_y / CELL] > max)
        {
            max = m[pos_x / CELL][pos_y / CELL];
        }
    }

    void countFrame()
    {
        fr = (fr + 1) % 10000;
    }

    void printMatrix()
    {
        std::cout << "-------------------------------------" << std::endl;
        for (int i = 0; i < c_w; i++)
        {
            for (int j = 0; j < c_h; j++)
            {
                std::cout << m[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }

private:
    int **m;        // Content matrix
    int w, h;       // Real size
    int c_w, c_h;   // Adaptative size
    int max;        // Max value
    int fr;         // Frame
};

#endif // __HEATMAP_H
