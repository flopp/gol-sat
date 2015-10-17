/*******************************************************************************
 * gol-sat
 *
 * Copyright (c) 2015 Florian Pigorsch
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *******************************************************************************/

#include "pattern.h"
#include <cassert>
#include <stdexcept>

void Pattern::load(std::istream& is)
{
    m_width = 0;
    m_height = 0;
    m_cells.clear();

    is >> m_width >> m_height;
    if (m_width <= 0 || m_height <= 0)
    {
        throw std::runtime_error("Pattern parsing failed when reading WIDTH and HEIGHT.");
    }

    int size = 0;
    char c;
    while (is.get(c))
    {
        if (c == '.' || c == '0')
        {
            if (size < m_width * m_height)
            {
                ++size;
                m_cells.push_back(CellState::Dead);
            }
            else
            {
                throw std::runtime_error("Pattern parsing failed when parsing cells (too many characters).");
            }
        }
        else if (c == 'X' || c == '1')
        {
            if (size < m_width * m_height)
            {
                ++size;
                m_cells.push_back(CellState::Alive);
            }
            else
            {
                throw std::runtime_error("Pattern parsing failed when parsing cells (too many characters).");
            }
        }
        else if (c == '?')
        {
            if (size < m_width * m_height)
            {
                ++size;
                m_cells.push_back(CellState::Unknown);
            }
            else
            {
                throw std::runtime_error("Pattern parsing failed when parsing cells (too many characters).");
            }
        }
    }

    if (size != m_width * m_height)
    {
        throw std::runtime_error("Pattern parsing failed when parsing cell (not enough characters).");
    }
}


const Pattern::CellState& Pattern::operator()(int x, int y) const
{
    assert(x >= 0 && x < width() && y >= 0 && y < height());

    return m_cells[x + width() * y];
}
