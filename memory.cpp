#include <cstdio>
#include <iostream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <cstring>
#include <array>
#include <queue>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <algorithm>
#include <iomanip>

typedef struct{
	int pid;
	long unsigned int rss;
	long unsigned int pss;
	long unsigned int swap;
	long unsigned int cache;
	long unsigned int majfl;
	long unsigned int minfl;


}process;

struct functor 
{
	int _x;
	functor(int x){_x = x;}
	bool operator()(const process a, const process b)
	{
		switch(_x)
		{
			case 0:
				return a.pid < b.pid;
				break;
			case 1:
				return a.rss > b.rss;
				break;
			case 2:
				return a.pss > b.pss;
				break;
			case 3:
				return a.swap > b.swap;
				break;
			case 4:
				return a.cache > b.cache;
				break;
			case 5:
				return a.majfl > b.majfl;
				break;
			case 6:
				return a.minfl > b.minfl;
				break;

		}
	}
};
/////////////////////////////////////////////
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
////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////

int main(int argc, char* argv[]){
	//while (true){
	int f_x;
	if(argc > 1)
	{
		if(std::string(argv[1]) == "-pid")
		{
			f_x = 0;
		}
		else if(std::string(argv[1]) == "-rss")
		{
			f_x = 1;
		}
		else if(std::string(argv[1]) ==  "-pss")
		{
			f_x = 2;

		}
		else if(std::string(argv[1]) == "-swap")
		{
			f_x = 3;
		}
		else if(std::string(argv[1]) == "-majf")
		{
			f_x = 5;
		}
		else if(std::string(argv[1]) ==  "-minf")
		{
			f_x = 6;
		}else{
			std::cout<<"Use one of these options (-pss by default): \n"<<
						"-pid\n"<<
						"-rss\n"<<
						"-swap\n"<<
						"-majf\n"<<
						"-minf\n"<<
						std::endl;
			return 1;
		} 
	}else
	{
		f_x = 2;
	}
	while(true){
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
		
		
		std::cout<< std::setw(8) << std::left << "Total" << 
					std::setw(8) << std::left << "RSS" << //std::setw(8) << std::left << 
					std::setw(8) << std::left << "PSS" << //std::setw(8) << std::left <<
					std::setw(8) << std::left << "CACHE" << std::endl;

		std::cout 	<< std::setw(8) << std::left << " " //<< std::setw(8)//<< "\t" 
					<< std::setw(8) << std::left << y.rss //<< std::setw(8)//<<"\t" 
					<< std::setw(8) << std::left << y.pss //<< std::setw(8)//<<"\t"
					<< std::setw(8) << std::left << y.cache //<< std::setw(8)//<< "\t"
					<< std::endl<<std::endl;

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
				x.majfl = 0;
				x.minfl = 0;

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

				//if(check(std::string("/proc/"+ line +"/stat").c_str())){
					std::ifstream fs;
					std::string trash;

					fs.open(std::string("/proc/"+ line +"/stat").c_str(), std::ifstream::in );
					
					if(fs.is_open()){
						for (int i=1; i<10; ++i){
							fs>>trash;
						}
						fs>>x.minfl;
					}
					fs>>trash;
					fs>>x.majfl;
					
					fs.close();
				//}
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
		
		std::cout<< std::setw(8) << std::left << "PID" << 
					std::setw(8) << std::left << "RSS" << //std::setw(8) << std::left << 
					std::setw(8) << std::left << "PSS" << //std::setw(8) << std::left <<
					std::setw(8) << std::left << "SWAP" << //std::setw(8) << std::left <<
					std::setw(8) << std::left << "MAJFL" << //std::setw(8) << std::left <<
					std::setw(8) << std::left << "MINFL" << std::endl;
	
		sort(p_vector.begin(), p_vector.end(), functor(f_x));
		for(int i= 0; i < 10; ++i){
			std::cout 	<< std::setw(8) << std::left << p_vector[i].pid //<< std::setw(8)//<< "\t" 
						<< std::setw(8) << std::left << p_vector[i].rss //<< std::setw(8)//<<"\t" 
						<< std::setw(8) << std::left << p_vector[i].pss //<< std::setw(8)//<<"\t"
						<< std::setw(8) << std::left << p_vector[i].swap //<< std::setw(8)//<< "\t"
						<< std::setw(8) << std::left << p_vector[i].majfl //<< std::setw(8)//<< "\t"
						<< std::setw(8) << std::left << p_vector[i].minfl //<< "\t"
						<< std::endl;
		}
		for (int i=0; i< 14; ++i)
			std::cout<<"\033[A";
	}
	return 0;
}