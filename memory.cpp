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
		std::string rss = exec((std::string( "cat " + path + "smaps | grep -i Rss | tr -s [:space:] | tr -d [:alpha:] | tr -d [:punct:] | awk '{ SUM += $1} END { print SUM }'")).c_str());
		if(isInteger (rss))
		{
			x.rss = stoi(rss);
		}
		std::string pss = exec((std::string( "cat " + path + "smaps | grep -i Pss | tr -s [:space:] | tr -d [:alpha:] | tr -d [:punct:] | awk '{ SUM += $1} END { print SUM }'")).c_str());
		if( isInteger(pss))
		{
			x.pss = stoi(rss);
		}
		std::string swap = exec((std::string( "cat " + path + "smaps | grep -i swap | tr -s [:space:] | tr -d [:alpha:] | tr -d [:punct:] | awk '{ SUM += $1} END { print SUM }'")).c_str());
		if(isInteger(swap))
		{
			x.swap = stoi(rss);
		}
		if(isInteger(swap) && isInteger(pss) && isInteger(rss))
			std::cout << line << " " << x.rss << " " << x.pss << " " << x.swap << std::endl;
		//std::cout << line << std::endl;
	}	
}