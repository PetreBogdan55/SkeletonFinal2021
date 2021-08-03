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
    Position2D m_position;

public:
    State() = delete;
    State(Data data) : m_data { std::move(data) } {}; // initialization_list

    Position2D GetPosition(const size_t index) const
    {
        return { index / N, index % N };
    }

    const Data& GetData() const
    {
        return m_data;
    }

    const Position2D& GetPosition() const
    {
        return m_position;
    }

    static const State GoalState()
    {     
        Data goalData;   

        //ElementType n = 1;
        //std::generate(goalData.begin(), goalData.end(), [&n]() { return n++; });
        std::iota(goalData.begin(), goalData.end(), 1);

        goalData.back() = 0;        
        return State(goalData);
    }

    bool IsGoalState() const
    {
        return m_data == GoalState().m_data;
    }

    bool IsValid() const
    {
        //Data sortedData;
        //std::copy(m_data.begin(), m_data.end(), sortedData.begin());
        //std::sort(sortedData.begin(), sortedData.end());

        //Data validSortedData;      
        //std::iota(validSortedData.begin(), validSortedData.end(), 0);

        //return std::is_permutation(sortedData.begin(), sortedData.end(), GoalState().m_data.begin());
        return std::is_permutation(m_data.begin(), m_data.end(), GoalState().m_data.begin());
    }

    bool IsSolvable() const
    {
		const auto inversionsCount = CountInversions(m_data.cbegin(), m_data.cend());
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
    size_t CountInversions(typename Data::const_iterator begin, typename Data::const_iterator end) const
    {
        size_t acc{ 0u };
        for (auto it = begin; it != end; ++it)
        {
            auto&& current = *it;
            if (current != 0)
                acc += std::count_if(it, end, [current](auto next) { return next != 0 && next < current; });
        }

        return acc;
    }

    size_t GetBlankPosition() const
    {
        auto it = std::find(m_data.begin(), m_data.end(), 0);
        if (it == m_data.end())
            throw std::runtime_error("Unexpected");
        return std::distance(m_data.begin(), it);
    }

    Position2D GetBlankPosition2D() const
    {
        auto&& absolute = GetBlankPosition(); 
        return { absolute / N, absolute % N };
    }

    std::optional<State> Move(MoveDirection direction) const
    {
        static const std::map <MoveDirection, std::function<
            std::optional<State>(const State&)>> movesMap
        {
            { MoveDirection::LEFT,  std::mem_fn(&State::MoveLeft) },
            { MoveDirection::RIGHT, std::mem_fn(&State::MoveRight) },
            { MoveDirection::UP,    std::mem_fn(&State::MoveUp) },
            { MoveDirection::DOWN,  std::mem_fn(&State::MoveDown) },
        };
        auto function = movesMap.at(direction);
        return function(*this);
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

    os << namesMap.at(dir);
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
