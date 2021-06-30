#ifndef __TTUtility_HPP__
#define __TTUtility_HPP__

#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <netinet/in.h>
#include <string>
#include <thread>

#define var2str(var) #var

int atoi(const char* begin, const char* end);
bool isNumber(const std::string &s);
bool isPositiveInteger(const std::string &s);
std::size_t index(const std::thread::id id);

inline std::string getTime(std::chrono::high_resolution_clock::time_point theTime, const char *format, bool withMicroSeconds = true) {
	std::time_t rawTime = std::chrono::system_clock::to_time_t(theTime);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&rawTime), format);
	if (withMicroSeconds)
		ss << std::setfill('0') << std::setw(6) << std::chrono::duration_cast<std::chrono::microseconds>(theTime.time_since_epoch()).count() % 1000000;
	return ss.str();
}

inline std::string getTime(std::chrono::high_resolution_clock::time_point theTime) {
	std::time_t rawTime = std::chrono::system_clock::to_time_t(theTime);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&rawTime), "%Y%m%d_%H%M%S_")
		<< std::setfill('0') << std::setw(6) << std::chrono::duration_cast<std::chrono::microseconds>(theTime.time_since_epoch()).count() % 1000000
	;
	return ss.str();
}

inline std::string getTime() {
	return getTime(std::chrono::system_clock::now());
}

inline std::tuple<long long/*日*/,int/*時*/,int/*分*/,int/*秒*/,long/*微秒*/> us2ddhhmmssus(long long total_us/*總微秒*/) {
	const auto us = total_us % (1000 * 1000); total_us /= (1000 * 1000);
	const auto ss = total_us % 60;            total_us /= 60;
	const auto mm = total_us % 60;            total_us /= 60;
	const auto hh = total_us % 24;            total_us /= 24;
	return {total_us, hh, mm, ss, us};
}

inline const std::string& valEmpty(const std::string &value, const std::string &defaultValue) {
	return value==""? defaultValue: value;
}

inline std::string fixedInteger_13(std::string value) {
	const auto integerSize = value.find_first_of(".");
	switch (integerSize == std::string::npos? value.size(): integerSize) {
		case  0: return "0000000000000";
		case  1: return "000000000000"+value;
		case  2: return "00000000000"+value;
		case  3: return "0000000000"+value;
		case  4: return "000000000"+value;
		case  5: return "00000000"+value;
		case  6: return "0000000"+value;
		case  7: return "000000"+value;
		case  8: return "00000"+value;
		case  9: return "0000"+value;
		case 10: return "000"+value;
		case 11: return "00"+value;
		case 12: return "0"+value;
		case 13: return value;
	}
	return value;
}

inline std::string fixedDecimal_6(std::string valueWithoutDecimal) {
	switch (valueWithoutDecimal.size()) {
		case 0: return "0.000000";
		case 1: return "0.00000"+valueWithoutDecimal;
		case 2: return "0.0000"+valueWithoutDecimal;
		case 3: return "0.000"+valueWithoutDecimal;
		case 4: return "0.00"+valueWithoutDecimal;
		case 5: return "0.0"+valueWithoutDecimal;
		case 6: return "0."+valueWithoutDecimal;
	}
	return valueWithoutDecimal.insert(valueWithoutDecimal.size()-6, ".");
}

inline std::string trimTailingZero(std::string value) {
	assert(value.find_first_of(".") <= value.find_last_not_of("0"));
	return value.erase(value.find_last_not_of("0")+1);
}

inline std::string trimLeadingZero(std::string value) {
	assert(value.find_first_not_of("0") <= value.find_first_of("."));
	return value.erase(0, value.find_first_not_of("0"));
}

class str {
	private:
		const char *src;
		char tmp;
		int size;
	public:
		str(const char *data, int dataSize) {
			src = data;
			size = dataSize;
			tmp = src[size];
			const_cast<char*>(src)[size] = '\0';
		}
		str(): src(nullptr), tmp('\0'), size(0) {}
		const char* operator()(const char *data, int dataSize) {
			if (src != nullptr) const_cast<char*>(src)[size] = tmp;
			src = data;
			size = dataSize;
			tmp = src[size];
			const_cast<char*>(src)[size] = '\0';
			return data;
		}
		~str() {
			if (src != nullptr) {
				const_cast<char*>(src)[size] = tmp;
				src = nullptr;
			}
		}
};

// helper function to print a tuple of any size
	template<class Tuple, std::size_t N>
	struct TuplePrinter {
		static void print(const Tuple& t) 
		{
			TuplePrinter<Tuple, N-1>::print(t);
			std::cout << ", " << std::get<N-1>(t);
		}
	};
	template<class Tuple>
	struct TuplePrinter<Tuple, 1> {
		static void print(const Tuple& t) 
		{
			std::cout << std::get<0>(t);
		}
	};
	template<typename... Args, std::enable_if_t<sizeof...(Args) == 0, int> = 0>
	void print(const std::tuple<Args...>& t)
	{
		std::cout << "()\n";
	}
	template<typename... Args, std::enable_if_t<sizeof...(Args) != 0, int> = 0>
	void print(const std::tuple<Args...>& t)
	{
		std::cout << "(";
		TuplePrinter<decltype(t), sizeof...(Args)>::print(t);
		std::cout << ")\n";
	}
// end helper function

// #include <condition_variable>
// #include <future>
// #include <vector>
// using namespace std::literals::chrono_literals;
// struct timer_killer {
// 	template<class R, class P> bool wait_for( std::chrono::duration<R,P> const& time ) {
// 		std::unique_lock<std::mutex> lock(m);
// 		return !cv.wait_for(lock, time, [&]{return terminate;});
// 	}
// 	void kill() {
// 		std::unique_lock<std::mutex> lock(m);
// 		terminate=true;
// 		cv.notify_all();
// 	}
// private:
// 	std::condition_variable cv;
// 	std::mutex m;
// 	bool terminate = false;
// };

// timer_killer bob;
// int main() {
// 	std::vector< std::future<void> > tasks;
// 	tasks.push_back( std::async(std::launch::async,[]{
// 		while(bob.wait_for(500ms))
// 			std::cout << "thread 1 says hi\n";
// 		std::cout << "thread 1 dead\n";
// 	}));
// 	bob.wait_for(250ms);
// 	tasks.push_back( std::async(std::launch::async,[]{
// 		while(bob.wait_for(500ms))
// 			std::cout << "thread 2 says hi\n";
// 		std::cout << "thread 2 dead\n";
// 	}));
// 	bob.wait_for(1000ms);
// 	std::cout << "killing threads\n";
// 	bob.kill();
// 	for (auto&& f:tasks)
// 		f.wait();
// 	std::cout << "done\n";
// 	// your code goes here
// 	return 0;
// }

#endif