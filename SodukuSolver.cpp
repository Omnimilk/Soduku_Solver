#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <fstream>

#define BOUNDARY "-------------------------"

typedef std::vector<std::vector<int>> Map;
typedef std::vector<std::vector<std::vector<int>>> PossibilityMap;

template<typename T, typename priority_t>
struct PriorityQueue {
    typedef std::pair<priority_t, T> PQElement;
    std::priority_queue<PQElement, std::vector<PQElement>,std::greater<PQElement> > elements;

    inline bool empty() const { return elements.empty(); }

    inline void put(T item, priority_t priority) {
        elements.emplace(priority, item);
    }

    inline T get() {
        T best_item = elements.top().second;
        elements.pop();
        return best_item;
    }
};

class SodukuSolver{
private:
	Map map;
	bool initialized = false;
	PossibilityMap possiblityMap;
    PriorityQueue<int, int> cellsToBeExaminated;
	Map result;

	bool is_solved(){
		if(initialized){
			for(int i = 0; i<9; i++){
				for(int j = 0; j<9; j++){
					if(possiblityMap[i][j].size()!=1){
						return false;
					}
				}
			}
			return true;
		}{
			return false;
		}

	}

	bool is_legal_rowwise(int row, int col, int val, Map& curMap){
		for(int i = 0; i<9;i++){
			if(i!=col && curMap[row][i] == val){
				return false;
			}
		}
		return true;
	}

	bool is_legal_colwise(int row, int col, int val, Map& curMap){
		for(int i = 0; i<9;i++){
			if(i!=row && curMap[i][col] == val){
				return false;
			}
		}
		return true;
	}

	bool is_legal_districtwise(int row, int col, int val, Map& curMap){
		std::vector<int> districtInfo = get_district_info(row,col);//rowStart,rowEnd,colStart,colEnd
		for(int i = districtInfo[0]; i<=districtInfo[1]; i++){
			for(int j = districtInfo[2]; j<=districtInfo[3]; j++){
				if(!(i==row && j==col) && curMap[i][j]==val){
					return false;
				}
			}
		}
		return true;
	}

	bool is_legal(int row, int col, int val, Map& curMap){
		return is_legal_rowwise(row,col,val,curMap) && is_legal_colwise(row,col,val,curMap) && is_legal_districtwise(row,col,val,curMap);
	}

	void do_elimination_for_cell(int row,int col, Map& curMap){
		//std::vector<int> temp_potential = possiblityMap[row][col];
		for(std::vector<int>::iterator i = possiblityMap[row][col].begin();i<possiblityMap[row][col].end();){
			if(!is_legal(row,col,(*i),curMap)){
				possiblityMap[row][col].erase(i);
			} else{
				i++;
			}
		}
        cellsToBeExaminated.put(row*9 + col, possiblityMap[row][col].size());
	}

    void do_combinational_elimination_rowwise(int row, int col, Map& curMap){
        //TODO:combine with col and district?
        std::vector<int> possibilities_in_other_cells(9);
        std::vector<int>::iterator it = possibilities_in_other_cells.begin();
        for(int i = 0;i<9;i++){
            if(i!=col){
                std::vector<int> possibilities_in_prev_cells(possibilities_in_other_cells.begin(),it);
                it = std::set_union(possiblityMap[row][i].begin(),possiblityMap[row][i].end(), possibilities_in_prev_cells.begin(),possibilities_in_prev_cells.end(),possibilities_in_other_cells.begin());
            }
        }
        possibilities_in_other_cells.resize(it - possibilities_in_other_cells.begin());

        if(possibilities_in_other_cells.size()<8){
            std::cout<<"Possibilities in other cells(rowwise) is less than 8, unsolvable! Cell: ["<<row<<", "<<col<<"]."<<std::endl;
            exit(1);
        }else if(possibilities_in_other_cells.size()==8){
            std::vector<int> complete_set = {1,2,3,4,5,6,7,8,9};
            std::vector<int> left_set(9);
            std::vector<int>::iterator it;
            it = std::set_difference(complete_set.begin(),complete_set.end(),possibilities_in_other_cells.begin(),possibilities_in_other_cells.end(),left_set.begin());
            left_set.resize(it-left_set.begin());
            if(left_set.size()!=1){
                std::cout<<"??????????????????????????????????"<<std::endl;
            } else{
                map[row][col] = left_set[0];
                std::cout<<"Setting cell ["<<row<<", "<<col<<"] with value "<<left_set[0]<<std::endl;
            }

        }else{
            //no finding
        }
        return;
    }

