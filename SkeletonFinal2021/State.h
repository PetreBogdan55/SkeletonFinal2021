#pragma once

#include "types.h"
#include <array>
#include <optional>
#include <map>
#include <numeric>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <ostream>

template<size_t N = 3>
class State
{
public: // Types
    using Position2D = std::pair<size_t, size_t>;
    using ElementType = uint8_t;
	using Data = std::array<ElementType, N*N>;

private: // members
    Data m_data;

public:
    State() = delete;
    State(Data data) : m_data { std::move(data) } {}; // initialization_list

    friend bool operator<(const State& first, const State& second)
    {
        return first.m_data < second.m_data;
    }

    const Data& GetData() const
    {
        return m_data;
    }

    static const State GoalState()
    {
        // TODO: Refactor with STL        
        Data goalData;         
        for (auto idx = 0u; idx < goalData.size(); ++idx)
        {
            goalData[idx] = idx+1;
        }
        goalData.back() = 0;        
        return State(goalData);
    }

    bool IsGoalState() const
    {
        return m_data == GoalState().m_data;
    }

    bool IsValid() const
    {
        // TODO refactor (the 3 fors) with STL
		// Is Valid if: no duplicates, pieces 1 - 8 and 0 is the empty space
        
		Data sortedData;
        
        for (auto idx = 0u; idx < m_data.size(); ++idx)
        {
            sortedData[idx] = m_data[idx];
        }

        for (auto idxI = 0u; idxI < sortedData.size() - 1 ; ++idxI)
        {
			for (auto idxJ = 0u; idxJ < sortedData.size() - idxI - 1 ; ++idxJ)
			{
				if (sortedData[idxJ] >  sortedData[idxJ + 1])
				{
					auto temp = sortedData[idxJ];
					sortedData[idxJ] = sortedData[idxJ + 1];
					sortedData[idxJ + 1] = temp;
				}
			}
        } 

        Data validSortedData;
        
        for (auto idx = 0u; idx < validSortedData.size(); ++idx)
        {
            validSortedData[idx] = idx;
        }

        return sortedData == validSortedData;
    }

    bool IsSolvable() const
    {
		// TODO: is this a good lambda?
        auto countInversions = [](auto begin, auto end)
        {
            size_t acc{ 0u };
            for (auto it = begin; it != end; ++it)
            {
                auto&& current = *it;
                if (current != 0)
                    acc += std::count_if(it, end, [current](auto next) { return next != 0 && next < current; });
            }

            return acc;
        };

        const auto inversionsCount = countInversions(m_data.begin(), m_data.end());
        const auto isInversionCountEven = inversionsCount % 2 == 0;
        const bool isNOdd = N % 2 == 1;
        const bool isBlankRowEven = GetBlankPosition2D().first % 2 == 0;

        return (isNOdd) ? isInversionCountEven :
                          (isBlankRowEven)     ? !isInversionCountEven :
                                                 isInversionCountEven;
    }

    std::vector<std::pair<State, MoveDirection>> GetChildren() const
    {
        /*auto child1 = MoveLeft();
        auto child2 = MoveRight();
        auto child3 = MoveUp();
        auto child4 = MoveDown();

        std::vector<std::pair<State, MoveDirection>> children;
        if (child1) children.emplace_back(*child1, MoveDirection::LEFT);
        if (child2) children.emplace_back(*child2, MoveDirection::RIGHT);
        if (child3) children.emplace_back(*child3, MoveDirection::UP);
        if (child4) children.emplace_back(*child4, MoveDirection::DOWN);*/

        //TODO: See Move method and refactor the Move method
        std::vector<std::pair<State, MoveDirection>> children;
        Moves allmoves = {MoveDirection::DOWN, MoveDirection::LEFT, MoveDirection::RIGHT, MoveDirection::UP };

        for (auto direction : allmoves)
        {
            std::optional<State> child = Move(direction);
            if (child.has_value())
            {
                children.emplace_back(*child, direction);
            }
        }
        return children;
    }

private: // methods

    size_t GetBlankPosition() const
    {
        // TODO refactor using STL algo
        for (auto idx = 0u; idx < m_data.size(); ++idx)
        {
            if (m_data[idx] == 0)
                return idx;
        }
        throw std::runtime_error("Unexpected");
    }

    Position2D GetBlankPosition2D() const
    {
        auto&& absolute = GetBlankPosition(); 
        // TODO : modern C++
		return std::make_pair( absolute / N, absolute % N ); 
    }

    // TODO: Perform the move if possible and return the state. Returns std::nullopt otherwise.
    std::optional<State> Move(MoveDirection direction) const
    {
        switch (direction)
        {
        case MoveDirection::LEFT:   return MoveLeft();
        case MoveDirection::UP:     return MoveUp();
        case MoveDirection::RIGHT:  return MoveRight();
        case MoveDirection::DOWN:   return MoveDown();
        default:                    throw std::runtime_error("Not implemented.");
        }
    }

    static State SwapTiles(const State& state, size_t firstPos, size_t secondPos)
    {
        auto childData = state.GetData();
        std::swap(childData[firstPos], childData[secondPos]);
        return { std::move(childData) };
    }

    std::optional<State> MoveRight() const
    {
        if (GetBlankPosition2D().second == 0) return std::nullopt;

        auto blankPosition = GetBlankPosition();
        return SwapTiles(*this, blankPosition, blankPosition - 1);
    }

    std::optional<State> MoveLeft() const
    {
        if (GetBlankPosition2D().second == N - 1) return std::nullopt;

        auto blankPosition = GetBlankPosition();
        return SwapTiles(*this, blankPosition, blankPosition + 1);
    }

    std::optional<State> MoveDown() const
    {
        if (GetBlankPosition2D().first == 0) return std::nullopt;

        auto blankPosition = GetBlankPosition();
        return SwapTiles(*this, blankPosition, blankPosition - N);
    }

    std::optional<State> MoveUp() const
    {
        if (GetBlankPosition2D().first == N - 1) return std::nullopt;

        auto blankPosition = GetBlankPosition();
        return SwapTiles(*this, blankPosition, blankPosition + N);
    }

};

std::ostream& operator<< (std::ostream& os, MoveDirection dir)
{
    static const std::map<MoveDirection, const char*> namesMap
    {
        { MoveDirection::LEFT, "l" },
        { MoveDirection::RIGHT, "r" },
        { MoveDirection::UP, "u" },
        { MoveDirection::DOWN, "d" },
    };

    os << namesMap.at(dir); // TODO: why not [ ] operator  ?
    return os;
}

template <size_t N>
std::ostream& operator<< (std::ostream& os, const State<N>& state)
{
    os << std::endl;
    for (size_t index = 0; index < state.GetData().size(); ++index)
    {
        os << static_cast<unsigned int>(state.GetData()[index]) << ' ';

        if (index % N == N - 1) os << std::endl;
    }
    os << std::endl;

    return os;
}

using State3X3 = State<3>;
using State4X4 = State<4>;
