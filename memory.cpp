#include <cstdio>
#include <iostream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <queue>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>

typedef struct{
	int pid;
	long unsigned int rss;
	long unsigned int pss;
	long unsigned int swap;
	long unsigned int cache;
	long unsigned int majfl;
	long unsigned int minfl;
}process;

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != NULL)
            result +=buffer.data();
    }
    return result;
}

std::string exec(const char* path, const char* cmd) {
	struct stat sb;

	if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode)){
		return exec(cmd);
	}return "";
}

bool check(const char* path) {
	struct stat sb;

	if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode)){
		return true;
	}
	return false;
}

bool isInteger(const std::string & s)
{
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;
   return true;
}
int main(){
	//while (true){
	std::istringstream f (exec((std::string("ps aux | awk '{print $2}' ")).c_str()));
	std::string line;
	getline(f,line);
	std::vector<process> p_vector;
	
	//Global
	process y;
	y.pid = -1;
	y.rss = stoi(exec((std::string("cat /proc/meminfo | grep -i MemTotal | tr -s [:space:] | tr -d [:alpha:] | tr -d [:punct:]")).c_str()));
	y.pss = y.rss;
	y.swap = stoi(exec((std::string("cat /proc/meminfo | grep -i SwapTotal | tr -s [:space:] | tr -d [:alpha:] | tr -d [:punct:]")).c_str())); 
	y.cache = stoi(exec((std::string("cat /proc/meminfo | grep -i Cached | tr -s [:space:] | tr -d [:alpha:] | tr -d [:punct:]")).c_str()));
	std::cout << " Total" << " " << y.rss << " " << y.pss << " " << y.swap << " " << y.cache << std::endl;
	
	while(getline(f,line))
	{

		std::string path = "/proc/" + line + "/";
		
		process x;
		x.pid = stoi(line);
		
		if(check(path.c_str()))
		{
			std::ifstream f(path+ "smaps");
			std::string l;
			x.rss = 0;
			x.pss = 0;
			x.swap = 0;
			long unsigned int rss, pss, swap;
			while(getline(f,l))
			{
				//std::cout << l << std::endl;
				if(l.compare(0, 3, "Rss") == 0)
				{
					std::string trash;
					std::stringstream ss(l);
					//std::cout<< l << std::endl;
					ss >> trash;
					ss >> rss;
					x.rss += rss;
				}
				if(l.compare(0, 3, "Pss") == 0)
				{
					std::string trash;
					std::stringstream ss(l);
					ss >> trash;
					ss >> pss;
					x.pss += pss;
				}
				if(l.compare(0, 4, "Swap") == 0)
				{
					std::string trash;
					std::stringstream ss(l);
					ss >> trash;
					ss >> swap;
					x.swap += swap;
				}
			}
			// std::string maj = exec(path.c_str(),std::string("ps -o maj_flt "+ line +"| awk 'NR==2'").c_str());
			// std::string min = exec(path.c_str(),std::string("ps -o min_flt "+ line +"| awk 'NR==2'").c_str());
	
			// x.majfl = stoi(maj);
			// x.minfl = stoi(min);
			p_vector.push_back(x);
		}

		// std::string rss = exec(path.c_str(),(std::string( "sudo cat " + path + "smaps | grep -i Rss | tr -s [:space:] | tr -d [:alpha:] | tr -d [:punct:] | awk '{ SUM += $1} END { print SUM }'")).c_str());
		// std::string pss = exec(path.c_str(),(std::string( "sudo cat " + path + "smaps | grep -i Pss | tr -s [:space:] | tr -d [:alpha:] | tr -d [:punct:] | awk '{ SUM += $1} END { print SUM }'")).c_str());
		// std::string swap = exec(path.c_str(),(std::string( "sudo cat " + path + "smaps | grep -i swap | tr -s [:space:] | tr -d [:alpha:] | tr -d [:punct:] | awk '{ SUM += $1} END { print SUM }'")).c_str());
		
		//std::cout << line << std::endl;
	 }
	std::cout<< "PID" << "\t" << "RSS" <<"\t"<< "PSS" <<"\t"
				<< "SWAP" << "\t"<< "MAJFL" << "\t" << "MINFL" << std::endl;
	
	while(true){
		for(int i= 0; i < 10; ++i){
			std::cout << p_vector[i].pid << "\t" 
						<< p_vector[i].rss <<"\t" 
						<< p_vector[i].pss <<"\t"
						<< p_vector[i].swap << "\t"
						<< std::endl;
		}
		for (int i=0; i< 10; ++i)
			std::cout<<"\033[A";
	}



	return 0;
}