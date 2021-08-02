#pragma once

#include "State.h"
#include "io.h"

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

		std::set<State_t> closedSet;

        while (!openSet.empty())
        {
			// TODO: who is first? second? structure binding
            auto currentNode = openSet.front();      
            auto&& currentState = currentNode.first;
            auto&& currentMoves = currentNode.second;
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
				//TODO
                auto&& childState = childMovePair.first;
                MoveDirection move = childMovePair.second;

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
	//TODO implement Validate
	//throw exception if not valid

    template <class State_t>
    static void Validate(const State_t& state)
    {
		return;
    }
};
