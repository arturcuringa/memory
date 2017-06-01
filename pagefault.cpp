#include <iostream>
#include <fstream>

long unsigned int find_fl(std::string pid, int numOfInt){
	std::ifstream fs;
	long unsigned int fl=0;
	std::string teste;
	fs.open(std::string("/proc/"+ pid +"/stat").c_str(), std::ifstream::in );
	
	if(fs.is_open()){
		for (int i=1; i<numOfInt; ++i){
			fs>>teste;
		}
		fs>>fl;
	}
	return fl;
}
long unsigned int find_minfl(std::string pid){
	return find_fl(pid,10);
}

long unsigned int find_majfl(std::string pid){
	return find_fl(pid,12);
}

int main(void){
	std::cout<<find_minfl("1")<<std::endl;
	std::cout<<find_majfl("1")<<std::endl;
	return 0;
}