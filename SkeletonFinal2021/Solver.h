#pragma once

#include "State.h"
#include "io.h"

#include <unordered_set>
#include <iostream>
#include <set>
#include <queue>

class Solver
{
public:
    template <class State_t>
    static Moves SolveBFS(const State_t& initialState)
    {
        std::cout << "Solving..." << std::endl << initialState;

        Validate(initialState);
        if (initialState.IsGoalState()) return {}; // no moves required. Initial state is goal state.

        using Node = std::pair<State_t, Moves>;

		std::queue<Node> openSet;
        openSet.push({ initialState, {} });

        //auto cmp = [](const State_t& first, const State_t& second) {return first.GetData() < second.GetData(); };

        auto cmp_eq = [](const State_t& first, const State_t& second) {return first.GetData() == second.GetData(); };

        std::unordered_set < State_t, std::function<size_t(const State_t&)>, decltype(cmp_eq) > closedSet(8u, &ComputeHash<State_t>, cmp_eq);

        while (!openSet.empty())
        {
            auto currentNode = openSet.front();   
            auto [currentState, currentMoves] = currentNode;
            openSet.pop();

            // some logging
            static size_t maxDepth = 0;
            if (currentMoves.size() > maxDepth)
            {
                maxDepth = currentMoves.size();
                std::cout << "Max Depth: " << maxDepth << std::endl;
            }
            // end logging

            if (currentState.IsGoalState())
            {
                std::cout << "Visited: " << closedSet.size() << std::endl;                
                return currentMoves; // leaving cycle statement
            }

            closedSet.insert(currentState);

            for (auto&& childMovePair : currentState.GetChildren())
            {
                auto [childState, move] = childMovePair;

				if(closedSet.find( childState) == closedSet.end())
                {
                    Moves childMoves = currentMoves;
                    childMoves.push_back(move);
                    openSet.push({ std::move(childState), std::move(childMoves) });
                }
            }
        }

        throw std::runtime_error("Couldn't solve");
    }

private:
    template <class State_t>
    static void Validate(const State_t& state)
    {
        if (!state.IsValid())
            throw std::runtime_error("State is invalid!");
        if (!state.IsSolvable())
            throw std::runtime_error("State is not solvable!");
		return;
    }

    template <class State_t>
    static size_t ComputeHash(const State_t& state)
    {
        static std::hash<State_t::ElementType> hasher;
        size_t hashValue = 0;
        for (auto&& element : state.GetData())
        {
            //hashValue += hasher(element);
            hashValue ^= hasher(element) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
        }
        return hashValue;
    }
};
