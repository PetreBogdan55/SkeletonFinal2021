#include "State.h"
#include "Solver.h"

#include <ctime>
#include <iostream>

template <size_t N>
static void Test(const State<N>& initialState)
{
    try
    {
        clock_t begin = clock();

        Moves&& result = Solver::SolveBFS(initialState);


        std::cout << "Found solution in " << result.size() << " moves:" << std::endl;
        
        std::for_each(result.begin(), result.end(), [](auto element) {std::cout << element << " "; });
		/*for (auto it = result.begin(); it != result.end(); it++)
        {
            std::cout << *it << " ";
        }  */      

        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

        std::cout << std::endl << elapsed_secs << "s " << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

int main()
{
    State3X3 state_3x3_18moves{ { 3, 6, 4, 7, 2, 1, 5, 8, 0 } }; // 18 moves
    //State4X4 state_4x4_50_moves{ { 12, 15, 6, 10, 4, 9, 5, 8, 14, 13, 0, 2, 1, 7, 11, 3 } }; //50 moves
    State4X4 state_4x4_50_moves{ { 15, 14, 1, 6, 9, 11, 4, 12, 0, 10, 7, 3, 13, 8, 5, 2 } }; // 52 moves
    //State4X4 state_4x4_50_moves{ { 15, 2, 1, 12, 8, 5, 6, 11, 4, 9, 10, 7, 3, 13, 14, 0 } }; // 54 moves
    //State4X4 state_4x4_50_moves{ { 7, 4, 1, 13, 15, 14, 5, 10, 0, 9, 11, 12, 2, 8, 6, 3 } }; // 60 moves 
    //State4X4 state_4x4_50_moves{ { 0, 12, 9, 13, 15, 11, 10, 14, 3, 7, 2, 5, 4, 8, 6, 1 } }; // NA 80 moves? this didn't solve in A* and didn't run long enough with IDA*.

    Test(state_3x3_18moves);

	system("pause");
    return 0;
}