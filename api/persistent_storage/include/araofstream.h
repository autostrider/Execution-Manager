#ifndef ARA_OFSTREAM_H
#define ARA_OFSTREAM_H

#include <fstream>

namespace api {

class araofstream : public std::ofstream
{
public:
  araofstream () noexcept;

  explicit araofstream (const char* filename,
                        ios_base::openmode mode = ios_base::out) noexcept;

  explicit araofstream (const std::string& filename,
                        ios_base::openmode mode = ios_base::out) noexcept;

  void open (const char* filename,
             ios_base::openmode mode = ios_base::out) noexcept(false);

  void open (const std::string& filename,
             ios_base::openmode mode = ios_base::out) noexcept(false);
};

} // namespace api
#endif // ARA_OFSTREAM_H
