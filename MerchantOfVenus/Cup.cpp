#include "Cup.hpp"
#include "Utilities.hpp"

Cup::Cup()
{
  // 2: 4 bonuses
  m_tokens.push_back(Token::Fare("2","10",120));
  m_tokens.push_back(Token::Demand("Designer Genes","2",40));
  m_tokens.push_back(Token::Demand("Designer Genes","2",40));
  m_tokens.push_back(Token::Demand("Psychotic Sculpture","2",90));
  
  // 3 5 bonuses (total 9)
  m_tokens.push_back(Token::Demand("Bionic Perfume","3",60));
  m_tokens.push_back(Token::Demand("Bionic Perfume","3",60));
  m_tokens.push_back(Token::Demand("Space Spice","3",60));
  m_tokens.push_back(Token::Demand("Space Spice","3",60));
  m_tokens.push_back(Token::Demand("Voil Silk","3",60));

  // 4a 3 bonuses (total 12)
  m_tokens.push_back(Token::Fare("4a","base",180));
  m_tokens.push_back(Token::Demand("Mulch Wine","4a",60));
  m_tokens.push_back(Token::Demand("Mulch Wine","4a",60));

  // 4b 5 bonuses (total 17)
  m_tokens.push_back(Token::Demand("Bionic Perfume","4b",60));
  m_tokens.push_back(Token::Demand("Space Spice","4b",40));
  m_tokens.push_back(Token::Demand("Space Spice","4b",40));
  m_tokens.push_back(Token::Demand("Space Spice","4b",40));
  m_tokens.push_back(Token::Demand("Space Spice","4b",40));

  // 5 6 bonuses (23)
  m_tokens.push_back(Token::Fare("5","base",140));
  m_tokens.push_back(Token::Demand("Mulch Wine","5",40));
  m_tokens.push_back(Token::Demand("Mulch Wine","5",40));
  m_tokens.push_back(Token::Demand("Mulch Wine","5",40));
  m_tokens.push_back(Token::Demand("Finest Dust","5",50));
  m_tokens.push_back(Token::Demand("Finest Dust","5",50));
                     
  // 6 3 bonuses (26)
  m_tokens.push_back(Token::Fare("6","4b",120));
  m_tokens.push_back(Token::Demand("Melf Pelts","6",50));
  m_tokens.push_back(Token::Demand("Melf Pelts","6",50));

  // 7a 3 bonuses (29)
  m_tokens.push_back(Token::Demand("Finest Dust","7a",50));
  m_tokens.push_back(Token::Demand("Finest Dust","7a",50));
  m_tokens.push_back(Token::Demand("Finest Dust","7a",50));
  
  // 7b 6 bonuses (35)
  m_tokens.push_back(Token::Fare("7b","base",110));
  m_tokens.push_back(Token::Demand("Finest Dust","7b",50));
  m_tokens.push_back(Token::Demand("Finest Dust","7b",50));
  m_tokens.push_back(Token::Demand("Immortal Grease","7b",50));
  m_tokens.push_back(Token::Demand("Immortal Grease","7b",50));
  m_tokens.push_back(Token::Demand("Immortal Grease","7b",50));
  
  // 8 1 bonus (36)
  m_tokens.push_back(Token::Demand("Chicle Liquor","8",60));
  
  // 9a 4 bonuses (40)
  m_tokens.push_back(Token::Demand("Chicle Liquor","9a",60));
  m_tokens.push_back(Token::Demand("Chicle Liquor","9a",60));
  m_tokens.push_back(Token::Demand("Immortal Grease","9a",50));
  m_tokens.push_back(Token::Demand("Immortal Grease","9a",50));

  // 9b 2 bonuses (42)
  m_tokens.push_back(Token::Fare("9b","5",110));
  m_tokens.push_back(Token::Demand("Impossible Furniture","9b",70));

  // 10 3 bonuses (45)
  m_tokens.push_back(Token::Fare("10","base",150));
  m_tokens.push_back(Token::Demand("Designer Genes","10",60));

  // base 3 bonuses (48 :-)
  m_tokens.push_back(Token::Fare("base","9a",110));
  m_tokens.push_back(Token::Fare("base","4a",140));
  m_tokens.push_back(Token::Fare("base","2",160));
}

Token Cup::Replace(Token i_token)
{
  m_tokens.push_back(i_token);
  myshuffle(m_tokens.begin(),m_tokens.end());
  Token result = m_tokens.back();
  m_tokens.pop_back();
  return result;
}

  
const std::vector<Token>& Cup::GetContents() const { return m_tokens; }
