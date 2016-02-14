#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

int getAttributes (string a);//get the header of the file in order to construct the decision table attributes
string trim (const string &s);//define trim function for removing white spaces in a given string

//class from printing formatted output file
class PrettyPrint {
	public:
		ofstream outfile; // file stream
		//print table attributes (f1 ... fn)	
		void printAttributes (int columns) {
			outfile.open("data.csv");//new file
			outfile << ", ";
			for (int i = 1; i < columns; i++) {
				if (i == columns-1) {
					outfile << "S" << i;
				} else {
					outfile << "S" << i << ", ";
				}
			}
			outfile << endl;
			counter = 0;
		}
		//print a row
		void printRows (string s) {
			setCounter(); //increment the counter by 1
			outfile << "e" << getCounter() << ", " << s << endl;
		}
		//print last row (0 ... 0) and last column will be 2
		void printLastRow (int columns) {
			setCounter ();
			outfile << 0 << ",";
			for (int k = 0; k <  columns-1; k++) {
				outfile << 0 << ",";
			}
			outfile << 2;
			outfile.close();//close the file
		}
	private:
		int counter; //count number of added rows
		//increment the value by 1
		int setCounter () {
			counter++;
		}
		//get the final value
		int getCounter () {
			return counter;
		}

};

//Process input file
class FileProcessing {	
	public:
		//class constructor
		FileProcessing (vector<string> v, int columns){
			PrettyPrint prettyPrint;
			prettyPrint.printAttributes (columns);
			string temp_1, temp_2;
			//find two rows with different decision only
			//loop over the whole dataset and combine any two rows with different decisions
			for (int i = 0; i < v.size()-1; i++) {
				temp_1 = trim(v[i].substr(v[i].find_last_of(',') + 1));//clean the string by removing the white spaces 
				for(int j = i+1; j < v.size(); j++){ 
					temp_2 = trim(v[j].substr(v[j].find_last_of(',') + 1));
					//two decision are different 
					if (temp_1.compare(temp_2) != 0){
						cout << dataSetToVCover (v[i], v[j], columns) << endl; //merge them	
						prettyPrint.printRows (dataSetToVCover (v[i], v[j], columns));//print them in the file
					}
				}
		
			}
			//prettyPrint.printLastRow (columns);//close the file
		}
	private:
		//merge two rows in one
		string dataSetToVCover (string a, string b, int size){
			string result = "";
			//if the attributes are same in a given file then they will be equal to 0 in the new file otherwise 1
			for(int i = 0; i < size-1; i++){
				string temp_1 = trim(a.substr(0, a.find_first_of(',')));
				string temp_2 = trim(b.substr(0, b.find_first_of(',')));
				if (temp_1 == temp_2) {
					if (i == size-2) {
						result += "0";
					} else {
						result += "0, ";
					}	
				} else {
					if (i == size-2) {
						result += "1";
					} else {
						result += "1, ";
					}
				}
				a = a.erase(0,a.find_first_of(',') + 1);
				b = b.erase(0,b.find_first_of(',') + 1);

			}
			//result += "1";	
			return result;
		}


};

//read the first line in order to get the total number of attributes that we should work on
int getAttributes (string a) {
	int columns =  count(a.begin(), a.end(), ','); //count the number of commas in a given line
	return ++columns; //increment the columns by 1 before return it back

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

//main function
int main() {
	ifstream file ("dataset.csv");//dataset file name
	if(!file.is_open()){ // file is not available  
		cout << "File couldn't be opened ...\n";//error message
		return 0; // exit the program
	}
	string firstLine;
	getline(file, firstLine);
	int columns = getAttributes(firstLine);	//get number of working attributes 
	vector<string> fileContents; //stores the contents of the file
	string dummyLine; 
	while (file.good()) { //boolean that returns false if badbit, eofbit or failbit, otherwise it returns true
     		getline(file, dummyLine);
		fileContents.push_back(dummyLine);//push each line in the vector	
	}
	file.close();//finishes working with the file. Close the file
	FileProcessing fp(fileContents, columns);//process the file		
	return 0;	

}
