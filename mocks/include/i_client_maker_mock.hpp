#ifndef I_CLIENT_MAKER_MOCK_HPP
#define I_CLIENT_MAKER_MOCK_HPP

#include <i_client_maker.hpp>

#include "gmock/gmock.h"

class IClientMakerMock : public IClientMaker
{
public:
    IClientMakerMock() = default;
    MOCK_CONST_METHOD1(makeClientPtr, std::shared_ptr<Client>(const int&));
};

#endif //I_CLIENT_MAKER_MOCK_HPP