    void do_combinational_elimination_colwise(int row, int col, Map& curMap){
        std::vector<int> possibilities_in_other_cells(9);
        std::vector<int>::iterator it = possibilities_in_other_cells.begin();
        for(int i = 0;i<9;i++){
            if(i!=row){
                std::vector<int> possibilities_in_prev_cells(possibilities_in_other_cells.begin(),it);
                it = std::set_union(possiblityMap[i][col].begin(),possiblityMap[i][col].end(), possibilities_in_prev_cells.begin(),possibilities_in_prev_cells.end(),possibilities_in_other_cells.begin());
            }
        }
        possibilities_in_other_cells.resize(it - possibilities_in_other_cells.begin());

        if(possibilities_in_other_cells.size()<8){
            std::cout<<"Possibilities in other cells(colwise) is less than 8, unsolvable! Cell: ["<<row<<", "<<col<<"]."<<std::endl;
            exit(1);
        }else if(possibilities_in_other_cells.size()==8){
            std::vector<int> complete_set = {1,2,3,4,5,6,7,8,9};
            std::vector<int> left_set(9);
            std::vector<int>::iterator it;
            it = std::set_difference(complete_set.begin(),complete_set.end(),possibilities_in_other_cells.begin(),possibilities_in_other_cells.end(),left_set.begin());
            left_set.resize(it-left_set.begin());
            if(left_set.size()!=1){
                std::cout<<"??????????????????????????????????"<<std::endl;
            } else{
                map[row][col] = left_set[0];
                std::cout<<"Setting cell ["<<row<<", "<<col<<"] with value "<<left_set[0]<<std::endl;
            }

        }else{
            //no finding
        }
        return;
    }

    void do_combinational_elimination_districtwise(int row, int col, Map& curMap){
        std::vector<int> possibilities_in_other_cells(9);
        std::vector<int>::iterator it = possibilities_in_other_cells.begin();
        std::vector<int> districtInfo = get_district_info(row,col);//rowStart,rowEnd,colStart,colEnd
        for(int i = districtInfo[0]; i<=districtInfo[1]; i++){
            for(int j = districtInfo[2]; j<=districtInfo[3]; j++){
                if(!(i==row && j==col)){
                    std::vector<int> possibilities_in_prev_cells(possibilities_in_other_cells.begin(),it);
                    it = std::set_union(possiblityMap[i][col].begin(),possiblityMap[i][col].end(), possibilities_in_prev_cells.begin(),possibilities_in_prev_cells.end(),possibilities_in_other_cells.begin());
                }
            }
        }
        possibilities_in_other_cells.resize(it - possibilities_in_other_cells.begin());

        if(possibilities_in_other_cells.size()<8){
            std::cout<<"Possibilities in other cells(colwise) is less than 8, unsolvable! Cell: ["<<row<<", "<<col<<"]."<<std::endl;
            exit(1);

        }else if(possibilities_in_other_cells.size()==8){
            std::vector<int> complete_set = {1,2,3,4,5,6,7,8,9};
            std::vector<int> left_set(9);
            std::vector<int>::iterator it;
            it = std::set_difference(complete_set.begin(),complete_set.end(),possibilities_in_other_cells.begin(),possibilities_in_other_cells.end(),left_set.begin());
            left_set.resize(it-left_set.begin());
            if(left_set.size()!=1){
                std::cout<<"??????????????????????????????????"<<std::endl;
            } else{
                map[row][col] = left_set[0];
                std::cout<<"Setting cell ["<<row<<", "<<col<<"] with value "<<left_set[0]<<std::endl;
            }

        }else{
            //no finding
        }
        return;
    }

    void do_combinational_elimination_for_cell(int row, int col, Map& curMap){
        do_combinational_elimination_rowwise(row,col,curMap);
        do_combinational_elimination_colwise(row,col,curMap);
        do_combinational_elimination_districtwise(row,col,curMap);
        return;
    }

