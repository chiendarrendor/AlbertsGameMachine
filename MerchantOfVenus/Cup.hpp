#include "Token.hpp"
#include "Serialize.hpp"


class Cup
{
public:
  Cup();

  Token Replace(Token i_t);

  const std::vector<Token>& GetContents() const;

private:
  std::vector<Token> m_tokens;

  SERIALIZE_FUNC
  {
    SERIALIZE(m_tokens);
  }
};

