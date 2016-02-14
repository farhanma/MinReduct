#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <math.h>
#include <string>
#include <limits>
#include "timer.h"
#include <omp.h>

using namespace std;

typedef struct returnfactor {
	vector< vector<string> > matrix;
	vector<string> subsets;
	int rows;
	int columns;
} vecIntInt;

class Preparation {
	public:
		vecIntInt readInputFile () {
			vecIntInt d; //return value
			ifstream file("data.csv"); // read input file
			if(!file.is_open()){ // make sure the file is available 
				cout << "File couldn't be opened ...\n";
				exit(0); // exit the program
			}
			string firstline;
			getline(file, firstline);  // read the first line (attributes)
			int columns =  count(firstline.begin(), firstline.end(), ',');// count how many attributes
			columns += 1; //number of columns == to number of ',' that's why we need to add one more
			string dumpline; //just a line to assign the value to the variable vector
			vector<string> filevec; // vector for the input file "using vector for the sake of the variable size that he offers"
			while (file.good()) { //boolean that returns false if badbit, eofbit or failbit, otherwise it returns true
		     	getline(file, dumpline);
				filevec.push_back(dumpline);//push each line in the vector	
			}
			file.close();//close the file
			//noticed if the csv file has been edited by excel application, excel automatically add an empty row int the end of the file
			if(filevec[filevec.size()-1] == ""){// make sure that the last element dose not equal empty string
				filevec.pop_back();//if so, remove the last element
			}
			vector< vector<string> > matrix;
			d.matrix = processVec(filevec.size(), columns, matrix, filevec);
			d.rows = d.matrix.size();
			d.columns = columns;
			d.subsets = getAllSubsets(columns);
			return d;
		}

	private:
		vector< vector<string> > processVec (int N, int M, vector< vector<string> > A, vector<string> V) {
			string temp_1, temp_2;
			for (int i = 0; i < N; i++) {
				temp_1 = trim(V[i]);
				vector<string> row;
				for (int j = 0; j < M; j++) { 
					temp_2 = temp_1.substr(0,temp_1.find_first_of(','));
					row.push_back(temp_2);
					temp_1 = temp_1.erase(0,temp_1.find_first_of(',') + 1); //remove treated element
				}
				A.push_back(row);
			}
			return A;
		}
		//trim function
		string trim (const string &str) {
			string s = str;
			size_t at_2 = s.find_last_not_of(" \t\r\n\0\a\b\f\v");//whitespace at the end
			size_t at_1 = s.find_first_not_of(" \t\r\n\0\a\b\f\v");//whitespace at the beginning 
		        if (at_2 != string::npos) {
				s.erase(at_2+1);
			}
			if (at_1 != string::npos) {
				s.erase(0,at_1);
			}
		   	return s;
		}
		//list the all subsets in a set
		vector<string> getAllSubsets (int columns) {
			//initialize subsets vector
			vector<string> subsets;
			for (int i = 1; i < columns; i++) {
				stringstream s;
				s << i;
				//s << ";";
				subsets.push_back(s.str());//add the original set
			}
			//start expanding the vector based on the original set
			int mxx = ((pow(2,(columns-1))) - (columns-1));
			for (int i = 0; i < mxx; i++) {
				int m = 0;
				string a = subsets[i];
				if (i > columns-2) {
					a = a.substr(a.find_last_of(';')+1);
				}
				if (atoi(a.c_str()) == columns-1) {
					continue;
				}
				m = atoi(a.c_str()) + 1;
				for(int j = m; j < columns; j++) {
					stringstream s;
					s << ';';
					s << j;
					string c = subsets[i] + s.str();
					subsets.push_back(c);
				}

			}
			return subsets;
		}

};

class MinReduct {
	public:
		bool init (vector< vector<string> > matrix, string subset, int rows, int columns) {
			bool b = false;
			if (isReduct (matrix, subset, rows, columns)) {
				b = true;
			}
			return b;
		}
	private:
		bool isReduct (vector< vector<string> > matrix, string subset, int rows, int columns) {
			int co =  count(subset.begin(), subset.end(), ';');// count how many attributes
			co = co + 1;
			bool seperable = false;
			for(int i = 0; i < rows-1; i++) {
				for (int j = i+1; j < rows; j++) {
					if (matrix[i][columns-1] != matrix[j][columns-1]) {
						string workingStr = subset;
						//if (!seperable) {
						for (int k = 0; k < co; k++) {
							string temp;
							if ( k != co-1 ) {
								temp = workingStr.substr(0, workingStr.find_first_of(';'));
							} else {
								temp = workingStr;
							}
							int kk = atoi(temp.c_str());
							kk = kk - 1;
							if (matrix[i][kk] != matrix[j][kk]) {
								seperable = true;
								break;
							}
							if ( k != co-1) {
								workingStr = workingStr.erase(0,workingStr.find_first_of(';') + 1);//remove treated element 
							}
							seperable = false; 
						}
						//}
						if (seperable == false) {
							return seperable;
						}
					}
				}
			}
			return seperable;
		}
};
int main () {

	StartTimer();
	vecIntInt d;
	vector< vector<string> > matrix;
	vector<string> subsets;
	int rows;
	int columns;
	Preparation p;
	d = p.readInputFile();
	matrix = d.matrix;
	rows = d.rows;
	columns = d.columns;
	subsets = d.subsets;
	cout << subsets.size() << endl;
	
	//for (int unsigned i = 0; i < subsets.size(); i++) {
	//	cout << subsets[i] << endl;
	//}

	//for (int i = 0; i < rows; i++) {
	//	for (int j = 0; j < columns; j++) {
	//		cout << matrix[i][j] << "\t";
	//	}
	//	cout << endl;
	//}
	int minCard = numeric_limits<int>::max(); // set the minimum cardinality to infinity
	MinReduct minReduct;
	int tt = 0;
	#pragma omp parallel for
	for (int unsigned i = 0; i < subsets.size(); i++) {
		int co =  count(subsets[i].begin(), subsets[i].end(), ';');// count how many attributes
		co = co + 1;
		if (minReduct.init(matrix, subsets[i], rows, columns)) {
			if (co < minCard) {
				minCard = co;
				tt = i;
			} 
		}
		cout <<"Iteration #: " << i << ", SUBSETS PASSED!!!" << endl;
	}
	cout << endl;
	cout << endl;
	double runTime = GetTimer();
	cout << "Run time is: " << runTime/1000.f << " sec" << endl;
	cout << minCard << endl;
	cout << subsets[tt] << endl;

	return 0;
}
