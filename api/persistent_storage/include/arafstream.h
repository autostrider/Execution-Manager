#ifndef ARA_FSTREAM_H
#define ARA_FSTREAM_H

#include <fstream>

namespace api {

class arafstream : public std::fstream
{
public:
  arafstream () noexcept;

  explicit arafstream (const char* filename,
                      ios_base::openmode mode =
                      ios_base::in | ios_base::out) noexcept;

  explicit arafstream (const std::string& filename,
                      ios_base::openmode mode =
                      ios_base::in | ios_base::out) noexcept;

  void open (const char* filename,
            ios_base::openmode mode =
            ios_base::in | ios_base::out) noexcept(false);

  void open (const std::string& filename,
            ios_base::openmode mode =
            ios_base::in | ios_base::out) noexcept(false);
};

} // namespace api
#endif // ARA_FSTREAM_H
