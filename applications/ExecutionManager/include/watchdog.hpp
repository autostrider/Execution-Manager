#ifndef WATCHDOG_HPP
#define WATCHDOG_HPP

#include <atomic>
#include <thread>

namespace ExecutionManager
{

class ExecutionManager;
class MsmHandler;

class Watchdog
{
public:
	Watchdog(ExecutionManager& em, MsmHandler& msmHandler);
	void start();
	void close();
  ~Watchdog();
private:
	void run();
private:
	ExecutionManager& m_em;
	MsmHandler& m_msmHandler;
	std::atomic<bool> m_isRunning;
	std::thread m_worker;
};

}

#endif // WATCHDOG_HPP
