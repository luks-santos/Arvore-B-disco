#include "header.h"

header::header() : serializable() {
    type = "***";
    version = 0;
    root = 0;
    firstDeleted = 0;
}

header::header(const string t, unsigned int v) : serializable() {
    type = t.substr(0, 3);
    version = v;
    root = 0;
    firstDeleted = 0;
}

header::header(const header &h) {
    type = h.getType();
    firstDeleted = h.getFirstDeleted();
    root = h.getRoot();
    version = h.getVersion();
}

header::~header() {
   //dtor
}

header header::operator=(const header &h) {
    header aux(h);
    if (this == &h)
        return *this;

    type = h.getType();
    firstDeleted = h.getFirstDeleted();
    root = h.getRoot();
    version = h.getVersion();
    return aux;
}

bool header::operator==(const header &h) {
    return type == h.getType() && version == h.getVersion() &&
            firstDeleted == h.getFirstDeleted() &&
            root == h.getRoot();
}

unsigned long long int header::getFirstDeleted() const {
    return firstDeleted;
}

void header::setFirstDeleted(unsigned long long int r) {
    firstDeleted = r;
}

unsigned long long int header::getRoot() const {
    return root;
}

void header::setRoot(unsigned long long int r) {
    root = r;
}

string header::getType() const {
    return type;
}

void header::setType(string t) {
    type = t.substr(0, 3);
}

unsigned int header::getVersion() const {
    return version;
}

void header::setVersion(unsigned int v) {
    version = v;
}

string header::toString() {
    string aux = "";
    aux += type;
    aux += string(reinterpret_cast<char*>(&version), sizeof(version));
    aux += string(reinterpret_cast<char*>(&root), sizeof(root));
    aux += string(reinterpret_cast<char*>(&firstDeleted), sizeof(firstDeleted));
    return aux;
}

void header::fromString(string repr) {
    int pos = 0;
    type = repr.substr(pos, 3);
    pos += type.length();
    repr.copy(reinterpret_cast<char*>(&version), sizeof(version), pos);
    pos += sizeof(version);
    repr.copy(reinterpret_cast<char*>(&root), sizeof(root), pos);
    pos += sizeof(root);
    repr.copy(reinterpret_cast<char*>(&firstDeleted), sizeof(firstDeleted), pos);
}

unsigned long long int header::size() const {
    return type.length() + sizeof(version) + sizeof(root) + sizeof(firstDeleted);
}
