#include "charSerial.h"
#include <iostream>
using namespace std;
charSerial::charSerial() : serializable() {
    character = '\0';
}

charSerial::charSerial(char character) : serializable() {
    this->character = character;
}

charSerial::charSerial(const charSerial& other) {
    character = other.getCharacter();
}

charSerial::~charSerial() {
    character = '\0';
}

charSerial charSerial::operator=(const charSerial& other) {
    charSerial aux(other);
    if(this == &other) {
        return *this;
    }
    character = other.getCharacter();
    return aux;
}
bool charSerial::operator==(const charSerial& other) const {
    return upperCase(character) == upperCase(other.getCharacter());
}

bool charSerial::operator>=(const charSerial& other) const {
    return upperCase(character) >= upperCase(other.getCharacter());
}

bool charSerial::operator<=(const charSerial& other) const {
    return upperCase(character) <= upperCase(other.getCharacter());
}

bool charSerial::operator!=(const charSerial& other) const {
    return upperCase(character) != upperCase(other.getCharacter());
}

bool charSerial::operator>(const charSerial& other) const {
     return upperCase(character) > upperCase(other.getCharacter());
}

bool charSerial::operator<(const charSerial& other) const {
    return upperCase(character) < upperCase(other.getCharacter());
}

char charSerial::upperCase(const char character)const {
    if(character >= 65 && character <= 90){
        return character;
    }
    if(character >= 97 && character <= 122){
        return character - 32;
    }else{
        return character;
    }
}

void charSerial::setCharacter(char character) {
    this->character = character;
}

char charSerial::getCharacter() const {
    return character;
}

string charSerial::toString() {
    string repr = "";
    repr += character;
    return repr;
}

void charSerial::fromString(string repr) {
    repr.copy(reinterpret_cast<char*>(&character), sizeof(character), 0);
}

unsigned long long int charSerial::size() const {
    return sizeof(character);
}
