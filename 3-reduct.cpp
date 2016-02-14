#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <algorithm> 
#include "timer.h"

using namespace std;

typedef struct myData {
	vector<string> filevec; //file contents
	int columns; //number of columns
} myData;
vector<int> constructed_cover; //global variable for the constructed cover


myData readInputFile (); //read input file
int **mem_Alloc (int **a, int n, int m); //allocate memory dynamically
string trim (const string &s);//define trim function for removing white spaces in a given string

//print the result whether in file or console
class PrettyPrint {
	public:		
		void vPrint (int **table, int rows, int columns, bool type) {//specify type of printing request
			if (type) {
				prettyPrint_File(table, rows, columns);//file
			} else {
				prettyPrint_Cons(table, rows, columns);//console
			}
		}

	private:
		//file
		void prettyPrint_File (int **table, int rows, int columns) {
			//FILE *output = fopen("output.csv", "w");
			ofstream output;
			output.open("output.csv");
			for(int i = 1; i < (columns-1); i++){
				if(table[rows-1][i] != -1){
					//fprintf(output, "%s%d%s", "S",i,",");
					output << "S" << i << ",";
				}
			}
			//fprintf(output, "%s\n", "D");
			output << "D" << endl;
			for(int i = 1; i < (columns-1); i++){
				if(table[rows-1][i] != -1){	
					//fprintf(output, "%s", "c,");
					output << "c,";
				}
			}
			//fprintf(output, "%s\n", "c");
			output << "c" << endl;
			for(int i = 0; i < rows; i++){
				//final_results.push_back(vector<int>());
				if(table[i][columns-1] != -1){
					for(int j = 1; j < columns; j++){
						if(table[i][columns-1] != -1 && table[rows-1][j] != -1){
							if(j != columns-1){
								//fprintf(output, "%d%s", table[i][j],",");
								output << table[i][j] << ",";
							} else{
								output << table[i][j];
								//fprintf(output, "%d", table[i][j]);
							}
						}
					}
					if(i != (rows-1)){
						output << endl;
						//fprintf(output, "\n");
					}
				}
		
			}
			output.close();	
		}
		//console
		void prettyPrint_Cons (int **table, int rows, int columns) {
			//output the results in the console 
			for(int i = 0; i < rows; i++){
				for(int j = 0; j < columns; j++){
					printf("%d\t", table[i][j]);
				}
				printf("\n");
			}

		}


};

