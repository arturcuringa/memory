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

bool isInteger(const std::string & s)
{
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;
   return true;
}
int main(){
	
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
		
		std::string rss = exec(path.c_str(),(std::string( "sudo cat " + path + "smaps | grep -i Rss | tr -s [:space:] | tr -d [:alpha:] | tr -d [:punct:] | awk '{ SUM += $1} END { print SUM }'")).c_str());
		std::string pss = exec(path.c_str(),(std::string( "sudo cat " + path + "smaps | grep -i Pss | tr -s [:space:] | tr -d [:alpha:] | tr -d [:punct:] | awk '{ SUM += $1} END { print SUM }'")).c_str());
		std::string swap = exec(path.c_str(),(std::string( "sudo cat " + path + "smaps | grep -i swap | tr -s [:space:] | tr -d [:alpha:] | tr -d [:punct:] | awk '{ SUM += $1} END { print SUM }'")).c_str());
		std::string maj = exec(std::string("ps -o min_flt,maj_flt"+ line +"| awk 'NR==2 {print $1}'").c_str())
		std::string min = exec(std::string("ps -o min_flt,maj_flt"+ line +"| awk 'NR==2 {print $2}'").c_str())
		
		if(isInteger (rss) && isInteger(pss) && isInteger(swap) && isInteger(maj) && isInteger(min))
		{
			x.rss = stoi(rss);
			x.pss = stoi(rss);
			x.swap = stoi(rss);
			x.majfl = stoi(maj);
			x.minfl = stoi(min);
			p_vector.push_back(x);
		}
		//std::cout << line << std::endl;
	}


	return 0;
}