	bool read_in_map(std::string filename){
		std::ifstream map_file(filename);
		std::string line;
		int row = 0;
		int actual_line = 0;
		if(map_file.is_open()){
			while(getline(map_file,line)){
				row++;
				if(line.empty() || line.at(0) == '#'){ continue;} //ignore the commented or empty lines

				if(line.size() != 9){
					std::cout<<"Map file format is not correct. Check line "<<row<<"."<<std::endl;
					return false;
				}

				for(int i =0; i<9;i++){
					int value = int(line.at(i))-48;
					//std::cout<<"location "<<row<<" "<<i<<" with value "<<value<<"and char"<<line.at(i)<<std::endl;
					if(is_valid_value(value)){
						map[actual_line][i] = value;
					} else{
						std::cout<<"Map file format is not correct. Check line "<<row<<" col "<<i+1<<"."<<std::endl;
						return false;
					}
				}
				actual_line++;
			}
			map_file.close();
		} else{
			std::cout<<"Cannot open map file. Check if "<< filename<<" is correct?"<<std::endl;
			return false;
		}

		if(actual_line!=9){
			std::cout<<"Make sure you have 9 lines for soduku map"<<std::endl;
			return false;
		}
		return true;
	}

	bool is_valid_value(int i){
		return i>=0 && i<=9;//0 means unknown cell
	}

	std::vector<int> get_district_info(int row, int col){
		//rowStart,rowEnd,colStart,colEnd
		int rowStart = (row/3)*3;
		int rowEnd = rowStart +2;
		int colStart = (col/3)*3;
		int colEnd = colStart +2;
		std::vector<int> res = {rowStart,rowEnd,colStart,colEnd};
		return res;
	}

	void build_possibility_map(){
		if(initialized){
			std::vector<int> potential_values = {1,2,3,4,5,6,7,8,9};
			for(int i = 0; i<9; i++){
				for(int j = 0; j<9; j++){
					if(map[i][j] != 0){
						std::vector<int> prob = {map[i][j]};
						possiblityMap[i][j] = prob;
					} else{
						possiblityMap[i][j] = potential_values;

					}
				}
			}
		} else{
			std::cout<<"Initialize Soduku map first!"<<std::endl;
		}
	}

    void do_elimination(){
        cellsToBeExaminated.empty();
        for(int i = 0;i<9;i++){
            for(int j = 0;j<9;j++){
                if(map[i][j]==0){ do_elimination_for_cell(i,j,map);}
            }
        }
    }

public:
	SodukuSolver(std::string filename){
		map = Map(9,std::vector<int>(9));
		possiblityMap = PossibilityMap(9,std::vector<std::vector<int>>(9,std::vector<int>(9)));
		initialized = read_in_map(filename);

		if(initialized){
			std::cout<<"map initialized successfully"<<std::endl;
			build_possibility_map();
		} else{
			std::cout<<"map initialization failed"<<std::endl;
			exit(1);
		}
	}

	void solve(){
		//TODO: add heuristic for doing combination emilination
        //naive try: try all combinations after elimination
        while(!is_solved()){
            do_elimination();
            int cell_code = cellsToBeExaminated.get();
            int row = cell_code/9;
            int col = cell_code%9;
            do_combinational_elimination_for_cell(row,col,map);
        }
	}

	void print_sol(){
		for(int i = 0; i < 9; i++){
			if(i%3 ==0){
				std::cout<<BOUNDARY<<std::endl;
			}
			for(int j = 0; j < 9; j++){
				if(j%3 ==0){
					std::cout<<"| ";
				}
				std::cout<< map[i][j]<< " ";
			}
			std::cout<<"|"<<std::endl;
		}
		std::cout<<BOUNDARY<<std::endl;
		return;
	};

	void print_possibility_map(){
		for(int i = 0; i < 9; i++){
			for(int j = 0; j < 9; j++){
				std::cout<<"cell "<< i+1 <<" "<<j+1<<": ";
				for(int k = 0; k<possiblityMap[i][j].size(); k++){
					std::cout<< possiblityMap[i][j][k]<< " ";
				}
				std::cout<<std::endl;
			}

		}
		return;
	}
};

int main(){
	SodukuSolver sodukuSolver("map.txt");
    sodukuSolver.print_sol();
    sodukuSolver.print_possibility_map();
	sodukuSolver.solve();

	return 0;

}