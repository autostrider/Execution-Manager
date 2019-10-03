#ifndef ARA_IFSTREAM_H
#define ARA_IFSTREAM_H

#include <fstream>

namespace api {

class araifstream : public std::ifstream
{
public:
  araifstream () noexcept;

  explicit araifstream (const char* filename,
                        ios_base::openmode mode = ios_base::in) noexcept;

  explicit araifstream (const std::string& filename,
                        ios_base::openmode mode = ios_base::in) noexcept;

  void open (const char* filename,
             ios_base::openmode mode = ios_base::in) noexcept(false);

  void open (const std::string& filename,
             ios_base::openmode mode = ios_base::in) noexcept(false);
};

} // namespace api
#endif // ARA_IFSTREAM_H