class ReductionSteps {
	public:
		int **mainCall (int **table, int rows, int columns) {
			//these two variables are used for making sure that there were modifying 
			//for the table after the three reduction steps
			//if there were then, the steps should be repeated again
			//otherwise break the loop
			int val = 0; //current value of the value of the last row and column
			int oldval = 0; //old value of the value of the last row and column
			ofstream file_results;
			file_results.open("Results.txt");
			file_results << "**** Read the input file that contains set cover problem";
			file_results << "\n**** Reduced the set cover table to decision table that contains " << rows-1 << " rows and " << columns-2 << " columns";
			file_results << "\n**** Optimization steps: Start";
			int counter = 1;
			int counter_row = 0;
			int counter_col = 0;
			while(true){
				file_results << "\n**** Iteration #"<< counter <<": Start";
				//step 1:
				table = step_One (table, rows, columns);
				file_results << "\n**** Step 1: Done";
				//step 2:
				table = step_Two (table, rows, columns);
				file_results << "\n**** Step 2: Done";
				//step 3:
				table = step_Three (table, rows, columns);
				file_results << "\n**** Step 3: Done";				
				//make sure that there is a change in after the fist iteration
				file_results << "\n**** Iteration #"<< counter <<": Results:\n";
				int t1 = counter_row;
				int t2 = counter_col;
				counter_row = 0;
				counter_col = 0;
				for (int i = 0; i < rows; i++) {
					if (table[i][columns-1] == -1) {
						//file_results << "**** ROW [" << i << "]" << " Has been deleted.\n";
						counter_row++;
					}
				}
				for (int i = 0; i < columns; i++) {
					if (table[rows-1][i] == -1) {
						counter_col++;
						//file_results << "**** COLUMN [" << i << "]" << " Has been deleted.\n";
					}
				}
				file_results << "**** TOTAL NUMBER OF DELETED ROWS IS: "<< counter_row-t1 << ".\n";
				file_results << "**** TOTAL NUMBER OF DELETED COLUMNS IS: "<< counter_col-t2 << ".\n";
				oldval = val;		
				table[rows-1][columns-1] = 0;
				for(int m = 0; m < (rows-1); m++){
					if(table[m][columns-1] == -1){
						table[rows-1][columns-1] += table[m][columns-1];	
					}
				}
				for(int m = 0; m < (columns-1); m++){
					if(table[rows-1][m] == -1){
						table[rows-1][columns-1] += table[rows-1][m];	
					}
				}
				val = table[rows-1][columns-1];
				file_results << "\n**** Iteration #"<< counter <<": End";
				counter++;
				//no change 
				if(val == oldval){
					break; //break the loop
				}
				cout << "\n***** One iteration\n\n";
			}
			file_results << "\n**** Optimization steps: End";
			file_results << "\n**** Constructed Cover is: ";

			for(int i= 0; (unsigned) i < constructed_cover.size(); i++){
				file_results << "S" << constructed_cover[i] << " ";
			}
			file_results.close();
			return table;
			
		}
	private:
		int **step_One (int **table, int rows, int columns) {
			//first step compare two rows if one is greater or equal compared to other then we must delete 
			//the greater one and keep his value as a part of a constructed cover.
			//--------------- 1 ---------------
			for(int i = 0; i < (rows-2); i++){ //first row
				if(table[i][columns-1] != -1){ //make sure a row has not been deleted before	
					for(int j = (i+1); j < (rows-1);j++){ //next row
						if(table[j][columns-1] != -1 && table[i][columns-1] != -1){ // make sure a row has not been deleted before
							bool flag = false; //true if there is a difference between the columns in the different rows
							for(int z = 1; z < (columns-1); z++){ //columns 
								if(table[rows-1][z] != -1){
									if(table[i][z] > table[j][z]){
										flag = true; //the first one should be greater or equal the second one
										bool not_deletable = false; //the first step could not be applied for those rows	
										for(int k = (z+1); k < (columns-1); k++){//start new loop based on the above condition
											if(table[rows-1][k] != -1){
												if(table[i][k] < table[j][k]){//found the second one greater than the first one 
													not_deletable = true;//the condition could not be applied
													break;//break the loop
												}
											}
										}
										if(!not_deletable){//the condition can be applied 
											table[i][columns-1] = -1;//delete the row
										}
									} else if (table[i][z] < table[j][z]){
										flag = true;//the second one should be greater or equal the first one
										bool not_deletable = false; //the first step could not be applied for those rows	
										for(int k = (z+1); k < (columns - 1); k++){//start new loop based on the above condition
											if(table[rows-1][k] != -1){
												if(table[i][k] > table[j][k]){//found the first one greater than the second one
													not_deletable = true;//the condition could not be applied
													break;//break the loop
												}
											}
										}
										if(!not_deletable){ //the conditon can be applied
											table[j][columns-1] = -1;//delete the row	
										}
									}
									if(flag){//condition applied no need for continue the loop
										break;//break the loop move to the next row
									} else if(z == (columns - 2)){ //both columns are identical
										table[i][columns-1] = -1; //remove the first one
									}
								}
							}
						}
					}
				}	

			}
		
			cout << "\n";
			cout << "Step One ... Done.";	
			//PrettyPrint p;
			//p.vPrint(table, columns, rows, false);
			//end of step 1
			return table;
		}

		int **step_Two (int **table, int rows, int columns) {
			//second step in algorithm: check all rows in the table and delete any row that has only one 1 in the row and the others are zeros. 
			//On that case, the algorithm must remove that row from the table as well as remove the column that has that one,
			//and the attribute that attached to that column will be in any constructed cover
			//--------------- 2 ---------------
			for(int i = 0; i < (rows-1); i++){ //row count
				for(int j = 1; j < (columns-1); j++){//column count
					if(table[i][columns-1] != -1 && table[rows-1][j] != -1){// if theis row and this column are not remvoved before
						bool flag = false;//found 1
						if(table[i][j] == 1){//found 1
							flag = true;//true flag
							bool not_deletable = false;//can be deleted
							for(int k = (j+1); k < (columns-1); k++){// start from the next one
								if(table[rows-1][k] != -1){
									if(table[i][k] == 1){//found other one break the loop
										not_deletable = true;//break the loop the condition is not applied on this row anymore
										break;
									}
								}
							}
							//can we applied the algorithm
							if(!not_deletable){
								table[i][columns-1] = -1;//remove the row
								table[rows-1][j] = -1;//remove the column
								constructed_cover.push_back(j);
								for(int m = 0; m < (rows-1); m++){
									if(table[m][j] == 1){
										table[m][columns-1] = -1;
									}
								}
							}
						}
						if(flag){
							break;//break the loop
						}
					}
				}
			}	

			cout << "\n";	
			//PrettyPrint p;
			//p.vPrint(table, columns, rows, false);
			cout << "Step Two ... Done";
			return table;
		}
		
