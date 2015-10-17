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

#include "field.h"
#include "formula.h"
#include "pattern.h"
#include <set>
#include <vector>
#include <cassert>

void rule(SatSolver& s, const Minisat::Lit& cell, const std::vector<Minisat::Lit>& n, const Minisat::Lit& next)
{
  (void)cell;
  assert(n.size() == 8);

#if 1
  // Under population (<=1 alive neighbor -> cell dies)
  for (std::size_t possiblyalive = 0; possiblyalive < n.size(); ++possiblyalive)
  {
    std::vector<Minisat::Lit> cond;
    for (std::size_t dead = 0; dead < n.size(); ++dead)
    {
      if (dead == possiblyalive) continue;
      cond.push_back(~n[dead]);
    }
    addImpl(s, cond, ~next);
  }
#endif

#if 1
  // status quo (=2 alive neighbours -> cell stays dead/alive)
  for (std::size_t alive1 = 0; alive1 < n.size(); ++alive1)
  {
    for (std::size_t alive2 = alive1 + 1; alive2 < n.size(); ++alive2)
    {
      std::vector<Minisat::Lit> cond;
      for (std::size_t i = 0; i < n.size(); ++i)
      {
        if (i == alive1 || i == alive2)
        {
          cond.push_back(n[i]);
        }
        else
        {
          cond.push_back(~n[i]);
        }
      }

      cond.push_back(cell);
      addImpl(s, cond, next);

      cond.back() = ~cell;
      addImpl(s, cond, ~next);
    }
  }
#endif

  // Birth (= 3 alive neighbors -> cell is alive)
  for (std::size_t alive1 = 0; alive1 < n.size(); ++alive1)
  {
    for (std::size_t alive2 = alive1 + 1; alive2 < n.size(); ++alive2)
    {
      for (std::size_t alive3 = alive2 + 1; alive3 < n.size(); ++alive3)
      {
        std::vector<Minisat::Lit> cond;
        for (std::size_t i = 0; i < n.size(); ++i)
        {
          if (i == alive1 || i == alive2 || i == alive3)
          {
            cond.push_back(n[i]);
          }
          else
          {
            cond.push_back(~n[i]);
          }
        }
        addImpl(s, cond, next);
      }
    }
  }

#if 1
  // Over population (>= 4 alive neighbors -> cell dies)

  for (std::size_t alive1 = 0; alive1 < n.size(); ++alive1)
  {
    for (std::size_t alive2 = alive1 + 1; alive2 < n.size(); ++alive2)
    {
      for (std::size_t alive3 = alive2 + 1; alive3 < n.size(); ++alive3)
      {
        for (std::size_t alive4 = alive3 + 1; alive4 < n.size(); ++alive4)
        {
          std::vector<Minisat::Lit> cond;
          cond.push_back(n[alive1]);
          cond.push_back(n[alive2]);
          cond.push_back(n[alive3]);
          cond.push_back(n[alive4]);
          addImpl(s, cond, ~next);
        }
      }
    }
  }
#endif
}


void transition(SatSolver& s, const Field& current, const Field& next)
{
  assert(current.width() == next.width());
  assert(current.height() == next.height());

  for (int x = -1; x <= current.width(); ++x)
  {
    for (int y = -1; y <= current.height(); ++y)
    {
      std::vector<Minisat::Lit> neighbours;

      for (int dx = -1; dx <= +1; ++dx)
      {
        for (int dy = -1; dy <= +1; ++dy)
        {
          if (dx == 0 && dy == 0) continue;
          neighbours.push_back(current(x+dx, y+dy));
        }
      }

      rule(s, current(x, y), neighbours, next(x, y));
    }
  }
}


void equivalent(SatSolver& s, const Field& field1, const Field& field2)
{
  assert(field1.width() == field2.width());
  assert(field1.height() == field2.height());

  for (int y = 0; y < field1.height(); ++y)
  {
    for (int x = 0; x < field1.width(); ++x)
    {
      s.addClause( field1(x, y), ~field2(x, y));
      s.addClause(~field1(x, y),  field2(x, y));
    }
  }
}


