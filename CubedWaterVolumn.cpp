// CubedWaterVolume.cpp : Defines the entry point for the console application.
//

// Author: Lin Luo
// 07/09/2014

#include "stdafx.h"

#include <stdio.h>
#include <memory.h>


// The Cell structure for each level
struct Cell
{
    unsigned char height; // either 0 or 1
    bool visited; // we only visit the cells that have height of 0
};

// The recursive function that visits a cell, accumulate the water volume (or if leaked due to being at the border, reset the values); it also
// looks to its 4 adjacent cells (if valid) recursively.
// Note that the top level function actually attempts to visit *all* the "connected" cells (and mark them as visited, so they will not be visited again)
// From the top level, the cells are thus visited in "chunks" (as long as they are connected)
void VisitCell(Cell* one_level_cells, unsigned short a_w, unsigned short a_h, unsigned short w, unsigned short h, unsigned __int64* sum, bool* leaked)
{
    Cell& cell = one_level_cells[h * a_w + w];
    if (cell.height == 0 && !cell.visited)
    {
        cell.visited = true;
        if (w == 0 || w + 1 == a_w || h == 0 || h + 1 == a_h)
        {
            // I am at the border while I am not guarding the water, the water for this "chunk" is then leaked!
            *leaked = true;
            *sum = 0;
        }

        if (!*leaked)
        {
            // potentially increment the volume, until it's detected leaked at some point
            ++*sum;
        }

        if (w < a_w - 1)    VisitCell(one_level_cells, a_w, a_h, w+1, h, sum, leaked);
        if (w > 0)          VisitCell(one_level_cells, a_w, a_h, w-1, h, sum, leaked);
        if (h < a_h - 1)    VisitCell(one_level_cells, a_w, a_h, w, h+1, sum, leaked);
        if (h > 0)          VisitCell(one_level_cells, a_w, a_h, w, h-1, sum, leaked);
    }
}

//@param int const * const unsigned short *a_board - argument representing the NxM board.
//@param unsigned short a_w - argument representing the width of the board
//@param unsigned short a_h - argument representing the height of the board
//@return - unsigned __int64 - the volume of water the board retains

// complexity: O(a_w * a_h * max_height)
unsigned __int64 CalculateVolume(const unsigned short *a_board, unsigned short a_w, unsigned short a_h)
{
    if (!a_board || a_w < 3 || a_h < 3)
    {
        return 0;
    }
    // Basic algorithm: slice the board horizontally into as many levels as the maximum height of the board
    // for each sliced level, determine the water volume cubed so far, and the total volume is the sum of the volume of the individual level
    unsigned __int32 n = a_w * a_h;
    unsigned short max_height = 0;
    for (unsigned __int32 i = 0; i < n; ++i)
    {
        if (max_height < a_board[i])
        {
            max_height = a_board[i];
        }
    }
    unsigned short *board = new unsigned short[n];
    memcpy(board, a_board, n * sizeof(unsigned short));
    Cell* one_level_cells = new Cell[n];
    unsigned __int64 total_volume = 0;
    for (unsigned short i = 0; i < max_height; ++i)
    {
        // form a new current level of cells (and update the copy of the board accordingly)
        unsigned __int64 volume_this_level = 0;
        for (unsigned __int32 j = 0; j < n; ++j)
        {
            if (board[j] > 0)
            {
                --board[j];
                one_level_cells[j].height = 1;
            }
            else
            {
                one_level_cells[j].height = 0;
            }
            one_level_cells[j].visited = false;
        }

        // visit all the cells within the current level
        // we mark the cells after being visited, and the cells are visited in "chunks" when they are "connected" together
        // so effectively, most of the top level cell visiting would return immediately, rather than trying to revisit the cells again and again
        for (unsigned short h = 0; h < a_h; ++h)
        {
            for (unsigned short w = 0; w < a_w; ++w)
            {
                unsigned __int64 sum = 0;
                bool leaked = false;
                // NB: the top level function here will attempt to cover *all* the connected cells at the current level (in the recursion)
                // so even though we are still iterating through all the cells at the top level, most of them should find that the cell has been visited
                // so the sum here is actually a "chunked" sum in the perception of the top level cells
                VisitCell(one_level_cells, a_w, a_h, w, h, &sum, &leaked);
                volume_this_level += sum;
            }
        }

        total_volume += volume_this_level;
    }

    delete[] one_level_cells;
    delete[] board;

    return total_volume;
}

int main()
{
#if 0
    unsigned short board[] = {
        4,3,3,
        4,2,3,
        4,1,4,
        4,4,4
    };
#endif

    // feel free to play with this board
    unsigned short board[] = {
        2, 2, 2, 2, 2, 2, 2, 2,
        2, 1, 1, 1, 1, 1, 1, 2,
        2, 1, 2, 3, 3, 2, 1, 2,
        2, 1, 3, 1, 1, 3, 1, 2,
        2, 1, 3, 1, 1, 3, 1, 2,
        2, 1, 2, 3, 3, 2, 1, 2,
        2, 1, 1, 1, 1, 1, 1, 2,
        2, 2, 2, 2, 2, 2, 2, 2,
    };
    printf("Volume: %lld\n", CalculateVolume(board, 8, 8));
	return 0;
}

