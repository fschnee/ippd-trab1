#include <valarray>
#include <cstdio>

#include "chrono.hpp"

auto is_valid_move(auto const already_placed, auto const column, auto const row) -> bool
{
    if (already_placed[column] >= 0) return false; // Mesma coluna.

    for(auto c = 0; c < column; ++c)
    {
        auto const dist = column - c;
        if     (already_placed[c] == row)        return false; // Mesma linha.
        else if(already_placed[c] == row - dist) return false; // Diagonal por baixo.
        else if(already_placed[c] == row + dist) return false; // Diagonal por cima.
    }

    return true;
}

auto backtrack(auto already_placed, auto const column, bool debug = false) -> void
{
    if(column >= already_placed.size()) // All positions filled.
    {
        if(debug)
        {
            std::printf("{ ");
            for(auto const& e : already_placed) std:printf("%i ", e);
            std::printf("}\n");
        }
        return;
    }

    auto row = already_placed.size();
    while(row--)
    {
        if(!is_valid_move(already_placed, column, row)) continue;

        already_placed[column] = row;
        backtrack(already_placed, column + 1);
        already_placed[column] = -already_placed.size();
    }
}

auto par_findall(auto n) -> void
{
    #pragma omp parallel for
    for(auto i = 0; i < n; ++i)
    {
        // Array sized n filled with -n.
        auto already_placed = std::valarray<int>(-n, n);
        already_placed[0] = i;

        backtrack(already_placed, 1);
    }
}

auto seq_findall(auto n) -> void
{
    for(auto i = 0; i < n; ++i)
    {
        // Vector sized n filled with -n.
        auto already_placed = std::valarray<int>(-n, n);
        already_placed[0] = i;

        backtrack(already_placed, 1);
    }
}

auto main() -> int
{
    auto const _n = 14;
    auto const runs = 100;

    std::printf("settings: final n = %i, averaging runs = %i\n", _n, runs);

    for(auto n = 1; n <= _n; ++n)
    {
        auto par_times = std::valarray<double>(.0, runs);
        auto seq_times = std::valarray<double>(.0, runs);
        for(auto i = 0; i < runs; ++i)
        {
            using timer    = std::chrono::high_resolution_clock;
            using ms_float = std::chrono::duration<double, std::ratio<1, 1000> >;
            par_times[i] = pinguim::chrono::time<timer, ms_float>([n]{ par_findall(n); });
            seq_times[i] = pinguim::chrono::time<timer, ms_float>([n]{ seq_findall(n); });
        }

        std::printf(
            "n = %2i: par = %14.06fms\tseq = %14.06fms\n",
            n,
            par_times.sum() / par_times.size(),
            seq_times.sum() / seq_times.size()
        );
    }

    return 0;
}