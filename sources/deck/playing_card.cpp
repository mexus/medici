#include "playing_card.h"

const std::map<PlayingCard::Suit, std::string> PlayingCard::suitsNames {
        {Hearts, "Червы"},
        {Diamonds, "Бубны"},
        {Clubs, "Крести"},
        {Spades, "Пики"}
};
const std::map<PlayingCard::Number, std::string> PlayingCard::numbersNames {
        {Six, "6"},
        {Seven, "7"},
        {Eight, "8"},
        {Nine, "9"},
        {Ten, "10"},
        {Jack, "Валет"},
        {Queen, "Дама"},
        {King, "Король"},
        {Ace, "Туз"},
};

const std::map<PlayingCard::Suit, std::string> PlayingCard::suitsNamesAbbrevations {
        {Hearts, "ч"},
        {Diamonds, "б"},
        {Clubs, "к"},
        {Spades, "п"}
};
const std::map<PlayingCard::Number, std::string> PlayingCard::numbersNamesAbbrevations {
        {Six, "6"},
        {Seven, "7"},
        {Eight, "8"},
        {Nine, "9"},
        {Ten, "X"},
        {Jack, "В"},
        {Queen, "Д"},
        {King, "К"},
        {Ace, "Т"},
};

PlayingCard::PlayingCard() : number(Six), suit(Hearts) {
}

PlayingCard::PlayingCard(const std::string& s) {
        bool set(false);
        std::string tmp(s);
        for (auto &pair : numbersNamesAbbrevations){
                const std::string &currentNumber = pair.second;
                size_t pos = tmp.find(currentNumber);
                if (pos != std::string::npos){
                        set = true;
                        number = pair.first;
                        tmp.erase(0, currentNumber.size());
                        break;
                }
        }
        if (!set)
                throw std::logic_error("Can't find a number for a {" + tmp + "}");
        set = false;
        for (auto &pair : suitsNamesAbbrevations){
                const std::string &currentSuit = pair.second;
                if (tmp == currentSuit){
                        set = true;
                        suit = pair.first;
                        break;
                }
        }
        if (!set)
                throw std::logic_error("Can't find a suit for a {" + tmp + "}");
}

PlayingCard::~PlayingCard() {
}

PlayingCard& PlayingCard::Next(bool &reachedEnd) {
        reachedEnd = false;
        if (number != Ace){
                number = static_cast<Number>(static_cast<int>(number) + 1);
        } else {
                number = Six;
                if (suit != Spades){
                        suit = static_cast<Suit>(static_cast<int>(suit) + 1);
                } else{
                        suit = Hearts;
                        reachedEnd = true;
                }
        }
        
        return *this;
}

std::string PlayingCard::Print(bool abbrevation) const {
        if (abbrevation)
                return numbersNamesAbbrevations.at(number) + suitsNamesAbbrevations.at(suit);
        else
                return numbersNames.at(number) + " " + suitsNames.at(suit);

}

std::string PlayingCard::PrintNumber(PlayingCard::Number n, bool abbrevation) {
        if (abbrevation)
                return numbersNamesAbbrevations.at(n);
        else
                return numbersNames.at(n);
}

std::string PlayingCard::PrintSuit(PlayingCard::Suit s, bool abbrevation) {
        if (abbrevation)
                return suitsNamesAbbrevations.at(s);
        else
                return suitsNames.at(s);
}

bool operator == (const PlayingCard& lhs, const PlayingCard& rhs){
        return lhs.number == rhs.number && lhs.suit == rhs.suit;

}

bool operator < (const PlayingCard& lhs, const PlayingCard& rhs){
        if (lhs.suit == rhs.suit){
                return lhs.number < rhs.number;
        } else
                return lhs.suit < rhs.suit;
}

PlayingCard::Number PlayingCard::GetNumber() const {
        return number;
}

PlayingCard::Suit PlayingCard::GetSuit() const {
        return suit;
}

