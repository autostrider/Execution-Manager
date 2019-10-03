#include "arafstream.h"

namespace api {

arafstream::arafstream () noexcept
  : std::fstream()
{}

arafstream::arafstream (const char* filename, ios_base::openmode mode)
noexcept
  : std::fstream(filename, mode)
{}

arafstream::arafstream (const std::string& filename, ios_base::openmode mode)
noexcept
  : std::fstream(filename, mode)
{}

void arafstream::open (const char* filename, ios_base::openmode mode)
{
  std::fstream::open(filename, mode);
}

void arafstream::open (const std::string& filename, ios_base::openmode mode)
{
  std::fstream::open(filename, mode);
}

} // namespace api