void patternConstraint(SatSolver& s, const Field& field, const Pattern& pat)
{
  assert(field.width() == pat.width());
  assert(field.height() == pat.height());

  for (int x = 0; x < field.width(); ++x)
  {
    for (int y = 0; y < field.height(); ++y)
    {
      switch (pat(x, y))
      {
        case Pattern::CellState::Alive:
          s.addClause(field(x, y));
          break;
        case Pattern::CellState::Dead:
          s.addClause(~field(x, y));
          break;
        case Pattern::CellState::Unknown:
          break;
      }
    }
  }
}


void buildHA(SatSolver& s, const Minisat::Lit& input1, const Minisat::Lit& input2, Minisat::Lit& carry, Minisat::Lit& sum)
{
  carry = Minisat::mkLit(s.newVar());
  sum = Minisat::mkLit(s.newVar());

  addImpl(s, {input1, input2}, carry);
  addImpl(s, {~input1}, ~carry);
  addImpl(s, {~input2}, ~carry);

  addImpl(s, {~input1, ~input2}, ~sum);
  addImpl(s, { input1, ~input2},  sum);
  addImpl(s, {~input1,  input2},  sum);
  addImpl(s, { input1,  input2}, ~sum);
}


void buildFA(SatSolver& s, const Minisat::Lit& input1, const Minisat::Lit& input2, const Minisat::Lit& carryin, Minisat::Lit& carry, Minisat::Lit& sum)
{
  carry = Minisat::mkLit(s.newVar());
  sum = Minisat::mkLit(s.newVar());

  addImpl(s, {input1, input2}, carry);
  addImpl(s, {input1, carryin}, carry);
  addImpl(s, {input2, carryin}, carry);
  addImpl(s, {~input1, ~input2}, ~carry);
  addImpl(s, {~input1, ~carryin}, ~carry);
  addImpl(s, {~input2, ~carryin}, ~carry);

  addImpl(s, {~input1, ~input2, ~carryin}, ~sum);
  addImpl(s, { input1, ~input2, ~carryin},  sum);
  addImpl(s, {~input1,  input2, ~carryin},  sum);
  addImpl(s, {~input1, ~input2,  carryin},  sum);
  addImpl(s, { input1,  input2, ~carryin}, ~sum);
  addImpl(s, { input1, ~input2,  carryin}, ~sum);
  addImpl(s, {~input1,  input2,  carryin}, ~sum);
  addImpl(s, { input1,  input2,  carryin},  sum);
}


std::vector<Minisat::Lit> add(SatSolver& s, const std::vector<Minisat::Lit>& x, const std::vector<Minisat::Lit>& y)
    {
  assert(x.size() > 0);
  assert(y.size() > 0);

  std::vector<Minisat::Lit> xx = x;
  std::vector<Minisat::Lit> yy = y;
  if (xx.size() < yy.size()) std::swap(xx, yy);

  std::vector<Minisat::Lit> outputs;
  Minisat::Lit carry;
  for (auto i = 0u; i < xx.size(); ++i)
  {
    Minisat::Lit cout;
    Minisat::Lit sum;
    if (i == 0)
    {
      buildHA(s, xx[0], yy[0], cout, sum);
    }
    else if (i < yy.size())
    {
      buildFA(s, xx[i], yy[i], carry, cout, sum);
    }
    else
    {
      buildHA(s, xx[i], carry, cout, sum);
    }
    outputs.push_back(sum);
    carry = cout;

  }
  outputs.push_back(carry);

  return outputs;
    }


std::vector<Minisat::Lit> buildAdder(SatSolver& s, const std::vector<Minisat::Lit>& inputs)
    {
  if (inputs.size() <= 1)
  {
    return inputs;
  }

  std::vector<std::vector<Minisat::Lit>> partials;
  for (const auto& input: inputs) partials.push_back({input});

  while (partials.size() > 1)
  {
    std::vector<std::vector<Minisat::Lit>> newPartials;

    for (auto i = 0u; i < partials.size()/2; ++i)
    {
      newPartials.push_back(add(s, partials[2*i], partials[2*i+1]));
    }
    if ((partials.size() & 1) == 1)
    {
      newPartials.push_back(partials.back());
    }
    std::swap(partials, newPartials);
  }

  return partials[0];
    }


void addNumberContraint(SatSolver& s, int value, const std::vector<Minisat::Lit>& encodingVars)
{
  Minisat::vec<Minisat::Lit> clause;
  for (const auto& lit: encodingVars)
  {
    clause.push((value & 1) ? lit : ~lit);
    value >>= 1;
  }
  assert(value == 0);
  s.addClause(clause);
}