		int **step_Three (int **table, int rows, int columns) {
			//third step: compare two columns and delete the greater column
			//--------------- 3 ---------------
			for(int i = 1; i < (columns-2); i++){ //first row
				if(table[rows-1][i] != -1){ //make sure a row has not been deleted before	
					for(int j = (i+1); j < (columns-1);j++){ //next row
						if(table[rows-1][j] != -1 && table[rows-1][i] != -1){ // make sure a row has not been deleted before
							bool flag = false; //true if there is a difference between the row in the different columns
							for(int z = 0; z < (rows-1); z++){ //columns 
								if(table[z][columns-1] != -1){
									if(table[z][i] > table[z][j]){
										flag = true; //the first one should be greater or equal the second one
										bool not_deletable = false; //the first step could not be applied for those rows	
										for(int k = (z+1); k < (rows-1); k++){//start new loop based on the above condition
											if(table[k][columns-1] != -1){
												if(table[k][i] < table[k][j]){//found the second one greater than the first one 
													not_deletable = true;//the condition could not be applied
													break;//break the loop
												}
											}
										}
										if(!not_deletable){//the condition can be applied 
											table[rows-1][j] = -1;//delete the column
										}
									} else if (table[z][i] < table[z][j]){
										flag = true;//the second one should be greater or equal the first one
										bool not_deletable = false; //the first step could not be applied for those columns	
										for(int k = (z+1); k < (rows-1); k++){//start new loop based on the above condition
											if(table[k][columns-1] != -1){
												if(table[k][i] > table[k][j]){//found the first one greater than the second one
													not_deletable = true;//the condition could not be applied
													break;//break the loop
												}
											}
										}
										if(!not_deletable){ //the conditon can be applied
											table[rows-1][i] = -1;//delete the column	
										}
									}
									if(flag){//condition applied no need for continue the loop
										break;//break the loop move to the next row
									} else if(z == (rows-2)){ //both columns are identical
										table[rows-1][j] = -1; //remove the first one
									}
								}
							}
						}
					}
				}	

			}

			cout << "\n";	
			//PrettyPrint p;
			//p.vPrint(table, columns, rows, false);
			cout << "Step Three ... Done";
			return table;

		}
};

int main(){

	StartTimer();	

	vector<string> filevec; //input file contents
	myData d;
	d = readInputFile(); //read input file	
	filevec = d.filevec;
	int columns = d.columns;
	int rows;
	rows = filevec.size(); //rows == vector size
	rows = rows+1;
	columns = columns+1;
	int **table;//working matrix initailization
	table = mem_Alloc(table, rows, columns); //allocate the matrix in the memory
	//process the vector (input file originally) and add the clean results to the working matrix 
	for(int i = 0; i < rows; i++){
		string temp = "";
		if (i != rows-1) {
			temp = filevec[i];
			if (temp.substr(0,1) == "e") {
			 	temp = temp.erase(0,1);//remove e
			}
			temp = trim(temp);
		}
		for(int j = 0; j < columns; j++){
			if (i == rows-1) {
				table[i][j] = 0;
			} else {
				string temp2 = temp.substr(0, temp.find_first_of(','));//remove the commas
				table[i][j] = atoi(temp2.c_str());//take the integer value of the string and place it in the matrix
				temp = temp.erase(0,temp.find_first_of(',') + 1);//remove treated element
			}
		}
		if (i != rows-1) {
			table[i][columns-1] = 1;
		} else {
			table[i][columns-1] = 2;
		}
	}

	PrettyPrint p;
	p.vPrint(table, rows, columns, false);//validate the results
	
	ReductionSteps rS;
	table = rS.mainCall(table, rows, columns);	
	cout << "\n\n";
	table[rows-1][columns-1] = 2;

	p.vPrint(table, rows, columns, true);
	
	cout << "\nConstructed Cover is: ";

	for(int i= 0; (unsigned) i < constructed_cover.size(); i++){
		cout << "S" << constructed_cover[i] << " ";
	}

	cout << "\n";

	
	double runtime = GetTimer();
	printf("execution time: %f s\n", runtime / 1000.f);	

	return 0;
}

myData readInputFile () {
	myData d; //return value
	ifstream file("data.csv"); // read input file
	if(!file.is_open()){ // make sure the file is available
		cout << "File couldn't be opened ...\n";
		exit(0); // exit the program
	}
	string firstline;
	getline(file, firstline);  // read the first line (attributes)
	//cout << "*** The attributes ... {" << firstline.substr(2,firstline.find('D') - 3) << "} ...\n"; // validate the first line
	int columns =  count(firstline.begin(), firstline.end(), ',');// count how many attributes
	columns += 1; //number of columns == to number of ',' that's why we need to add one more
	//cout << "*** Number of working attributes is: " << columns << " ...\n"; 
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

	d.columns = columns;
	d.filevec = filevec;
	return d;
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

int **mem_Alloc (int **a, int n, int m) {

	int *temp; //temp 1D matrix pointer-double
	a = (int**) malloc(n * sizeof(int**)); //allocate columns
	temp = (int*) malloc(n * m * sizeof(int*)); //allocate rows
	//memory allocation and initialization
	for(int i = 0; i < n;i++){
		a[i] = temp + (i * m); //allocate 2D matrix contiguously in the memory  
		memset(a[i], 0, m * sizeof(int));//initialization to 0s
	}

	return a;

}
