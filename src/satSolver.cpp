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

#include "satSolver.h"

void addClause(SatSolver& s, const std::vector<Minisat::Lit>& c)
{
    Minisat::vec<Minisat::Lit> clause;
    for (const auto& lit: c) clause.push(lit);
    s.addClause(clause);
}

void addImpl(SatSolver& s, const std::vector<Minisat::Lit>& c, const Minisat::Lit& implied)
{
    Minisat::vec<Minisat::Lit> clause;
    for (const auto& lit: c) clause.push(~lit);
    clause.push(implied);
    s.addClause(clause);
}