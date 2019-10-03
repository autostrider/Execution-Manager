#include "araofstream.h"

namespace api {

araofstream::araofstream () noexcept
  : std::ofstream()
{}

araofstream::araofstream (const char* filename, ios_base::openmode mode)
noexcept
  : std::ofstream(filename, mode)
{}

araofstream::araofstream (const std::string& filename, ios_base::openmode mode)
noexcept
  : std::ofstream(filename, mode)
{}

void araofstream::open (const char* filename, ios_base::openmode mode)
{
  std::ofstream::open(filename, mode);
}

void araofstream::open (const std::string& filename, ios_base::openmode mode)
{
  std::ofstream::open(filename, mode);
}

} // namespace api