#include <iostream>
#include <vector>
#include <fstream>

#define BOUNDARY "-------------------------"

typedef std::vector<std::vector<int>> Map;
typedef std::vector<std::vector<std::vector<int>>> PossibilityMap;

class SodukuSolver{
private:
	Map map;
	bool initialized = false;
	PossibilityMap possiblityMap;

	Map result;

	bool is_solved(){
		if(initialized){
			for(int i = 0; i<9; i++){
				for(int j = 0; j<0; j++){
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

	void do_elimination(int row,int col, Map& curMap){
		//std::vector<int> temp_potential = possiblityMap[row][col];
		for(std::vector<int>::iterator i = possiblityMap[row][col].begin();i<possiblityMap[row][col].end();){
			if(!is_legal(row,col,(*i),curMap)){
				possiblityMap[row][col].erase(i);
			} else{
				i++;
			}
		}
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
		for(int i = 0;i<9;i++){
			for(int j = 0;j<9;j++){
				do_elimination(i,j,map);
			}
		}
		//TODO: add heuristic for doing combination emilination
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
	sodukuSolver.solve();
	sodukuSolver.print_possibility_map();
	return 0;

}