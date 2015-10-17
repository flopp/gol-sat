# GoL-SAT

A SAT-based forward/backwards solver for Conway's "Game of Life".

## Compile

1. Make sure `git`, `cmake`, and `boost::program_options` are installed
2. Clone the repository: `$ git clone https://github.com/flopp/gol-sat`
3. Change into the newly created directory: `$ cd gol-sat`
2. Build: `$ make`

## Usage

Run `$ bin/gol-sat -e X pattern.txt` to perform a *backwards computation* consisting of `X` steps an finally yields the pattern specified in the file `pattern.txt`.

Run `$ bin/gol-sat -f -e X pattern.txt` to perform a *forward computation* consisting of `X` steps starting with the pattern specified in the file `pattern.txt`.

## Pattern Format
The text file format used for patterns starts with two numbers specifying the `width` and `height` of the pattern. Then `width` * `height` cell characters follow, where

- `.` or `0` is a dead cell
- `X` or `1` is an alive cell
- `?` is an unspecified cell

The `patterns` subdirectory contains some samples.

## Example
The following command performs backwards computation consisting of two evolution steps (option `-e 2`) that finally yield the game of life pattern specified in the file `patterns/smily.txt`:

```
$ bin/gol-sat -e 2 patterns/smily.txt
-- Reading pattern from file: patterns/smily.txt
-- Building formula for 2 evolution steps...
-- Setting pattern constraint on last generation...
-- Solving formula...

-- Initial generation:
...X.....XX
..X....X...
.X..X....XX
.X.XXX.XX..
..X...XX...
....X..X...
X.X....XXXX
X.XX.X.....
......X..XX
...X.X..X..

-- Evolves to:
...........
..XX....X..
.X..XXXX.X.
.X.XXX.XXX.
..X........
.X.X.....X.
..X.X.XXXX.
..XX..XX...
..XX.XX..X.
.........X.

-- Evolves to final generation (from pattern):
...........
..XXXXXXX..
.X.......X.
.X.X...X.X.
.X.......X.
.X.X...X.X.
.X..XXX..X.
.X.......X.
..XXXXXXX..
...........
```
