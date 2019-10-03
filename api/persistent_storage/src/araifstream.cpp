#include "araifstream.h"

namespace api {

araifstream::araifstream () noexcept
  : std::ifstream()
{}

araifstream::araifstream (const char* filename, ios_base::openmode mode)
noexcept
  : std::ifstream(filename, mode)
{}

araifstream::araifstream (const std::string& filename, ios_base::openmode mode)
noexcept
  : std::ifstream(filename, mode)
{}

void araifstream::open (const char* filename, ios_base::openmode mode)
{
  std::ifstream::open(filename, mode);
}

void araifstream::open (const std::string& filename, ios_base::openmode mode)
{
  std::ifstream::open(filename, mode);
}

} // namespace api