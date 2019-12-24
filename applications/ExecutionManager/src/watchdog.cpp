#include "watchdog.hpp"
#include "execution_manager.hpp"
#include "msm_handler.hpp"
#include "application_handler.hpp"
#include "os_interface.hpp"

namespace ExecutionManager
{

Watchdog::Watchdog(ExecutionManager& em, MsmHandler& msmHandler)
	: m_em{em},
	  m_msmHandler{msmHandler},
		m_isRunning{true}
{ }

void Watchdog::start()
{
	m_worker = std::thread(&Watchdog::run, this);
}

void Watchdog::run()
{
  ApplicationHandler handler{std::make_unique<OsInterface>()};
	do
	{
    auto activeApps = m_em.getActiveApps();

    for (const auto& app: activeApps)
    {
      if (!handler.isActiveProcess(app))
      {

      }
    }

	} while (m_isRunning);
}

Watchdog::~Watchdog()
{
  m_isRunning = false;
  if (m_worker.joinable())
  {
    m_worker.join();
  }
}

}
